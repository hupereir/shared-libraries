/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "TextEditor.h"
#include "BaseContextMenu.h"
#include "BaseFindDialog.h"
#include "BaseFindWidget.h"
#include "BaseIconNames.h"
#include "BaseReplaceDialog.h"
#include "BaseReplaceWidget.h"
#include "Color.h"
#include "CppUtil.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "KeyModifier.h"
#include "LineEditor.h"
#include "LineNumberDisplay.h"
#include "QtUtil.h"
#include "RegExpUtil.h"
#include "SelectLineDialog.h"
#include "SelectLineWidget.h"
#include "Singleton.h"
#include "StandardAction.h"
#include "TextBlockData.h"
#include "TextBlockRange.h"
#include "TextDocument.h"
#include "TextEditorMarginWidget.h"
#include "TextSeparator.h"
#include "Util.h"
#include "XmlOptions.h"


#include <QApplication>
#include <QAbstractTextDocumentLayout>
#include <QDrag>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextStream>
#include <QToolTip>

#include <numeric>

//______________________________________________
TextSelection& TextEditor::lastSelection()
{
    static TextSelection selection;
    return selection;
}

//______________________________________________
TextEditor::TextEditor( QWidget *parent ):
    BaseEditor( parent ),
    Counter( QStringLiteral("TextEditor") ),
    marginWidget_( new TextEditorMarginWidget( this ) ),
    boxSelection_( this ),
    cursorMonitor_( viewport() ),
    removeLineBuffer_( this ),
    clickCounter_( this, 4 )
{

    Debug::Throw( QStringLiteral("TextEditor::TextEditor.\n") );

    // set customized document
    auto document( new TextDocument(0) );
    Base::Key::associate( this, document );

    #ifdef QT_USE_PLAIN_TEXT_EDIT
    // set document layout
    document->setDocumentLayout( new QPlainTextDocumentLayout( document ) );
    #endif

    // assign
    setDocument( document );

    // paragraph highlight
    blockHighlight_ = new BlockHighlight( this );

    // actions
    _installActions();

    // line number
    lineNumberDisplay_ = new LineNumberDisplay( this );

    connect( this, &QTextEdit::cursorPositionChanged, &blockHighlight(), &BlockHighlight::highlight );
    connect( this, &QTextEdit::copyAvailable, this, &TextEditor::_updateSelectionActions );
    connect( this, &QTextEdit::selectionChanged, this, &TextEditor::_synchronizeSelection );
    connect( this, &QTextEdit::selectionChanged, this, &TextEditor::_updateClipboard );
    connect( this, &QTextEdit::cursorPositionChanged, this, &TextEditor::_synchronizeSelection );

    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &TextEditor::_updateConfiguration );

    // track changes of block counts
    connect( TextEditor::document(), &QTextDocument::blockCountChanged, this, &TextEditor::_blockCountChanged );
    connect( TextEditor::document(), &QTextDocument::contentsChanged, this, &TextEditor::_updateContentActions );
    connect( TextEditor::document(), &QTextDocument::contentsChanged, marginWidget_, &TextEditorMarginWidget::setDirty );

    // update configuration
    _updateConfiguration();
    _blockCountChanged(0);
    _updateContentActions();

    marginWidget_->show();

}

//________________________________________________
TextEditor::~TextEditor()
{

    Debug::Throw() << "TextEditor::~TextEditor - key: " << key() << Qt::endl;

    // cast document
    auto document( qobject_cast<TextDocument*>( TextEditor::document() ) );
    if( document && Base::KeySet<TextEditor>( document ).size() == 1 ) document->deleteLater();

    // update associates synchronization flags
    Base::KeySet<TextEditor> editors( this );

    // nothing to be done if no associates
    if( editors.empty() ) return;

    // keep position of current cursor
    int position( textCursor().position() );
    int anchor( textCursor().anchor() );

    // clear associations to other text editors
    // and reset synchronization flag
    clearAssociations<TextEditor>();
    setSynchronized( false );

    // reset Text document to avoid deletion while deleting this editor
    setDocument( new QTextDocument );

    // keep reference to first associate
    TextEditor &editor( **editors.begin() );

    // recreate an appropriate cursor
    QTextCursor cursor( editor.document() );
    cursor.setPosition( anchor );
    cursor.setPosition( position, QTextCursor::KeepAnchor );
    editor.setTextCursor( cursor );

    // turn off synchronization
    if( editors.size() == 1 ) editor.setSynchronized( false );

}

//________________________________________________
int TextEditor::blockCount() const
{
    Debug::Throw( QStringLiteral("TextEditor::blockCount.\n") );
    TextBlockRange range( document() );
    return std::accumulate( range.begin(), range.end(), 0,
        [this]( int sum, const QTextBlock& block )
        { return std::move(sum) + blockCount( block ); } );
}

//________________________________________________
TextPosition TextEditor::textPosition()
{

    auto cursor( textCursor() );

    // calculate index
    TextPosition out;
    out.index() = cursor.columnNumber();
    out.paragraph() = cursor.blockNumber();
    return out;

}

//________________________________________________
bool TextEditor::isCursorVisible() const
{

    QRect cursor_rect( cursorRect() );
    QRect rect( viewport()->rect() );
    return rect.intersects( cursor_rect );

}

//______________________________________________________________________
bool TextEditor::hasSelection() const
{ return textCursor().hasSelection() || !boxSelection_.empty(); }

//______________________________________________________________________
TextSelection TextEditor::selection() const
{
    Debug::Throw( QStringLiteral("TextEditor::selection.\n") );

    // copy last selection
    TextSelection out( QLatin1String("") );

    // copy attributes from last selection
    out.setFlag( TextSelection::CaseSensitive, lastSelection().hasFlag( TextSelection::CaseSensitive ) );
    out.setFlag( TextSelection::EntireWord, lastSelection().hasFlag( TextSelection::EntireWord ) );

    // try set from current selection
    QString text;
    if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) {

        Debug::Throw( QStringLiteral("TextEditor::selection - from clipboard.\n") );
        out.setText( text );

    } else if( textCursor().hasSelection() ) {

        Debug::Throw() << "TextEditor::selection - from cursor: " << textCursor().selectedText() << Qt::endl;
        out.setText( textCursor().selectedText() );

    } else {

        Debug::Throw( QStringLiteral("TextEditor::selection - from last selection.\n") );
        out.setText( lastSelection().text() );

    }

    return out;

}

//________________________________________________
void TextEditor::setPlainText( const QString& text )
{
    Debug::Throw( QStringLiteral("TextEditor::setPlainText.\n") );

    lineNumberDisplay_->clear();

    bool enabled( blockHighlight_->isEnabled() );
    blockHighlight_->setEnabled( false );
    BaseEditor::setPlainText( text );
    blockHighlight_->setEnabled( enabled );

}

//________________________________________________
void TextEditor::setHtml( const QString& text )
{
    Debug::Throw( QStringLiteral("TextEditor::setHtml.\n") );

    bool enabled( blockHighlight_->isEnabled() );
    blockHighlight_->setEnabled( false );

    #ifdef QT_USE_PLAIN_TEXT_EDIT
    BaseEditor::setPlainText( text );
    #else
    BaseEditor::setHtml( text );
    #endif

    blockHighlight_->setEnabled( enabled );

}

//___________________________________________________________________________
void TextEditor::paintMargin( QPainter& painter )
{

    int height( TextEditor::height() - 2*frameWidth() );
    if( horizontalScrollBar()->isVisible() ) { height -= horizontalScrollBar()->height() + 2; }

    // clip
    painter.setClipRect( QRect( 0, 0, leftMargin_, height ), Qt::IntersectClip );
    painter.setPen( Qt::NoPen );

    painter.translate( 0, -verticalScrollBar()->value() );

    // draw current block rect
    if( blockHighlightAction_->isEnabled() && blockHighlightAction_->isChecked() && _currentBlockRect().isValid() )
    {

        painter.setBrush( palette().color( QPalette::AlternateBase ) );
        painter.drawRect( _currentBlockRect() );

    }

    if( marginWidget_->drawVerticalLine() ) {
        painter.setBrush( QBrush( marginWidget_->foregroundColor(), Qt::Dense4Pattern ) );
        painter.drawRect( leftMargin_-1, verticalScrollBar()->value(), 1, height+verticalScrollBar()->value() );
    }

    // set brush and pen suitable to further painting
    painter.setBrush( Qt::NoBrush );
    painter.setPen(marginWidget_->foregroundColor()  );

    // draw lines
    if(
        lineNumberDisplay_ &&
        showLineNumberAction_ &&
        showLineNumberAction_->isVisible() &&
        showLineNumberAction_->isChecked() )
    { lineNumberDisplay_->paint( painter ); }

}

//________________________________________________
void TextEditor::selectWord()
{
    Debug::Throw( QStringLiteral("TextEditor::selectWord.\n") );

    // retrieve text cursor, block and text
    auto cursor( textCursor() );
    QTextBlock block( cursor.block() );
    QString text( cursor.block().text() );

    // retrieve local cursor position in block
    int localPosition( cursor.position() - block.position() );
    int begin = localPosition;
    int end = localPosition;

    // parse text
    if( TextSeparator::get().base().contains( text[begin] ) )
    {

        // see if cursor is in base separator list
        while( begin > 0 &&  TextSeparator::get().base().contains( text[begin-1] ) ) begin--;
        while( end < text.size() && TextSeparator::get().base().contains( text[end] ) ) end++;

    } else if( TextSeparator::get().extended().contains( text[begin] ) ) {

        // see if cursor is in extended separator list
        while( begin > 0 &&  TextSeparator::get().extended().contains( text[begin-1] ) ) begin--;
        while( end < text.size() && TextSeparator::get().extended().contains( text[end] ) ) end++;

    } else {

        // cursor is in word
        while( begin > 0 && !TextSeparator::get().all().contains( text[begin-1] ) ) begin--;
        while( end < (int)text.size() && !TextSeparator::get().all().contains( text[end] ) ) end++;

    }

    // move cursor to begin of selection
    cursor.movePosition( QTextCursor::Left, QTextCursor::MoveAnchor, localPosition-begin );
    cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, end-begin );

    // assign cursor to Text editor
    setTextCursor( cursor );

    return;

}

//________________________________________________
void TextEditor::selectLine()
{
    Debug::Throw( QStringLiteral("TextEditor::selectLine.\n") );

    auto cursor( textCursor() );
    auto block( cursor.block() );
    int begin( block.position() );
    int end( block.position() + block.length() );
    if( !block.next().isValid() ) end--;

    cursor.setPosition( begin );
    cursor.setPosition( end, QTextCursor::KeepAnchor );

    // assign cursor to text editor and make sure it is visible
    setTextCursor( cursor );
    ensureCursorVisible();

    // updateClipboard need to be called manually
    // because somehow the selectionChanged signal is not caught.
    _updateClipboard();

    return;

}

//________________________________________________
void TextEditor::mergeCurrentCharFormat( const QTextCharFormat& format )
{

    static const QRegularExpression regexp( QStringLiteral( "\\s+$" ) );

    auto cursor( textCursor() );
    if( cursor.hasSelection() )
    {

        // get selection, look for trailing spaces
        const auto match( regexp.match( cursor.selectedText() ) );
        if( match.hasMatch() )
        {

            // create local cursor, copy current, in proper order
            QTextCursor local( document() );
            local.beginEditBlock();
            local.setPosition( qMin( cursor.position(), cursor.anchor() ), QTextCursor::MoveAnchor );
            local.setPosition( qMax( cursor.position(), cursor.anchor() ), QTextCursor::KeepAnchor );
            local.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, match.capturedLength() );

            local.mergeCharFormat( format );
            local.endEditBlock();

            return;

        }

    } else if( boxSelection_.state() == BoxSelection::State::Finished ) {

        // process box selection
        // boxSelection_.setCharFormat( format );
        boxSelection_.mergeCharFormat( format );
        return;

    }

    BaseEditor::mergeCurrentCharFormat( format );
    return;

}

//________________________________________________
void TextEditor::synchronize( TextEditor* editor )
{
    Debug::Throw( QStringLiteral("TextEditor::synchronize.\n") );

    // retrieve and cast old document
    auto document( qobject_cast<TextDocument*>( BaseEditor::document() ) );

    // set font
    setFont( editor->font() );

    // assign new document and associate
    setDocument( editor->document() );
    Base::Key::associate( this, qobject_cast<TextDocument*>( editor->document() ) );

    // delete old document, if needed
    if( document && Base::KeySet<TextEditor>( document ).size() == 1 ) delete document;

    // set synchronization flag
    editor->setSynchronized( true );
    setSynchronized( true );

    // synchronize cursor position
    setTextCursor( editor->textCursor() );

    // restore scrollbar positions
    horizontalScrollBar()->setValue( editor->horizontalScrollBar()->value() );
    verticalScrollBar()->setValue( editor->verticalScrollBar()->value() );

    // synchronize tab emulation
    _setTabSize( editor->emulatedTabCharacter().size() );
    tabEmulationAction_->setChecked( editor->tabEmulationAction_->isChecked() );

    // synchronize wrap mode
    wrapModeAction_->setChecked( editor->wrapModeAction_->isChecked() );

    // track changes of block counts
    lineNumberDisplay_->synchronize( &editor->_lineNumberDisplay() );
    connect( TextEditor::document(), &QTextDocument::blockCountChanged, this, &TextEditor::_blockCountChanged );
    connect( TextEditor::document(), &QTextDocument::contentsChanged, this, &TextEditor::_updateContentActions );
    connect( TextEditor::document(), &QTextDocument::contentsChanged, &_marginWidget(), &TextEditorMarginWidget::setDirty );

    // margin
    _setLeftMargin( editor->leftMargin_ );

    return;

}

//_____________________________________________________________________
bool TextEditor::setActive( bool active )
{

    Debug::Throw( QStringLiteral("TextEditor::setActive.\n") );

    // check if value is changed
    if( isActive() == active ) return false;
    active_ = active;
    return true;

}

//__________________________________________________________________
void TextEditor::showReplacements( int counts )
{

    Debug::Throw( QStringLiteral("TextEditor::showReplacements.\n") );

    QString buffer;
    if( !counts ) buffer = tr( "String not found." );
    else if( counts == 1 ) buffer = tr( "One replacement performed" );
    else buffer = tr( "%1 replacements performed" ).arg( counts );
    InformationDialog dialog( this, buffer );
    dialog.setWindowTitle( tr( "Replace in Text" ) );
    dialog.centerOnWidget( qApp->activeWindow() ).exec();

    return;

}

//__________________________________________________________________
void TextEditor::setTrackAnchors( bool value )
{
    Debug::Throw( QStringLiteral("TextEditor::setTrackAnchors.\n") );
    trackAnchors_ = value;
    if( value ) setMouseTracking( true );
}

//__________________________________________________________________
void TextEditor::setReadOnly( bool readOnly )
{
    Debug::Throw( QStringLiteral("TextEditor::setReadOnly.\n") );
    BaseEditor::setReadOnly( readOnly );
    _updateReadOnlyActions( readOnly );
    if( readOnly ) document()->setModified( false );
}

//____________________________________________________________________
void TextEditor::resetUndoRedoStack()
{
    if( isReadOnly() || !document()->isUndoRedoEnabled() ) return;

    Debug::Throw(QStringLiteral(" TextEditor::resetUndoRedoStack.\n"));
    document()->setUndoRedoEnabled( false );
    document()->setUndoRedoEnabled( true );

}

//______________________________________________________________________________
void TextEditor::installContextMenuActions( BaseContextMenu* menu, bool allActions )
{

    Debug::Throw( QStringLiteral("TextEditor::installContextMenuActions.\n") );

    // anchors
    if( trackAnchors_ && !anchor().isEmpty() )
    {
        menu->addAction( copyLinkAction_ );
        menu->addSeparator();
    }

    // wrapping
    menu->addAction( showLineNumberAction_ );
    menu->addAction( wrapModeAction_ );
    menu->addSeparator();

    if( allActions )
    {
        menu->addAction( undoAction_ );
        menu->addAction( redoAction_ );
        menu->addSeparator();
    }

    menu->addAction( cutAction_ );
    menu->addAction( copyAction_ );
    menu->addAction( pasteAction_ );
    if( acceptRichText() )
    { menu->addAction( pasteUnformatedAction_ ); }

    menu->addAction( clearAction_ );
    menu->addSeparator();

    menu->addAction( selectAllAction_ );
    menu->addAction( upperCaseAction_ );
    menu->addAction( lowerCaseAction_ );
    menu->addSeparator();

    menu->addAction( findAction_ );
    if( allActions )
    {
        menu->addAction( findAgainAction_ );
        menu->addAction( findSelectionAction_);
        menu->addSeparator();
    }

    menu->addAction( replaceAction_ );

    if( allActions )
    {
        menu->addAction( replaceAgainAction_ );
        menu->addAction( gotoLineAction_);
    }

    return;

}

//______________________________________________________________________
void TextEditor::createFindWidget( bool compact )
{

    Debug::Throw( QStringLiteral("TextEditor::createFindWidget.\n") );
    if( !findWidget_ )
    {
        findWidget_ = new BaseFindWidget( this, compact );
        connect( findWidget_, &BaseFindWidget::find, this, &TextEditor::find );
        connect( this, &TextEditor::matchFound, findWidget_, &BaseFindWidget::matchFound );
        connect( this, &TextEditor::noMatchFound, findWidget_, &BaseFindWidget::noMatchFound );
    }

    return;

}

//______________________________________________________________________
void TextEditor::createReplaceWidget( bool compact )
{

    Debug::Throw( QStringLiteral("TextEditor::createReplaceWidget.\n") );
    if( !replaceWidget_ )
    {
        replaceWidget_ = new BaseReplaceWidget( this, compact );
        connect( replaceWidget_, &BaseReplaceWidget::find, this, &TextEditor::find );
        connect( replaceWidget_, &BaseReplaceWidget::replace, this, &TextEditor::replace );
        connect( replaceWidget_, &BaseReplaceWidget::replaceInWindow, this, QOverload<const TextSelection&>::of( &TextEditor::replaceInWindow ) );
        connect( replaceWidget_, &BaseReplaceWidget::replaceInSelection, this, QOverload<const TextSelection&>::of( &TextEditor::replaceInSelection ) );
        connect( replaceWidget_, &BaseReplaceWidget::menuAboutToShow, this, &TextEditor::_updateReplaceInSelection );
        connect( this, &TextEditor::matchFound, replaceWidget_, &BaseReplaceWidget::matchFound );
        connect( this, &TextEditor::noMatchFound, replaceWidget_, &BaseReplaceWidget::noMatchFound );
    }

    return;

}

//________________________________________________
void TextEditor::createSelectLineWidget( bool compact )
{
    if( !selectLineWidget_ )
    {
        Debug::Throw( QStringLiteral("TextEditor::createSelectLineWidget.\n") );
        selectLineWidget_ = new SelectLineWidget( this, compact );
        connect( selectLineWidget_, &SelectLineWidget::lineSelected, this, QOverload<int>::of( &TextEditor::selectLine ) );
        connect( this, &TextEditor::lineFound, selectLineWidget_, &SelectLineWidget::matchFound );
        connect( this, &TextEditor::lineNotFound, selectLineWidget_, &SelectLineWidget::noMatchFound );
   }
}

//___________________________________________________________________________
void TextEditor::setBackground( const QTextBlock& constBlock, const QColor& color )
{

    Debug::Throw( QStringLiteral("TextEditor::setBackground.\n") );

    // local copy
    auto block( constBlock );

    // try retrieve data or create
    auto data( static_cast<TextBlockData*>( block.userData() ) );
    if( !data ) block.setUserData( data = new TextBlockData );

    // try assign color
    if( data->setBackground( color ) && updatesEnabled() )
    { document()->markContentsDirty(block.position(), block.length()-1); }
    return;

}

//___________________________________________________________________________
void TextEditor::clearBackground( const QTextBlock& block )
{

    Debug::Throw( QStringLiteral("TextEditor::clearBackground.\n") );
    auto data( static_cast<TextBlockData*>( block.userData() ) );
    if( data && data->hasFlag( TextBlock::HasBackground ) && data->setBackground( QColor() ) && updatesEnabled()  )
    { document()->markContentsDirty(block.position(), block.length()-1); }

    return;
}

//___________________________________________________________________________
void TextEditor::clearAllBackgrounds()
{
    Debug::Throw( QStringLiteral("TextEditor::clearAllBackgrounds.\n") );
    for( const auto& block:TextBlockRange( document() ) )
    { clearBackground( block ); }
}

//________________________________________________
void TextEditor::cut()
{

    Debug::Throw( QStringLiteral("TextEditor::cut.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    if( boxSelection_.state() == BoxSelection::State::Finished )
    {

        boxSelection_.toClipboard( QClipboard::Clipboard );
        boxSelection_.removeSelectedText();
        boxSelection_.clear();
        emit copyAvailable( false );

    } else BaseEditor::cut();

    return;
}

//________________________________________________
void TextEditor::copy()
{

    Debug::Throw( QStringLiteral("TextEditor::copy.\n") );
    if( boxSelection_.state() == BoxSelection::State::Finished ) boxSelection_.toClipboard( QClipboard::Clipboard );
    else BaseEditor::copy();

}

//________________________________________________
void TextEditor::paste()
{

    Debug::Throw( QStringLiteral("TextEditor::paste.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    if( boxSelection_.state() == BoxSelection::State::Finished )
    {

        boxSelection_.fromClipboard( QClipboard::Clipboard );
        boxSelection_.clear();

    } else BaseEditor::paste();

}


//________________________________________________
void TextEditor::pasteUnformated()
{

    Debug::Throw( QStringLiteral("TextEditor::pasteUnformated.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    if( boxSelection_.state() == BoxSelection::State::Finished )
    {

        boxSelection_.fromClipboard( QClipboard::Clipboard );
        boxSelection_.clear();

    } else {

        const auto md = QGuiApplication::clipboard()->mimeData(QClipboard::Clipboard);
        if( md && md->hasText() )
        {
            const auto text = md->text();
            if( !text.isNull() )
            {
                const auto fragment = QTextDocumentFragment::fromPlainText(text);
                textCursor().insertFragment(fragment);
                ensureCursorVisible();

            }
        }
    }

}

//________________________________________________
void TextEditor::upperCase()
{
    Debug::Throw( QStringLiteral("TextEditor::upperCase.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    auto cursor = textCursor();
    if( cursor.hasSelection() )
    {

        // process standard selection
        cursor.insertText( cursor.selectedText().toUpper() );

    } else if( boxSelection_.state() == BoxSelection::State::Finished ) {

        // process box selection
        boxSelection_.toUpper();

    }

    return;

}

//________________________________________________
void TextEditor::lowerCase()
{

    Debug::Throw( QStringLiteral("TextEditor::lowerCase.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    auto cursor = textCursor();

    if( cursor.hasSelection() )
    {

        // process standard selection
        cursor.insertText( cursor.selectedText().toLower() );

    } else if( boxSelection_.state() == BoxSelection::State::Finished ) {

        // process box selection
        boxSelection_.toLower();

    }

    return;

}

//______________________________________________________________________
void TextEditor::find( const TextSelection &selection )
{
    Debug::Throw( QStringLiteral("TextEditor::find.\n") );
    bool found( selection.hasFlag( TextSelection::Backward ) ? _findBackward( selection, true ):_findForward( selection, true ) );
    if( found ) emit matchFound();
    else emit noMatchFound();
}

//______________________________________________________________________
void TextEditor::findSelectionForward()
{
    Debug::Throw( QStringLiteral("TextEditor::findSelectionForward.\n") );
    _findForward( selection(), true );
}

//______________________________________________________________________
void TextEditor::findSelectionBackward()
{
    Debug::Throw( QStringLiteral("TextEditor::findSelectionBackward.\n") );
    _findBackward( selection(), true );
}

//______________________________________________________________________
void TextEditor::findAgainForward()
{
    Debug::Throw( QStringLiteral("TextEditor::findAgainForward.\n") );
    _findForward( lastSelection(), true );
}

//______________________________________________________________________
void TextEditor::findAgainBackward()
{
    Debug::Throw( QStringLiteral("TextEditor::findAgainBackward.\n") );
    _findBackward( lastSelection(), true );
}

//______________________________________________________________________
void TextEditor::replace( const TextSelection &selection )
{

    Debug::Throw( QStringLiteral("TextEditor::replace.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    // see if current selection match
    // perform replacement if yes
    auto cursor = textCursor();
    bool accepted( true );
    accepted &= cursor.hasSelection();
    if( selection.hasFlag( TextSelection::RegExp ) )
    {

        accepted &= Base::exactMatch( QRegularExpression( selection.text() ), cursor.selectedText() );

    } else {

        accepted &= ( !cursor.selectedText().compare(
            selection.text(),
            selection.hasFlag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

    }

    if( accepted )
    {
        cursor.insertText( selection.replaceText() );
        setTextCursor( cursor );
    }

    // try find next occurence
    find( selection );
    return;

}

//______________________________________________________________________
void TextEditor::replaceAgainForward()
{
    Debug::Throw( QStringLiteral("TextEditor::replaceAgainForward.\n") );
    auto selection = lastSelection();
    selection.setFlag( TextSelection::Backward, false );
    replace( selection );
}

//______________________________________________________________________
void TextEditor::replaceAgainBackward()
{
    Debug::Throw( QStringLiteral("TextEditor::replaceAgainBackward.\n") );
    auto selection = lastSelection();
    selection.setFlag( TextSelection::Backward, true );
    replace( selection );
}

//______________________________________________________________________
int TextEditor::replaceInSelection( const TextSelection &selection, bool showDialog )
{

    Debug::Throw( QStringLiteral("TextEditor::replaceInSelection.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return 0;

    // progress dialog
    if( showDialog ) _createProgressDialog();

    int counts(0);

    if( boxSelection_.state() == BoxSelection::State::Finished )
    {

        Debug::Throw( QStringLiteral("TextEditor::replaceInSelection - box selection.\n") );
        auto cursors( boxSelection_.cursorList() );
        counts = std::accumulate( cursors.begin(), cursors.end(), 0,
            [this, &selection]( int count, QTextCursor cursor )
            { return std::move(count) + _replaceInRange( selection, cursor, CursorMode::Move ); } );

        boxSelection_.clear();

    } else {

        Debug::Throw( QStringLiteral("TextEditor::replaceInSelection - normal selection.\n") );
        auto cursor = textCursor();
        counts = _replaceInRange( selection, cursor, CursorMode::Expand );

    }

    Debug::Throw( QStringLiteral("TextEditor::replaceInSelection - done.\n") );
    if( showDialog ) showReplacements( counts );
    return counts;

}

//______________________________________________________________________
int TextEditor::replaceInWindow( const TextSelection &selection, bool showDialog )
{

    Debug::Throw( QStringLiteral("TextEditor::replaceInWindow.\n") );

    // progress dialog
    if( showDialog ) _createProgressDialog();

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return 0;

    auto cursor = textCursor();
    cursor.movePosition( QTextCursor::Start );
    cursor.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
    auto counts = _replaceInRange( selection, cursor, CursorMode::Expand );

    if( showDialog ) showReplacements( counts );
    return counts;

}

//________________________________________________
void TextEditor::selectLine( int index )
{

    Debug::Throw() << "TextEditor::selectLine - index: " << index << Qt::endl;
    auto block = document()->begin();
    for( int localIndex = 0;localIndex < index && block.isValid(); block = block.next(), localIndex++ )
    {}

    if( block.isValid() )
    {
        // create cursor at begin of block. move to end of block, keeping anchor unchanged
        QTextCursor cursor( block );
        cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );

        // assign to editor
        setTextCursor( cursor );

        emit lineFound();

    } else {

        setTextCursor( QTextCursor( document()->end() ) );
        emit lineNotFound();

    }

    return;

}

//________________________________________________
void TextEditor::removeLine()
{

    Debug::Throw( QStringLiteral("TextEditor::removeLine.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    auto cursor = textCursor();
    cursor.movePosition( QTextCursor::StartOfBlock, QTextCursor::MoveAnchor );

    // create cursor selection, depending on whether next block is valid or not
    if( cursor.block().next().isValid() )
    {

        cursor.movePosition( QTextCursor::NextBlock, QTextCursor::KeepAnchor );
        removeLineBuffer_.append( cursor.selectedText() );

    } else {

        // move to previous character
        if( cursor.block().previous().isValid() )
        {
            cursor.movePosition( QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor );
            cursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
        }

        // move to end of current block
        cursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
        removeLineBuffer_.append( cursor.selectedText() );

    }

    setUpdatesEnabled( false );
    setTextCursor( cursor );
    cut();
    setUpdatesEnabled( true );

}

//________________________________________________
void TextEditor::clear()
{
    Debug::Throw( QStringLiteral("TextEditor::clear.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    setUpdatesEnabled( false );
    selectAll();
    cut();
    setUpdatesEnabled( true );

}

//________________________________________________
bool TextEditor::event( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::ToolTip:
        {
            auto helpEvent( static_cast<QHelpEvent*>(event) );
            auto position( helpEvent->pos() );
            auto globalPosition( helpEvent->globalPos() );
            QString anchor;
            if( trackAnchors_ && !( anchor = anchorAt( position ) ).isEmpty() )
            {
                QRect rect;
                auto cursor( cursorForPosition( position ) );
                auto block( cursor.block() );

                for( auto&& it = block.begin(); it != block.end(); ++it)
                {
                    const auto fragment = it.fragment();
                    if( !fragment.isValid() ) continue;
                    if( fragment.position() > cursor.position() || fragment.position() + fragment.length() <= cursor.position() )
                    { continue; }

                    cursor.setPosition( fragment.position() );
                    cursor.setPosition( fragment.position() + fragment.length(), QTextCursor::KeepAnchor );
                    rect = cursorRect( cursor );
                    break;
                }


                QToolTip::showText( globalPosition, anchor, viewport(), rect );
            }

        }
        break;

        default: break;
    }

    return BaseEditor::event( event );

}

//________________________________________________
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void TextEditor::enterEvent( QEvent* event )
#else
void TextEditor::enterEvent( QEnterEvent* event )
#endif
{
    _updateClipboardActions( QClipboard::Clipboard );
    _updateClipboardActions( QClipboard::Selection );
    BaseEditor::enterEvent( event );
}

//________________________________________________
void TextEditor::mousePressEvent( QMouseEvent* event )
{

    Debug::Throw( QStringLiteral("TextEditor::mousePressEvent.\n") );

    // check button
    if( event->button() == Qt::LeftButton )
    {

        // increment counter
        clickCounter_.increment( cursorForPosition( event->pos() ).position() );
        switch( clickCounter_.counts() )
        {

            case 1:
            {
                // if single click in existing box selection, store drag position
                if(
                    event->modifiers() == Qt::NoModifier &&
                    boxSelection_.state() == BoxSelection::State::Finished &&
                    boxSelection_.rect().contains( fromViewport( event->pos() ) )
                    )
                {
                    // store position for drag
                    dragStart_ = event->pos();
                    BaseEditor::mousePressEvent( event );
                    return;
                }

                // if single click outside of existing box selection, clear the selection
                if( event->button() == Qt::LeftButton && boxSelection_.state() == BoxSelection::State::Finished )
                {

                    boxSelection_.clear();
                    _synchronizeBoxSelection();
                    emit copyAvailable( false );

                }

                // if single click and Control key pressed, start a new box selection
                if( event->modifiers() == Qt::ControlModifier  )
                {

                    // try re-enable box selection in case font has changed
                    if( boxSelection_.checkEnabled() )
                    {

                        boxSelection_.start( event->pos() );

                        // synchronize with other editors
                        _synchronizeBoxSelection();
                        return;
                    }

                }

                BaseEditor::mousePressEvent( event );
                return;

            }

            case 2:
            selectWord();
            break;

            case 3:
            selectLine();
            break;

            case 4:
            selectAll();
            _synchronizeSelection();
            _updateClipboard();
            break;

            default:
            event->ignore();
            break;
        }

        return;

    } else {

        // call base class implementation
        BaseEditor::mousePressEvent( event );

    }

    // for mid button, locate cursor at new position
    if(  event->button() == Qt::MiddleButton )
    { setTextCursor( cursorForPosition( event->pos() ) ); }

}

//________________________________________________
void TextEditor::mouseDoubleClickEvent( QMouseEvent* event )
{

    Debug::Throw( QStringLiteral("TextEditor::mouseDoubleClickEvent.\n") );

    if( event->button() != Qt::LeftButton ) BaseEditor::mouseDoubleClickEvent( event );
    else {

        /*
        mousePressEvent is not sent in case of double-click.
        we have to send one manually, in order to increment the selection mode
        */

        mousePressEvent( event );
    }

    return;

}

//________________________________________________
void TextEditor::mouseMoveEvent( QMouseEvent* event )
{

    Debug::Throw( 2, QStringLiteral("TextEditor::mouseMoveEvent.\n") );

    // do nothing if some buttons are pressed
    if( trackAnchors_ )
    {
        QString anchor;
        if( !( event->buttons() || ( anchor = anchorAt( event->pos() ) ).isEmpty() ) )
        {
            viewport()->setCursor( Qt::PointingHandCursor );

        } else {

            viewport()->setCursor( Qt::IBeamCursor );
            emit linkHovered(anchor);

        }

    }

    // see if there is a box selection in progress
    if( event->buttons() == Qt::LeftButton && boxSelection_.isEnabled() && boxSelection_.state() == BoxSelection::State::Started )
    {

        boxSelection_.update( event->pos() );
        _synchronizeBoxSelection();
        emit copyAvailable( true );

        if( autoScrollTimer_.isActive())
        {
            if( viewport()->rect().contains( event->pos() ) ) autoScrollTimer_.stop();
        } else if (!viewport()->rect().contains( event->pos() )) autoScrollTimer_.start(100, this);

        return;

    }

    // start a new box selection if requested
    if( event->buttons() == Qt::LeftButton && boxSelection_.isEnabled() && event->modifiers() == Qt::ControlModifier && viewport()->rect().contains( event->pos() ) )
    {

        boxSelection_.start( event->pos() );
        _synchronizeBoxSelection();
        emit copyAvailable( true );
        return;
    }

    // see if dragging existing box selection
    if( event->buttons() == Qt::LeftButton && boxSelection_.state() == BoxSelection::State::Finished && (event->pos() - dragStart_ ).manhattanLength() > QApplication::startDragDistance() )
    {
        // start drag
        auto drag( new QDrag(this) );

        // store data
        auto text( boxSelection_.toString() );
        auto data = new QMimeData;
        data->setText( text );
        data->setData( BoxSelection::mimeType, qPrintable( text ) );
        drag->setMimeData( data );
        drag->exec();

        return;
    }

    BaseEditor::mouseMoveEvent( event );

}

//________________________________________________
void TextEditor::mouseReleaseEvent( QMouseEvent* event )
{

    Debug::Throw( QStringLiteral("TextEditor::mouseReleaseEvent.\n") );

    autoScrollTimer_.stop();

    // no need to check for enability because there is no way for the box to start if disabled
    if( event->button() == Qt::LeftButton && boxSelection_.state() == BoxSelection::State::Started )
    {
        boxSelection_.finish( event->pos() );
        _synchronizeBoxSelection();
        BaseEditor::mouseReleaseEvent( event );
        return;
    }

    if( event->button() == Qt::LeftButton && boxSelection_.state() == BoxSelection::State::Finished )
    {
        boxSelection_.clear();
        _synchronizeBoxSelection();
        emit copyAvailable( false );
        BaseEditor::mouseReleaseEvent( event );
        return;
    }

    if( event->button() == Qt::LeftButton && clickCounter_.counts() > 1 )
    {
        // when multiple-click is in progress
        // do nothing because it can reset the selection
        event->ignore();
        return;
    }

    QString anchor;
    if( trackAnchors_ && event->button() == Qt::LeftButton &&
        !event->modifiers() &&
        !textCursor().hasSelection() &&
        !( anchor = anchorAt( event->pos() ) ).isEmpty() )
    {
        emit linkActivated( anchor );
        return;
    }

    if( event->button() == Qt::MiddleButton  && boxSelection_.state() == BoxSelection::State::Finished )
    { boxSelection_.clear(); }

    // process event
    BaseEditor::mouseReleaseEvent( event );
}

//________________________________________________
void TextEditor::wheelEvent( QWheelEvent* event )
{

    if( event->modifiers() & Qt::ControlModifier )
    {
        event->accept();

        // calculate delta
        const auto offset = double(event->angleDelta().y())/120;

        // check if direction has changed
        if( wheelOffsetAccumulated_ != 0 && Base::sign(offset) != Base::sign( wheelOffsetAccumulated_ ) )
        { wheelOffsetAccumulated_ = 0; }

        const auto offsetInt = int( wheelOffsetAccumulated_ + offset );
        if( offsetInt != 0 ) _incrementFontSize( offsetInt );
        wheelOffsetAccumulated_ += offset - offsetInt;
    } else {
        wheelOffsetAccumulated_ = 0;
        BaseEditor::wheelEvent( event );
    }

}

//________________________________________________
void TextEditor::dropEvent( QDropEvent* event )
{

    Debug::Throw( QStringLiteral("TextEditor::dropEvent.\n") );

    // static empty mimeData used to pass to base class
    // so that drop events are finished properly even when actually doing nothing
    static auto emptyData( new QMimeData );
    QDropEvent dropEvent( event->pos(), event->possibleActions(), emptyData, Qt::NoButton, Qt::NoModifier );

    // if mimeData is block selection, block selection is enabled here
    // and there is no active selection (standard or box), insert new box selection
    // at cursor position
    if(
        event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
        boxSelection_.isEnabled() &&
        boxSelection_.state() == BoxSelection::State::Empty &&
        !textCursor().hasSelection() )
    {
        Debug::Throw( QStringLiteral("TextEditor::dropEvent - dropping box selection.\n") );

        // retrieve text from mimeType
        auto text( event->mimeData()->text() );

        // create an empty boxSelection from current position with proper size
        boxSelection_.start( event->pos() );
        boxSelection_.finish( event->pos() );
        boxSelection_.fromString( text );
        boxSelection_.clear();

        event->acceptProposedAction();
        BaseEditor::dropEvent( &dropEvent );
        return;
    }

    if(
        event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
        boxSelection_.isEnabled() &&
        boxSelection_.state() == BoxSelection::State::Finished &&
        !toViewport( boxSelection_.rect() ).contains( event->pos() ) &&
        event->source() == this
        )
    {
        // drag is box selection and from this window. Move current block selection around.
        Debug::Throw( QStringLiteral("TextEditor::dropEvent - [box] moving current box selection.\n") );

        // count rows in current selection
        int rowCount( boxSelection_.cursorList().size() - 1 );

        // store cursor at new insertion position
        auto newCursor( cursorForPosition( event->pos() ) );

        // remove current selection
        boxSelection_.removeSelectedText();
        boxSelection_.clear();

        // prepare new selection
        QRect rect( cursorRect( newCursor ) );
        QPoint start( rect.center().x(), rect.top() );
        QPoint end( rect.center().x(), rect.top() + rowCount*fontMetrics().height() );

        boxSelection_.start( start );
        boxSelection_.finish( end );

        // join modifications with previous so that they appear as one entry in undo/redo list
        newCursor.joinPreviousEditBlock();

        // insert text in new box
        boxSelection_.fromString( event->mimeData()->text() );
        boxSelection_.clear();
        newCursor.endEditBlock();

        event->acceptProposedAction();
        BaseEditor::dropEvent( &dropEvent );
        return;
    }

    // check if there is one valid box selection that contains the drop point
    if(
        event->mimeData()->hasText() &&
        boxSelection_.isEnabled() &&
        boxSelection_.state() == BoxSelection::State::Finished &&
        toViewport( boxSelection_.rect() ).contains( event->pos() ) )
    {
        if( event->source() == this )
        {
            // current selection is inserted in itself. Doing nothing
            Debug::Throw( QStringLiteral("TextEditor::dropEvent - [box] doing nothing.\n") );
            event->acceptProposedAction();
            BaseEditor::dropEvent( &dropEvent );
            return;
        } else {
            // insert mine data in current box selection
            Debug::Throw( QStringLiteral("TextEditor::dropEvent - [box] inserting selection.\n") );
            boxSelection_.fromString( event->mimeData()->text() );
            setTextCursor( boxSelection_.cursorList().back() );
            boxSelection_.clear();
            event->acceptProposedAction();
            BaseEditor::dropEvent( &dropEvent );
            return;
        }
    }

    // retrieve selection bounding rect
    if( event->mimeData()->hasText() && textCursor().hasSelection() )
    {
        auto cursor( textCursor() );
        auto newCursor( cursorForPosition( event->pos() ) );
        bool contained(
            newCursor.position() >= qMin( cursor.position(), cursor.anchor() ) &&
            newCursor.position() <= qMax( cursor.position(), cursor.anchor() ) );
        if( contained && event->source() != this )
        {
            // drag action is from another widget and ends in selection. Replace this selection
            Debug::Throw( QStringLiteral("TextEditor::dropEvent - inserting selection.\n") );
            cursor.insertText( event->mimeData()->text() );
            event->acceptProposedAction();
            BaseEditor::dropEvent( &dropEvent );
            return;
        }

        if( event->source() == this )
        {
            // drag action is from this widget
            // insert selection at current location and remove old selection
            Debug::Throw( QStringLiteral("TextEditor::dropEvent - moving selection.\n") );
            cursor.beginEditBlock();
            cursor.removeSelectedText();
            cursor.setPosition( newCursor.position() );
            cursor.insertText( event->mimeData()->text() );
            cursor.endEditBlock();
            setTextCursor( cursor );

            event->acceptProposedAction();
            BaseEditor::dropEvent( &dropEvent );
            return;
        }

    }

    // for all other cases, use default
    BaseEditor::dropEvent( event );
}

//________________________________________________
void TextEditor::keyPressEvent( QKeyEvent* event )
{

    // clear line buffer.
    removeLineBuffer_.clear();

    /*
    need to grap Qt::CTRL+X, C and V event to forward them to the
    daughter implementation of cut, copy and paste, otherwise
    they are passed to the base class, with no way to override
    */
    if( event->modifiers() == Qt::ControlModifier )
    {
        if( event->matches( QKeySequence::Cut ) )
        {
            cut();
            event->ignore();
            return;
        }

        if( event->matches( QKeySequence::Copy ) )
        {
            copy();
            event->ignore();
            return;
        }

        if( event->matches( QKeySequence::Paste ) )
        {
            paste();
            event->ignore();
            return;
        }

    }

    // special key processing for box selection
    if( boxSelection_.state() == BoxSelection::State::Finished )
    {
        if(
            (event->key() >= Qt::Key_Shift &&  event->key() <= Qt::Key_ScrollLock) ||
            (event->key() >= Qt::Key_F1 &&  event->key() <= Qt::Key_F25) ||
            (event->key() >= Qt::Key_Super_L && event->key() <= Qt::Key_Direction_R ) ||
            (event->modifiers() != Qt::NoModifier && event->modifiers() != Qt::ShiftModifier ) )
        {
            BaseEditor::keyPressEvent( event );
            return;
        }

        // if cursor move clear selection
        if( event->key() >= Qt::Key_Home && event->key() <= Qt::Key_Down )
        {
            boxSelection_.clear();
            BaseEditor::keyPressEvent( event );
            return;
        }

        // if delete or backspace remove selection
        if( event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete )
        {
            boxSelection_.removeSelectedText();
            boxSelection_.clear();
            return;
        }

        // any other key should replace the selection
        if( event->key() == Qt::Key_Tab )
        {
            if( !_hasTabEmulation() ) boxSelection_.fromString( tabCharacter() );
            else {
                // retrieve position from begin of block
                int position( boxSelection_.cursorList().front().anchor() );
                position -= document()->findBlock( position ).position();
                int n( position % emulatedTabCharacter().size() );
                boxSelection_.fromString( emulatedTabCharacter().right( emulatedTabCharacter().size()-n ) );
            }

            boxSelection_.clear();
        } else if( !(event->text().isNull() || event->text().isEmpty() ) ) {
            boxSelection_.fromString( event->text() );
            boxSelection_.clear();
        }

        return;
    }

    // tab emulation
    if( event->key() == Qt::Key_Tab )
    {
        _insertTab();
        return;
    }

    // insertion mode
    if( event->key() == Qt::Key_Insert )
    {
        _toggleOverwriteMode();
        event->ignore();
        return;
    }

    // default event handling
    BaseEditor::keyPressEvent( event );

    // check NumLock and CapsLock
    /** right now this works only on X11 */
    bool changed( false );
    if( event->key() == Qt::Key_CapsLock ) changed = _setModifier( Modifier::CapsLock, !modifier( Modifier::CapsLock ) );
    else if( event->key() == Qt::Key_NumLock ) changed = _setModifier( Modifier::NumLock, !modifier( Modifier::NumLock ) );
    if( changed ) { emit modifiersChanged( modifiers() ); }

    return;
}

//_______________________________________________________
void TextEditor::focusInEvent( QFocusEvent* event )
{
    Debug::Throw() << "TextEditor::focusInEvent - " << key() << Qt::endl;
    if(
        _setModifier( Modifier::CapsLock, KeyModifier( Qt::Key_CapsLock ).state() == KeyModifier::State::On ) ||
        _setModifier( Modifier::NumLock, KeyModifier( Qt::Key_NumLock ).state() == KeyModifier::State::On ) )
    { emit modifiersChanged( modifiers() );}
    emit hasFocus( this );
    BaseEditor::focusInEvent( event );
}

//________________________________________________
void TextEditor::contextMenuEvent( QContextMenuEvent* event )
{
    Debug::Throw( QStringLiteral("TextEditor::contextMenuEvent.\n") );
    contextMenuPosition_ = event->pos();

    BaseContextMenu menu( this );
    menu.setHideDisabledActions( true );
    installContextMenuActions( &menu );
    if( !menu.isEmpty() )
    { menu.exec( event->globalPos() ); }
}

//______________________________________________________________
void TextEditor::resizeEvent( QResizeEvent* event )
{
    BaseEditor::resizeEvent( event );

    // update margin widget geometry
    QRect rect( contentsRect() );
    marginWidget_->setGeometry( QRect( rect.topLeft(), QSize( marginWidget_->width(), rect.height() ) ) );

    if( lineWrapMode() == BaseEditor::NoWrap ) return;
    if( event->oldSize().width() == event->size().width() ) return;
    if( !lineNumberDisplay_ ) return;

    // tell line number display to update at next draw
    lineNumberDisplay_->needUpdate();

}

//______________________________________________________________
void TextEditor::paintEvent( QPaintEvent* event )
{

    // create painter and translate from widget to viewport coordinates
    QPainter painter( viewport() );
    painter.setClipRect( event->rect() );

    painter.translate( -scrollbarPosition() );
    painter.setPen( Qt::NoPen );

    // loop over blocks that match the event rect
    TextBlockRange range(
        cursorForPosition( event->rect().topLeft() ).block(),
        cursorForPosition( event->rect().bottomRight() ).block().next() );

    for( const auto& block:range )
    {

        // check block
        if( !block.isValid() ) break;

        // retrieve block data and check background
        // static cast is use because should be faster and safe enough here
        auto data( static_cast<TextBlockData*>( block.userData() ) );
        if( !(data && data->hasFlag( TextBlock::HasBackground|TextBlock::CurrentBlock ) ) ) continue;

        // retrieve block rect
        auto blockRect( document()->documentLayout()->blockBoundingRect( block ) );
        blockRect.setLeft(0);
        blockRect.setWidth( viewport()->width() + scrollbarPosition().x() );

        QColor color;
        if( data->hasFlag( TextBlock::CurrentBlock ) && blockHighlightAction_->isEnabled() && blockHighlightAction_->isChecked() )
        {
            color = palette().color( QPalette::AlternateBase );

            // update current block rect
            // and redraw margin if changed
            if( _setCurrentBlockRect( QRect( QPoint(0, int(blockRect.topLeft().y()) ), QSize( marginWidget_->width(), int(blockRect.height()) ) ) ) )
            { marginWidget_->setDirty(); }

        }

        if( data->hasFlag( TextBlock::HasBackground ) )
        { color = Base::Color( color ).merge( data->background() ); }

        if( color.isValid() )
        {
            painter.setBrush( color );
            painter.drawRect( blockRect );
        }

    }

    if( boxSelection_.state() == BoxSelection::State::Started || boxSelection_.state() == BoxSelection::State::Finished )
    {
        painter.setPen( boxSelection_.color() );
        painter.setBrush( boxSelection_.brush() );
        painter.drawRect( boxSelection_.rect() );
    }

    painter.end();

    // base class painting
    BaseEditor::paintEvent( event );
    return;
}

//______________________________________________________________
void TextEditor::timerEvent(QTimerEvent *event)
{

    if (event->timerId() == autoScrollTimer_.timerId() )
    {

        auto globalPosition = QCursor::pos();
        auto position = viewport()->mapFromGlobal(globalPosition);
        QMouseEvent mouseEvent(QEvent::MouseMove, position, globalPosition, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        mouseMoveEvent(&mouseEvent);

    } else BaseEditor::timerEvent( event );

}

//______________________________________________________________
void TextEditor::changeEvent(QEvent *event)
{

    // base class
    BaseEditor::changeEvent( event );

    // update margin
    if( event->type() == QEvent::FontChange )
    {

        // update minimum font size
        minimumFontSize_ = qMin( 4, font().pointSize() );

        // update line number display
        if( lineNumberDisplay_ )
        {
            // update margins
            lineNumberDisplay_->updateWidth( document()->blockCount() );
            lineNumberDisplay_->needUpdate();
            _updateMargin();
            marginWidget_->setDirty();
        }

    }
}

//______________________________________________________________
void TextEditor::scrollContentsBy( int dx, int dy )
{
    // mark margins dirty if vertical scroll is non empty
    if( dy != 0 ) marginWidget_->setDirty();

    // base class call
    BaseEditor::scrollContentsBy( dx, dy );
}

//______________________________________________________________
void TextEditor::_installActions()
{
    Debug::Throw( QStringLiteral("TextEditor::_installActions.\n") );

    // create actions
    addAction( undoAction_ = new StandardAction( StandardAction::Type::Undo, this ) );
    undoAction_->setEnabled( document()->isUndoAvailable() && !isReadOnly() );
    connect( undoAction_, &QAction::triggered, document(), QOverload<>::of( &QTextDocument::undo ) );
    connect( this, &QTextEdit::undoAvailable, this, &TextEditor::_updateUndoRedoActions );

    addAction( redoAction_ = new StandardAction( StandardAction::Type::Redo, this ) );
    redoAction_->setEnabled( document()->isRedoAvailable() && !isReadOnly() );
    connect( redoAction_, &QAction::triggered, document(), QOverload<>::of( &QTextDocument::redo ) );
    connect( this, &QTextEdit::redoAvailable, this, &TextEditor::_updateUndoRedoActions );

    addAction( cutAction_ = new StandardAction( StandardAction::Type::Cut, this ) );
    cutAction_->setShortcut( QKeySequence::Cut );
    connect( cutAction_, &QAction::triggered, this, &TextEditor::cut );

    addAction( copyAction_ = new StandardAction( StandardAction::Type::Copy, this ) );
    connect( copyAction_, &QAction::triggered, this, &TextEditor::copy );

    addAction( pasteAction_ = new StandardAction( StandardAction::Type::Paste, this ) );
    connect( pasteAction_, &QAction::triggered, this, &TextEditor::paste );

    addAction( pasteUnformatedAction_ = new QAction( IconEngine::get( IconNames::Paste ), tr( "Paste Without Formatting" ), this ) );
    connect( pasteUnformatedAction_, &QAction::triggered, this, &TextEditor::pasteUnformated );
    pasteUnformatedAction_->setShortcut( Qt::SHIFT|Qt::CTRL|Qt::Key_V );
    pasteUnformatedAction_->setShortcutContext( Qt::WidgetShortcut );
    _updatePasteAction();

    addAction( clearAction_ = new QAction( IconEngine::get( IconNames::EditClear ), tr( "Clear" ), this ) );
    connect( clearAction_, &QAction::triggered, this, &TextEditor::clear );

    addAction( selectAllAction_ = new QAction( tr( "Select All" ), this ) );
    selectAllAction_->setShortcut( QKeySequence::SelectAll );
    selectAllAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( selectAllAction_, &QAction::triggered, this, &QTextEdit::selectAll );

    addAction( upperCaseAction_ = new QAction( tr( "Upper Case" ), this ) );
    upperCaseAction_->setShortcut( Qt::CTRL|Qt::Key_U );
    upperCaseAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( upperCaseAction_, &QAction::triggered, this, &TextEditor::upperCase );

    addAction( lowerCaseAction_ = new QAction( tr( "Lower Case" ), this ) );
    lowerCaseAction_->setShortcut( Qt::SHIFT|Qt::CTRL|Qt::Key_U );
    lowerCaseAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( lowerCaseAction_, &QAction::triggered, this, &TextEditor::lowerCase );

    addAction( findAction_ = new QAction( IconEngine::get( IconNames::Find ), tr( "Find..." ), this ) );
    findAction_->setShortcut( QKeySequence::Find );
    findAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAction_, &QAction::triggered, this, &TextEditor::_findFromDialog );

    addAction( findAgainAction_ = new QAction( tr( "Find Again" ), this ) );
    findAgainAction_->setShortcut( Qt::CTRL|Qt::Key_G );
    findAgainAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainAction_, &QAction::triggered, this, &TextEditor::findAgainForward );

    addAction( findAgainBackwardAction_ = new QAction( this ) );
    findAgainBackwardAction_->setShortcut( Qt::SHIFT|Qt::CTRL|Qt::Key_G );
    findAgainBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainBackwardAction_, &QAction::triggered, this, &TextEditor::findAgainBackward );

    addAction( findSelectionAction_ = new QAction( tr( "Find Selection" ), this ) );
    findSelectionAction_->setShortcut( Qt::CTRL|Qt::Key_H );
    findSelectionAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionAction_, &QAction::triggered, this, &TextEditor::findSelectionForward );

    addAction( findSelectionBackwardAction_ = new QAction( this ) );
    findSelectionBackwardAction_->setShortcut( Qt::SHIFT|Qt::CTRL|Qt::Key_H );
    findSelectionBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionBackwardAction_, &QAction::triggered, this, &TextEditor::findSelectionBackward );

    addAction( replaceAction_ = new QAction( IconEngine::get( IconNames::Find ), tr( "Replace..." ), this ) );
    replaceAction_->setShortcut( QKeySequence::Replace );
    replaceAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( replaceAction_, &QAction::triggered, this, &TextEditor::_replaceFromDialog );

    addAction( replaceAgainAction_ = new QAction( tr( "Replace Again" ), this ) );
    replaceAgainAction_->setShortcut( Qt::CTRL|Qt::Key_T );
    replaceAgainAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( replaceAgainAction_, &QAction::triggered, this, &TextEditor::replaceAgainForward );

    addAction( replaceAgainBackwardAction_ = new QAction( this ) );
    replaceAgainBackwardAction_->setShortcut( Qt::SHIFT|Qt::CTRL|Qt::Key_T );
    replaceAgainBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( replaceAgainBackwardAction_, &QAction::triggered, this, &TextEditor::replaceAgainBackward );

    addAction( gotoLineAction_ = new QAction( tr( "Goto Line Number..." ), this ) );
    gotoLineAction_->setShortcut( Qt::CTRL|Qt::Key_L );
    gotoLineAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( gotoLineAction_, &QAction::triggered, this, &TextEditor::_selectLineFromDialog );

    // remove line action
    QAction* remove_line_action( new QAction( tr( "Remove Current Line" ), this ) );
    addAction( remove_line_action );
    remove_line_action->setShortcut( Qt::CTRL|Qt::Key_K );
    remove_line_action->setShortcutContext( Qt::WidgetShortcut );
    connect( remove_line_action, &QAction::triggered, this, &TextEditor::removeLine );

    // current block highlight
    addAction( blockHighlightAction_ = new QAction( tr( "Highlight Current Paragraph" ), this ) );
    blockHighlightAction_->setCheckable( true );
    blockHighlightAction_->setChecked( blockHighlight_->isEnabled() );
    blockHighlightAction_->setShortcut( Qt::Key_F12 );
    blockHighlightAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( blockHighlightAction_, &QAction::toggled, this, &TextEditor::_toggleBlockHighlight );

    // wrap mode
    addAction( wrapModeAction_ = new QAction( tr( "Wrap Text" ), this ) );
    wrapModeAction_->setCheckable( true );
    wrapModeAction_->setChecked( lineWrapMode() == BaseEditor::WidgetWidth );
    _setModifier( Modifier::Wrap, lineWrapMode() == BaseEditor::WidgetWidth );
    wrapModeAction_->setShortcut( Qt::Key_F10 );
    wrapModeAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( wrapModeAction_, &QAction::toggled, this, &TextEditor::_toggleWrapMode );

    // tab emulation action
    addAction( tabEmulationAction_ = new QAction( tr( "Emulate Tabs" ), this ) );
    tabEmulationAction_->setCheckable( true );
    tabEmulationAction_->setChecked( hasTabEmulation_ );
    connect( tabEmulationAction_, &QAction::toggled, this, &TextEditor::_toggleTabEmulation );

    // line number action
    addAction( showLineNumberAction_ =new QAction( tr( "Show Line Numbers" ), this ) );
    showLineNumberAction_->setToolTip( tr( "Show/hide line numbers" ) );
    showLineNumberAction_->setCheckable( true );
    showLineNumberAction_->setShortcut( Qt::Key_F11 );
    showLineNumberAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( showLineNumberAction_, &QAction::toggled, this, &TextEditor::_toggleShowLineNumbers );

    addAction( copyLinkAction_ = new QAction( IconEngine::get( IconNames::Copy ), tr( "Copy Link" ), this ) );
    connect( copyLinkAction_, &QAction::triggered, this, &TextEditor::_copyLinkLocation );

    addAction( incrementFontSizeAction_ = new QAction( tr( "Increase Font Size" ), this ) );
    incrementFontSizeAction_->setShortcut( QKeySequence::ZoomIn );
    incrementFontSizeAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( incrementFontSizeAction_, &QAction::triggered, this, &TextEditor::_incrementFontSize );

    addAction( decrementFontSizeAction_ = new QAction( tr( "Decrease Font Size" ), this ) );
    decrementFontSizeAction_->setShortcut( QKeySequence::ZoomOut );
    decrementFontSizeAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( decrementFontSizeAction_, &QAction::triggered, this, &TextEditor::_decrementFontSize );

    addAction( restoreDefaultFontAction_ = new QAction( tr( "Restore Default Font Size" ), this ) );
    restoreDefaultFontAction_->setShortcut( Qt::CTRL|Qt::Key_0 );
    restoreDefaultFontAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( restoreDefaultFontAction_, &QAction::triggered, this, &TextEditor::_restoreDefaultFont );

    // update actions that depend on the presence of a selection
    _updateSelectionActions( textCursor().hasSelection() );

}

//______________________________________________________________________
void TextEditor::_createFindDialog()
{

    Debug::Throw( QStringLiteral("TextEditor::_createFindDialog.\n") );
    if( !findDialog_ )
    {
        if( !findWidget_ ) createFindWidget( false );
        findDialog_ = new BaseFindDialog( this );
        findDialog_->setWindowTitle( tr( "Find in Text" ) );
        findDialog_->setBaseFindWidget( findWidget_ );
    }

    return;

}

//______________________________________________________________________
void TextEditor::_createReplaceDialog()
{

    Debug::Throw( QStringLiteral("TextEditor::_createReplaceDialog.\n") );
    if( !replaceDialog_ )
    {
        if( !replaceWidget_ ) createReplaceWidget( false );
        replaceDialog_ = new BaseReplaceDialog( this );
        replaceDialog_->setWindowTitle( tr( "Replace in Text" ) );
        replaceDialog_->setBaseFindWidget( replaceWidget_ );
    }

    return;

}

//________________________________________________
void TextEditor::_createSelectLineDialog()
{
    if( !selectLineDialog_ )
    {
        Debug::Throw( QStringLiteral("TextEditor::_createSelectLineDialog.\n") );
        selectLineDialog_ = new SelectLineDialog( this );
        if( !selectLineWidget_ ) createSelectLineWidget( false );
        selectLineDialog_->setSelectLineWidget( selectLineWidget_ );
    }

    return;
}

//__________________________________________________
void TextEditor::_createProgressDialog()
{

    Debug::Throw( QStringLiteral("TextEditor::_createProgressDialog.\n") );

    // create dialog
    auto dialog = new QProgressDialog(0);
    dialog->setAttribute( Qt::WA_DeleteOnClose, true );
    dialog->setLabelText( tr( "Replace text in selection" ) );
    dialog->setWindowTitle( tr( "Replace in Text" ) );

    // connections
    connect( this, &TextEditor::busy, dialog, &QProgressDialog::setMaximum );
    connect( this, &TextEditor::progressAvailable, dialog, &QProgressDialog::setValue );
    connect( this, &TextEditor::idle, dialog, &QWidget::close );

    QtUtil::centerOnWidget( dialog, this );
    dialog->show();

}

//______________________________________________________________________
bool TextEditor::_findForward( const TextSelection& selection, bool rewind )
{
    Debug::Throw( QStringLiteral("TextEditor::_findForward.\n") );
    if( selection.text().isEmpty() ) return false;

    // store selection
    setLastSelection( selection );

    // retrieve current cursor
    auto cursor( textCursor() );

    // if no_increment, start from the beginning of the possible current selection
    if( cursor.hasSelection() && selection.hasFlag( TextSelection::NoIncrement ) )
    { cursor.setPosition( cursor.anchor() ); }

    if( selection.hasFlag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        QRegularExpression regexp( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.hasFlag( TextSelection::CaseSensitive ) );

        // make a copy of current cursor
        auto found( cursor );

        // if current text has selection that match, make sure pointer is located at the end of it
        if( found.hasSelection() && Base::exactMatch( regexp, found.selectedText() ) )
        { found.setPosition( qMax( found.position(), found.anchor() ) ); }

        // move the found to the end of the document
        // and retrieve selected text
        found.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
        QString text( found.selectedText() );

        // parse text
        auto match = regexp.match( text );
        if( !match.hasMatch() )
        {
            // no match found
            // if not rewind, stop here
            if( !rewind ) return false;

            // update selection to the beginning of the document
            found.movePosition( QTextCursor::Start, QTextCursor::KeepAnchor );
            text = found.selectedText();
            match = regexp.match( text );

            if( !match.hasMatch() ) return false;
        }

        // match found. Update selection and return
        const auto length = match.capturedLength();
        int position( match.capturedStart() + qMin( found.anchor(), found.position() ) );
        found.setPosition( position, QTextCursor::MoveAnchor );
        found.setPosition( position+length, QTextCursor::KeepAnchor );
        setTextCursor( found );

        // copy selected text to clipboard
        if( qApp->clipboard()->supportsSelection() )
        { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }

        return true;

    } else {

        // search flags
        QTextDocument::FindFlags flags( 0 );
        if( selection.hasFlag( TextSelection::CaseSensitive ) )  flags |= QTextDocument::FindCaseSensitively;
        if( selection.hasFlag( TextSelection::EntireWord ) ) flags |= QTextDocument::FindWholeWords;

        auto found( document()->find( selection.text(), cursor, flags ) );

        // find failed.
        if( found.isNull() && rewind )
        {
            cursor.movePosition( QTextCursor::Start );
            found = document()->find( selection.text(), cursor, flags );
        }

        if( found.isNull() ) return false;
        else {

            setTextCursor( found );

            // copy selected text to clipboard
            if( qApp->clipboard()->supportsSelection() )
            { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }

            return true;
        }

    }

    // useless
    return false;

}

//______________________________________________________________________
bool TextEditor::_findBackward( const TextSelection& selection, bool rewind )
{

    Debug::Throw( QStringLiteral("TextEditor::_findBackward.\n") );
    if( selection.text().isEmpty() ) return false;
    setLastSelection( selection );

    // retrieve current cursor
    auto cursor( textCursor() );

    // if no_increment, start from the beginning of the possible current selection
    if( cursor.hasSelection() && selection.hasFlag( TextSelection::NoIncrement ) )
    { cursor.setPosition( cursor.anchor()+selection.text().size()+1 ); }

    if( selection.hasFlag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        QRegularExpression regexp( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.hasFlag( TextSelection::CaseSensitive ) );

        // make a copy of current cursor
        auto found( cursor );

        // if current text has selection that match, make sure pointer is located at the end of it
        if( found.hasSelection() && Base::exactMatch( regexp, found.selectedText() ) )
        { found.setPosition( qMin( found.position(), found.anchor() ) ); }

        // move cursor to beginning of the text
        found.movePosition( QTextCursor::Start, QTextCursor::KeepAnchor );
        QString text( found.selectedText() );

        // parse text
        QRegularExpressionMatch match;
        const auto index = text.lastIndexOf( regexp, -1, &match );
        if( index < 0 || !match.hasMatch() )
        {
            // no match found
            // if not rewind, stop here
            if( !rewind ) return false;

            // update selection to the beginning of the document
            found.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
            text = found.selectedText();
            match = regexp.match( text );

            if( !match.hasMatch() ) return false;
        }

        const auto length = match.capturedLength();

        // match found. Update selection and return
        int position( match.capturedStart() + qMin( found.anchor(), found.position() )+length );
        found.setPosition( position, QTextCursor::MoveAnchor );
        found.setPosition( position-length, QTextCursor::KeepAnchor );
        setTextCursor( found );

        // copy selected text to clipboard
        if( qApp->clipboard()->supportsSelection() )
        { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }

        return true;

    } else {

        // search flags
        QTextDocument::FindFlags flags( QTextDocument::FindBackward );
        if( selection.hasFlag( TextSelection::CaseSensitive ) )  flags |= QTextDocument::FindCaseSensitively;
        if( selection.hasFlag( TextSelection::EntireWord ) ) flags |= QTextDocument::FindWholeWords;

        auto found( document()->find( selection.text(), cursor, flags ) );

        // find failed.
        if( found.isNull() && rewind )
        {
            cursor.movePosition( QTextCursor::End );
            found = document()->find( selection.text(), cursor, flags );
        }

        if( found.isNull() ) return false;
        else {
            setTextCursor( found );
            return true;
        }
    }

    // useless
    return false;

}

//______________________________________________________________________
int TextEditor::_replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode )
{

    Debug::Throw()
        << "TextEditor::_replaceInRange -"
        << " anchor: " << cursor.anchor()
        << " position: " << cursor.position()
        << " selection: " << selection.text()
        << " replacement: " << selection.replaceText()
        << Qt::endl;

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return 0;

    // check selection
    if( selection.text().isEmpty() ) return 0;
    setLastSelection( selection );

    // check cursor
    if( !cursor.hasSelection() ) return 0;

    // store number of matches
    // and make local copy of cursor
    int found = 0;

    int savedAnchor( qMin( cursor.position(), cursor.anchor() ) );
    int savedPosition( qMax( cursor.position(), cursor.anchor() ) );
    int currentPosition( savedAnchor );

    // check if regexp should be used or not
    if( selection.hasFlag( TextSelection::RegExp ) )
    {

        Debug::Throw( QStringLiteral("TextEditor::_replaceInRange - regexp.\n") );

        // construct regexp and check
        QRegularExpression regexp( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        Base::setCaseSensitivity( regexp, selection.hasFlag( TextSelection::CaseSensitive ) );

        // replace everything in selected text
        QString selectedText( cursor.selectedText() );
        emit busy( selectedText.size() );

        QRegularExpressionMatch match;
        for( int position = 0; (position = selectedText.indexOf( regexp, position, &match )) != -1; )
        {
            // replace in selected text
            selectedText.replace( position, match.capturedLength(), selection.replaceText() );

            // replace in cursor
            /* this is to allow for undoing the changes one by one */
            cursor.setPosition( savedAnchor + position );
            cursor.setPosition( savedAnchor + position + match.capturedLength(), QTextCursor::KeepAnchor );
            cursor.insertText( selection.replaceText() );
            currentPosition = cursor.position();

            // increment position
            position += selection.replaceText().size();
            currentPosition = savedAnchor + position;

            found++;

            emit progressAvailable( position );

        }

        emit idle();

        // update cursor
        if( mode == CursorMode::Expand )
        {
            cursor.setPosition( savedAnchor );
            cursor.setPosition( savedAnchor + selectedText.length(), QTextCursor::KeepAnchor );

        } else if( mode == CursorMode::Move ) cursor.setPosition( currentPosition );

    } else {

        Debug::Throw( QStringLiteral("TextEditor::_replaceInRange - normal replacement.\n") );

        emit busy( cursor.selectedText().size() );

        // changes local cursor to beginning of the selection
        cursor.setPosition( savedAnchor );

        // define search flags
        QTextDocument::FindFlags flags(0);
        if( selection.hasFlag( TextSelection::CaseSensitive ) )  flags |= QTextDocument::FindCaseSensitively;
        if( selection.hasFlag( TextSelection::EntireWord ) ) flags |= QTextDocument::FindWholeWords;

        while( !( cursor = document()->find( selection.text(), cursor, flags ) ).isNull() && cursor.position() <= savedPosition )
        {

            // perform replacement
            cursor.insertText( selection.replaceText() );
            currentPosition = cursor.position();
            savedPosition += selection.replaceText().size() - selection.text().size();
            found ++;

            emit busy( savedPosition );
            emit progressAvailable( currentPosition );

        }

        emit idle();

        if( mode == CursorMode::Expand )
        {
            cursor.setPosition( savedAnchor );
            cursor.setPosition( savedPosition, QTextCursor::KeepAnchor );
        } else if( mode == CursorMode::Move ) cursor.setPosition( currentPosition );

    }

    return found;

}

//_____________________________________________________________
void TextEditor::_synchronizeBoxSelection() const
{

    if( !isSynchronized() ) return;

    // Debug::Throw( QStringLiteral("TextEditor::_synchronizeBoxSelection.\n") );
    Base::KeySet<TextEditor> displays( this );
    for( const auto& editor:Base::KeySet<TextEditor>(this) )
    { editor->boxSelection_.synchronize( boxSelection_ ); }

}

//_____________________________________________________________
bool TextEditor::_setLeftMargin( int margin )
{

    Debug::Throw() << "TextEditor::_setLeftMargin - margin: " << margin << Qt::endl;
    if( margin == leftMargin_ ) return false;

    leftMargin_ = margin;
    setViewportMargins( leftMargin_, 0, 0, 0 );
    marginWidget_->resize( leftMargin_, marginWidget_->height() );
    return true;

}

//_____________________________________________________________
void TextEditor::_toggleOverwriteMode()
{

    Debug::Throw( QStringLiteral("TextEditor::_toggleOverwriteMode.\n") );
    setOverwriteMode( !overwriteMode() );
    if( _setModifier( Modifier::Insert, overwriteMode() ) ) emit modifiersChanged( modifiers() );
    return;

}

//________________________________________________
bool TextEditor::_setTabSize( int tabSize )
{

    Debug::Throw() << "TextEditor::_setTabSize - " << tabSize << Qt::endl;
    Q_ASSERT( tabSize > 0 );

    int stopWidth( tabSize * fontMetrics().horizontalAdvance( ' ' ) );
    if( tabSize == emulatedTab_.size() && tabStopDistance() == stopWidth )
    { return false; }

    // create strings and regular expressions
    // define normal tabs
    normalTab_ = QStringLiteral("\t");
    normalTabRegexp_.setPattern( QStringLiteral("^(\\t)+") );
    setTabStopDistance( stopWidth );

    // define emulated tabs
    emulatedTab_ = QString( tabSize, ' ' );
    emulatedTabRegexp_.setPattern( QStringLiteral( "^(%1)+" ).arg( emulatedTab_ ) );

    // update tab string according to tab emulation state
    if( _hasTabEmulation() ) tab_ = emulatedTab_;
    return true;
}

//_____________________________________________________________
void TextEditor::_insertTab()
{
    Debug::Throw( QStringLiteral("TextEditor::_insertTab.\n") );

    // retrieve current cursor
    auto cursor( textCursor() );
    if( !_hasTabEmulation() ) cursor.insertText( normalTabCharacter() );
    else {

        // retrieve position from begin of block
        int position( qMin( cursor.position(), cursor.anchor() ) );
        position -= document()->findBlock( position ).position();
        int n( position % emulatedTabCharacter().size() );
        cursor.insertText( emulatedTabCharacter().right( emulatedTabCharacter().size()-n ) );

    }

    return;

}

//___________________________________________________________________________
bool TextEditor::_updateMargin()
{

    Debug::Throw( QStringLiteral("TextEditor::_updateMargin.\n") );
    int leftMargin( 0 );

    if( showLineNumberAction_->isChecked() && showLineNumberAction_->isVisible() )
    { leftMargin += lineNumberDisplay_->width(); }

    return _setLeftMargin( leftMargin );
    if( leftMargin_ == leftMargin ) return false;
    return true;

}

//________________________________________________
void TextEditor::_updateConfiguration()
{

    Debug::Throw( QStringLiteral("TextEditor::_updateConfiguration.\n") );

    // wrap mode
    if( wrapFromOptions() )
    { wrapModeAction_->setChecked( XmlOptions::get().get<bool>( QStringLiteral("WRAP_TEXT") ) ); }

    if( lineNumbersFromOptions() )
    { showLineNumberAction_->setChecked( XmlOptions::get().get<bool>( QStringLiteral("SHOW_LINE_NUMBERS") ) ); }

    // tab emulation
    _setTabSize( XmlOptions::get().get<int>(QStringLiteral("TAB_SIZE")) );
    tabEmulationAction_->setChecked( XmlOptions::get().get<bool>( QStringLiteral("TAB_EMULATION") ) );

    // paragraph highlighting
    if( highlightBlockFromOptions_ )
    {
        blockHighlight_->setEnabled( XmlOptions::get().get<bool>( QStringLiteral("HIGHLIGHT_PARAGRAPH") ) );
        blockHighlightAction_->setEnabled( true );
        blockHighlightAction_->setChecked( XmlOptions::get().get<bool>( QStringLiteral("HIGHLIGHT_PARAGRAPH") ) );
    }

    // update box configuration
    // clear
    boxSelection_.updateConfiguration();
    if( !boxSelection_.isEnabled() && boxSelection_.state() != BoxSelection::State::Empty )
    {
        boxSelection_.clear();
        _synchronizeBoxSelection();
        emit copyAvailable( false );
    }

    // cursor monitor
    cursorMonitor_.setEnabled( XmlOptions::get().get<int>( QStringLiteral("AUTOHIDE_CURSOR_DELAY") ) > 0 );
    cursorMonitor_.setAutoHideDelay( XmlOptions::get().get<int>( QStringLiteral("AUTOHIDE_CURSOR_DELAY") ) * 1000 );

    return;

}

//________________________________________________
void TextEditor::_incrementFontSize( int delta )
{

    // change font size
    auto font( this->font() );
    font.setPointSize( qMax( minimumFontSize_, font.pointSize() + delta ) );

    setFont( font );

    // also update associated displays
    for( auto&& editor:Base::KeySet<TextEditor>( this ) )
    { editor->setFont( font ); }

}

//________________________________________________
void TextEditor::_restoreDefaultFont()
{

    const auto font( qApp->font( this ) );
    setFont( font );

    // also update associated displays
    for( auto&& editor:Base::KeySet<TextEditor>( this ) )
    { editor->setFont( font ); }

}

//________________________________________________
void TextEditor::_synchronizeSelection()
{

    //Debug::Throw( QStringLiteral("TextEditor::_synchronizeSelection.\n") );
    if( !isSynchronized() ) return;

    for( const auto& editor:Base::KeySet<TextEditor>(this) )
    {

        // check if textCursor is different
        if( editor->textCursor().position() == textCursor().position() &&
            editor->textCursor().anchor() == textCursor().anchor() )
            continue;

        // store scrollbar positions
        auto scrollbars( editor->scrollbarPosition() );

        editor->setSynchronized( false );
        editor->setUpdatesEnabled( false );
        editor->setTextCursor( textCursor() );

        // restore scrollbar positions
        editor->horizontalScrollBar()->setValue( scrollbars.x() );
        editor->verticalScrollBar()->setValue( scrollbars.y() );

        editor->setUpdatesEnabled( true );
        editor->setSynchronized( true );
    }
}

//________________________________________________________
void TextEditor::_updateContentActions()
{

    Debug::Throw( QStringLiteral("TextEditor::_updateContentActions.\n") );

    // actions
    clearAction_->setEnabled( !(document()->isEmpty() || isReadOnly() ) );
    selectAllAction_->setEnabled( !document()->isEmpty() );

}

//________________________________________________
void TextEditor::_updateReadOnlyActions( bool readOnly )
{

    Debug::Throw( QStringLiteral("TextEditor::_updateReadOnlyActions.\n") );
    bool hasSelection( textCursor().hasSelection() );

    clearAction_->setEnabled( !(readOnly || document()->isEmpty() ) );

    cutAction_->setEnabled( hasSelection && !readOnly );
    upperCaseAction_->setEnabled( hasSelection && !readOnly );
    lowerCaseAction_->setEnabled( hasSelection && !readOnly );

    replaceAction_->setEnabled( !readOnly );
    replaceAgainAction_->setEnabled( !readOnly );
    replaceAgainBackwardAction_->setEnabled( !readOnly );

    _updateUndoRedoActions();
    _updatePasteAction();

}

//________________________________________________
void TextEditor::_updateUndoRedoActions()
{

    Debug::Throw( QStringLiteral("TextEditor::_updateUndoRedoActions.\n") );
    undoAction_->setEnabled( document()->isUndoAvailable() && !isReadOnly() );
    redoAction_->setEnabled( document()->isRedoAvailable() && !isReadOnly() );

}

//________________________________________________
void TextEditor::_updateSelectionActions( bool hasSelection )
{

    Debug::Throw() << "TextEditor::_updateSelectionActions - hasSelection: " << hasSelection << Qt::endl;

    bool editable( !isReadOnly() );
    cutAction_->setEnabled( hasSelection && editable );
    copyAction_->setEnabled( hasSelection );
    upperCaseAction_->setEnabled( hasSelection && editable );
    lowerCaseAction_->setEnabled( hasSelection && editable );

    _updateClipboardActions( QClipboard::Clipboard );
    _updateClipboardActions( QClipboard::Selection );

}

//________________________________________________
void TextEditor::_updateClipboardActions( QClipboard::Mode mode )
{
    Debug::Throw( QStringLiteral("TextEditor::_updateClipboardActions.\n") );
    if( mode == QClipboard::Selection )
    {

        const bool hasSelection( !qApp->clipboard()->text( QClipboard::Selection ).isEmpty() );
        findSelectionAction_->setEnabled( hasSelection );
        findSelectionBackwardAction_->setEnabled( hasSelection );

    }

}

//________________________________________________
void TextEditor::_updateClipboard()
{
    Debug::Throw( QStringLiteral("TextEditor::_updateClipboard.\n") );

    // copy selected text to clipboard
    if( qApp->clipboard()->supportsSelection() && textCursor().hasSelection() )
    { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }

}

//_____________________________________________
void TextEditor::_copyLinkLocation()
{
    Debug::Throw( QStringLiteral("TextEditor::_copyLinkLocation.\n") );
    if( !trackAnchors_ ) return;

    auto anchor( this->anchor() );
    if( anchor.isEmpty() ) return;

    // copy selected text to clipboard
    qApp->clipboard()->setText( anchor, QClipboard::Clipboard );
    if( qApp->clipboard()->supportsSelection() ) qApp->clipboard()->setText( anchor, QClipboard::Selection );
    if( qApp->clipboard()->supportsFindBuffer() ) qApp->clipboard()->setText( anchor, QClipboard::FindBuffer );
}

//________________________________________________
void TextEditor::_updatePasteAction()
{

    Debug::Throw( QStringLiteral("TextEditor::_updatePasteAction.\n") );
    pasteAction_->setEnabled( !isReadOnly() );
    pasteUnformatedAction_->setEnabled( !isReadOnly() );
}

//_________________________________________________
void TextEditor::_toggleBlockHighlight( bool state )
{

    Debug::Throw( QStringLiteral("TextEditor::_toggleBlockHighlight.\n") );

    // enable
    blockHighlight_->setEnabled( state );

    // update options
    XmlOptions::get().set<bool>( QStringLiteral("HIGHLIGHT_PARAGRAPH"), state );

    // update current paragraph
    if( blockHighlight_->isEnabled() ) blockHighlight_->highlight();
    else blockHighlight_->clear();

    // redraw
    update();

    // propagate to other displays
    if( isSynchronized() )
    {
        // temporarely disable synchronization
        // to avoid infinite loop
        setSynchronized( false );

        for( const auto& editor:Base::KeySet<TextEditor>( this ) )
        { if( editor->isSynchronized() ) editor->blockHighlightAction_->setChecked( state ); }
        setSynchronized( true );

    }

}

//________________________________________________
bool TextEditor::_toggleWrapMode( bool state )
{
    Debug::Throw() << "TextEditor::_toggleWrapMode - " << (state ? "True":"false") << Qt::endl;
    LineWrapMode mode( state ? BaseEditor::WidgetWidth : BaseEditor::NoWrap );
    if( mode == lineWrapMode() ) return false;

    setLineWrapMode( mode );
    if( _setModifier( Modifier::Wrap, state ) ) emit modifiersChanged( modifiers() );

    // propagate to associated display
    if( isSynchronized() )
    {

        // temporarely disable synchronization
        // to avoid infinite loop
        setSynchronized( false );
        for( const auto& editor:Base::KeySet<TextEditor>( this ) )
        { if( editor->isSynchronized() ) editor->wrapModeAction_->setChecked( state ); }
        setSynchronized( true );

    }

    return true;

}

//________________________________________________
bool TextEditor::_toggleTabEmulation( bool state )
{

    Debug::Throw() << "TextEditor::_toggleTabEmulation - " << (state ? "True":"false") << Qt::endl;

    // check if changed
    if( hasTabEmulation_ == state ) return false;

    // set flag
    hasTabEmulation_ = state;
    tab_ = hasTabEmulation_ ? emulatedTab_ : normalTab_;
    tabRegexp_ = hasTabEmulation_ ? emulatedTabRegexp_ : normalTabRegexp_;

    // propagate to associated display
    if( isSynchronized() )
    {

        // temporarely disable synchronization
        // to avoid infinite loop
        setSynchronized( false );
        for( const auto& editor:Base::KeySet<TextEditor>( this ) )
        { if( editor->isSynchronized() ) editor->tabEmulationAction_->setChecked( state ); }
        setSynchronized( true );

    }

    return true;

}

//_______________________________________________________
void TextEditor::_toggleShowLineNumbers( bool state )
{

    _updateMargin();

    // update options
    XmlOptions::get().set<bool>( QStringLiteral("SHOW_LINE_NUMBERS"), state );

    // propagate to other displays
    if( isSynchronized() )
    {
        // temporarely disable synchronization
        // to avoid infinite loop
        setSynchronized( false );

        for( const auto& editor:Base::KeySet<TextEditor>( this ) )
        { if( editor->isSynchronized() ) editor->showLineNumberAction_->setChecked( state ); }
        setSynchronized( true );

    }

    return;
}

//________________________________________________________
void TextEditor::_blockCountChanged( int count )
{

    Debug::Throw( QStringLiteral("TextEditor::_blockCountChanged.\n") );

    // margins
    if( !( lineNumberDisplay_ && lineNumberDisplay_->updateWidth( count ) ) ) return;
    if( !( showLineNumberAction_ && showLineNumberAction_->isChecked() && showLineNumberAction_->isVisible() ) ) return;
    _updateMargin();
    update();

}

//_____________________________________________________________________
void TextEditor::_findFromDialog()
{
    Debug::Throw( QStringLiteral("TextEditor::_findFromDialog.\n") );

    // set default text
    // update find text
    QString text( selection().text() );
    if( !text.isEmpty() )
    {
        const int maxLength( 1024 );
        text.truncate( maxLength );
    }

    // create
    if( useEmbeddedWidgets_ )
    {

        if( !findWidget_ ) createFindWidget( true );
        findWidget_->show();

    } else {

        if( !findDialog_ ) _createFindDialog();
        findDialog_->centerOnParent();
        findDialog_->show();
        findDialog_->activateWindow();

    }

    findWidget_->enableRegExp( true );
    findWidget_->synchronize();
    findWidget_->matchFound();
    findWidget_->setText( text );
    findWidget_->editor().setFocus();


    return;
}

//_____________________________________________________________________
void TextEditor::_replaceFromDialog()
{
    Debug::Throw( QStringLiteral("TextEditor::_replaceFromDialog.\n") );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    if( useEmbeddedWidgets_ )
    {

        if( !replaceWidget_ ) createReplaceWidget( true );
        replaceWidget_->show();

    } else {

        if( !replaceDialog_ ) _createReplaceDialog();
        replaceDialog_->centerOnParent();
        replaceDialog_->show();
        replaceDialog_->activateWindow();

    }

    // synchronize combo-boxes
    replaceWidget_->synchronize();
    replaceWidget_->matchFound();

    // update find text
    QString text;
    if( !( text = qApp->clipboard()->text( QClipboard::Selection) ).isEmpty() ) replaceWidget_->setText( text );
    else if( textCursor().hasSelection() ) replaceWidget_->setText( textCursor().selectedText() );
    else if( !( text = lastSelection().text() ).isEmpty() ) replaceWidget_->setText( text );
    replaceWidget_->editor().setFocus();

    // update replace text
    if( !lastSelection().replaceText().isEmpty() ) replaceWidget_->setReplaceText( lastSelection().replaceText() );

    return;
}

//_____________________________________________
void TextEditor::_updateReplaceInSelection()
{ if( replaceWidget_ ) replaceWidget_->enableReplaceInSelection( hasSelection() ); }

//________________________________________________
void TextEditor::_selectLineFromDialog()
{

    Debug::Throw( QStringLiteral("TextEditor::_selectLineFromDialog.\n") );

    if( useEmbeddedWidgets_ )
    {

        if( !selectLineWidget_ ) createSelectLineWidget( true );
        selectLineWidget_->show();

    } else {

        if( !selectLineDialog_ ) _createSelectLineDialog();
        selectLineDialog_->centerOnParent();
        selectLineDialog_->show();
        selectLineDialog_->activateWindow();

    }

    selectLineWidget_->matchFound();
    selectLineWidget_->editor().clear();
    selectLineWidget_->editor().setFocus();

}

//_________________________________________________________
TextEditor::Container::Container( QWidget* parent ):
    Container( parent, new TextEditor )
{}

//_________________________________________________________
TextEditor::Container::Container( QWidget* parent, TextEditor* editor ):
    QWidget( parent ),
    Counter( QStringLiteral("TextEditor::Container") ),
    editor_( editor )
{ _initialize(); }

//_________________________________________________________
void TextEditor::Container::_initialize()
{
    Debug::Throw( QStringLiteral("TextEditor::Container::_initialize.\n") );
    editor_->setParent( this );

    auto vLayout = new QVBoxLayout;
    QtUtil::setMargin(vLayout, 0);
    vLayout->setSpacing(2);
    setLayout( vLayout );

    // editor
    vLayout->addWidget( editor_ );
    editor_->useEmbeddedWidgets_ = true;

    // find widget
    editor_->createFindWidget( true );
    editor_->findWidget_->setParent( this );
    vLayout->addWidget( editor_->findWidget_ );
    editor_->findWidget_->hide();

    // replace widget
    editor_->createReplaceWidget( true );
    editor_->replaceWidget_->setParent( this );
    vLayout->addWidget( editor_->replaceWidget_ );
    editor_->replaceWidget_->hide();

    // select line dialog
    editor_->createSelectLineWidget( true );
    editor_->selectLineWidget_->setParent( this );
    vLayout->addWidget( editor_->selectLineWidget_ );
    editor_->selectLineWidget_->hide();

    // make connections so that focus is restored on close
    connect( &editor_->findWidget_->closeButton(), &QAbstractButton::clicked, this, [this](bool){ editor_->setFocus(); } );
    connect( &editor_->replaceWidget_->closeButton(), &QAbstractButton::clicked, this, [this](bool){ editor_->setFocus(); } );
    connect( &editor_->selectLineWidget_->closeButton(), &QAbstractButton::clicked, this, [this](bool){ editor_->setFocus(); } );

}
