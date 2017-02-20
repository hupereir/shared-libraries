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

#include "BaseIconNames.h"
#include "BaseContextMenu.h"
#include "BaseFindDialog.h"
#include "BaseFindWidget.h"
#include "BaseReplaceDialog.h"
#include "BaseReplaceWidget.h"
#include "InformationDialog.h"
#include "Color.h"
#include "CustomTextDocument.h"
#include "IconEngine.h"
#include "KeyModifier.h"
#include "LineNumberDisplay.h"
#include "QtUtil.h"
#include "SelectLineDialog.h"
#include "SelectLineWidget.h"
#include "Singleton.h"
#include "StandardAction.h"
#include "TextBlockData.h"
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
#include <QRegExp>
#include <QTextBlock>
#include <QTextStream>
#include <QToolTip>

//______________________________________________
TextSelection& TextEditor::lastSelection( void )
{
    static TextSelection selection;
    return selection;
}

//______________________________________________
TextEditor::TextEditor( QWidget *parent ):
    BaseEditor( parent ),
    Counter( "TextEditor" ),
    marginWidget_( new TextEditorMarginWidget( this ) ),
    boxSelection_( this ),
    cursorMonitor_( viewport() ),
    removeLineBuffer_( this ),
    clickCounter_( this, 4 )
{

    Debug::Throw( "TextEditor::TextEditor.\n" );

    // set customized document
    CustomTextDocument* document( new CustomTextDocument(0) );
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

    connect( this, SIGNAL(cursorPositionChanged()), &blockHighlight(), SLOT(highlight()) );
    connect( this, SIGNAL(copyAvailable(bool)), SLOT(_updateSelectionActions(bool)) );
    connect( this, SIGNAL(selectionChanged()), SLOT(_synchronizeSelection()) );
    connect( this, SIGNAL(selectionChanged()), SLOT(_updateClipboard()) );
    connect( this, SIGNAL(cursorPositionChanged()), SLOT(_synchronizeSelection()) );
    connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );

    // track changes of block counts
    connect( TextEditor::document(), SIGNAL(blockCountChanged(int)), SLOT(_blockCountChanged(int)) );
    connect( TextEditor::document(), SIGNAL(contentsChanged()), SLOT(_updateContentActions()) );
    connect( TextEditor::document(), SIGNAL(contentsChanged()), &_marginWidget(), SLOT(setDirty()) );

    // update configuration
    _updateConfiguration();
    _blockCountChanged(0);
    _updateContentActions();

    marginWidget_->show();

}

//________________________________________________
TextEditor::~TextEditor( void )
{

    Debug::Throw() << "TextEditor::~TextEditor - key: " << key() << endl;

    // cast document
    CustomTextDocument* document( qobject_cast<CustomTextDocument*>( TextEditor::document() ) );
    if( document && Base::KeySet<TextEditor>( document ).size() == 1 ) document->deleteLater();

    // update associates synchronization flags
    Base::KeySet<TextEditor> editors( this );

    // nothing to be done if no associates
    if( editors.empty() ) return;

    // keep position of current cursor
    int position( textCursor().position() );
    int anchor( textCursor().anchor() );

    // need to reset Text document
    // to avoid deletion while deleting this editor
    setDocument( new QTextDocument() );

    // keep reference to first associate
    TextEditor &editor( **editors.begin() );

    // recreate an appropriate cursor
    // this is dangerous
    QTextCursor cursor( editor.document() );
    cursor.setPosition( anchor );
    cursor.setPosition( position, QTextCursor::KeepAnchor );
    editor.setTextCursor( cursor );

    // turn off synchronization
    if( editors.size() == 1 ) editor.setSynchronized( false );

}

//________________________________________________
int TextEditor::blockCount( void ) const
{
    Debug::Throw( "TextEditor::blockCount.\n" );

    int count = 0;
    for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next() )
    { count += blockCount( block ); }
    return count;
}

//________________________________________________
TextPosition TextEditor::textPosition( void )
{

    QTextCursor cursor( textCursor() );
    QTextBlock block( cursor.block() );

    // calculate index
    TextPosition out;

    #if QT_VERSION >= 0x040200

    // direct access as introduced in Qt 4.2
    out.index() = cursor.columnNumber();
    out.paragraph() = cursor.blockNumber();

    #else

    // slow access for prior versions of Qt
    out.index() = cursor.position() - block.position();
    while( block.isValid() )
    {
        block = block.previous();
        out.paragraph()++;
    }

    // need to decrement once
    out.paragraph()--;

    #endif

    return out;

}

//________________________________________________
bool TextEditor::isCursorVisible( void ) const
{

    QRect cursor_rect( cursorRect() );
    QRect rect( viewport()->rect() );
    return rect.intersects( cursor_rect );

}

//______________________________________________________________________
bool TextEditor::hasSelection( void ) const
{ return textCursor().hasSelection() || !boxSelection_.empty(); }

//______________________________________________________________________
TextSelection TextEditor::selection( void ) const
{
    Debug::Throw( "TextEditor::selection.\n" );

    // copy last selection
    TextSelection out( "" );

    // copy attributes from last selection
    out.setFlag( TextSelection::CaseSensitive, lastSelection().flag( TextSelection::CaseSensitive ) );
    out.setFlag( TextSelection::EntireWord, lastSelection().flag( TextSelection::EntireWord ) );

    // try set from current selection
    QString text;
    if( !( text = qApp->clipboard()->text( QClipboard::Selection ) ).isEmpty() ) {

        Debug::Throw( "TextEditor::selection - from clipboard.\n" );
        out.setText( text );

    } else if( textCursor().hasSelection() ) {

        Debug::Throw() << "TextEditor::selection - from cursor: " << textCursor().selectedText() << endl;
        out.setText( textCursor().selectedText() );

    } else {

        Debug::Throw( "TextEditor::selection - from last selection.\n" );
        out.setText( lastSelection().text() );

    }

    return out;

}

//________________________________________________
void TextEditor::setPlainText( const QString& text )
{
    Debug::Throw( "TextEditor::setPlainText.\n" );

    lineNumberDisplay_->clear();

    bool enabled( blockHighlight_->isEnabled() );
    blockHighlight_->setEnabled( false );
    BaseEditor::setPlainText( text );
    blockHighlight_->setEnabled( enabled );

}

//________________________________________________
void TextEditor::setHtml( const QString& text )
{
    Debug::Throw( "TextEditor::setHtml.\n" );

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
    painter.setClipRect( QRect( 0, 0, _leftMargin(), height ), Qt::IntersectClip );
    painter.setPen( Qt::NoPen );

    painter.translate( 0, -verticalScrollBar()->value() );

    // draw current block rect
    if( blockHighlightAction_->isEnabled() && blockHighlightAction_->isChecked() && _currentBlockRect().isValid() )
    {

        painter.setBrush( palette().color( QPalette::AlternateBase ) );
        painter.drawRect( _currentBlockRect() );

    }

    if( marginWidget_->drawVerticalLine() ) {
        painter.setBrush( QBrush( marginWidget_->palette().color( QPalette::WindowText ), Qt::Dense4Pattern ) );
        painter.drawRect( _leftMargin()-1, verticalScrollBar()->value(), 1, height+verticalScrollBar()->value() );
    }

    // set brush and pen suitable to further painting
    painter.setBrush( Qt::NoBrush );
    painter.setPen(marginWidget_->palette().color( QPalette::WindowText )  );

    // draw lines
    if(
        _hasLineNumberDisplay() &&
        hasLineNumberAction() &&
        showLineNumberAction().isVisible() &&
        showLineNumberAction().isChecked() )
    { lineNumberDisplay_->paint( painter ); }

}

//________________________________________________
void TextEditor::selectWord( void )
{
    Debug::Throw( "TextEditor::selectWord.\n" );

    // retrieve text cursor, block and text
    QTextCursor cursor( textCursor() );
    QTextBlock block( cursor.block() );
    QString text( cursor.block().text() );

    // retrieve local cursor position in block
    int localPosition( cursor.position() - block.position() );
    int begin = localPosition;
    int end = localPosition;

    // parse text
    if( TextSeparator::get().base().find( text[begin] ) != TextSeparator::get().base().end() )
    {

        // see if cursor is in base separator list
        while( begin > 0 &&  TextSeparator::get().base().find( text[begin-1] ) != TextSeparator::get().base().end() ) begin--;
        while( end < text.size() && TextSeparator::get().base().find( text[end] ) != TextSeparator::get().base().end() ) end++;

    } else if( TextSeparator::get().extended().find( text[begin] ) != TextSeparator::get().extended().end() ) {

        // see if cursor is in extended separator list
        while( begin > 0 &&  TextSeparator::get().extended().find( text[begin-1] ) != TextSeparator::get().extended().end() ) begin--;
        while( end < text.size() && TextSeparator::get().extended().find( text[end] ) != TextSeparator::get().extended().end() ) end++;

    } else {

        // cursor is in word
        while( begin > 0 &&  TextSeparator::get().all().find( text[begin-1] ) == TextSeparator::get().all().end() ) begin--;
        while( end < (int)text.size() && TextSeparator::get().all().find( text[end] ) == TextSeparator::get().all().end() ) end++;

    }

    // move cursor to begin of selection
    for( ;begin < localPosition; localPosition-- ) { cursor.movePosition( QTextCursor::Left, QTextCursor::MoveAnchor ); }
    for( ;localPosition < end; localPosition++ ) { cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor ); }

    // assign cursor to Text editor
    setTextCursor( cursor );

    return;

}

//________________________________________________
void TextEditor::selectLine( void )
{
    Debug::Throw( "TextEditor::selectLine.\n" );

    QTextCursor cursor( textCursor() );
    QTextBlock block( cursor.block() );
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

    static QRegExp regexp( "\\s+$" );

    QTextCursor cursor( textCursor() );
    if( cursor.hasSelection() )
    {

        // get selection, look for trailing spaces
        QString text( cursor.selectedText() );
        if( regexp.indexIn( text ) >= 0 )
        {

            // create local cursor, copy current, in proper order
            QTextCursor local( document() );
            local.beginEditBlock();
            local.setPosition( qMin( cursor.position(), cursor.anchor() ), QTextCursor::MoveAnchor );
            local.setPosition( qMax( cursor.position(), cursor.anchor() ), QTextCursor::KeepAnchor );
            local.movePosition( QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, regexp.matchedLength() );

            local.mergeCharFormat( format );
            local.endEditBlock();

            return;

        }

    } else if( boxSelection_.state() == BoxSelection::SelectionFinished ) {

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
    Debug::Throw( "TextEditor::synchronize.\n" );

    // retrieve and cast old document
    CustomTextDocument* document( qobject_cast<CustomTextDocument*>( BaseEditor::document() ) );

    // assign new document and associate
    setDocument( editor->document() );
    Base::Key::associate( this, qobject_cast<CustomTextDocument*>( editor->document() ) );

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
    tabEmulationAction().setChecked( editor->tabEmulationAction().isChecked() );

    // synchronize wrap mode
    wrapModeAction().setChecked( editor->wrapModeAction().isChecked() );

    // track changes of block counts
    lineNumberDisplay_->synchronize( &editor->_lineNumberDisplay() );
    connect( TextEditor::document(), SIGNAL(blockCountChanged(int)), SLOT(_blockCountChanged(int)) );
    connect( TextEditor::document(), SIGNAL(contentsChanged()), SLOT(_updateContentActions()) );
    connect( TextEditor::document(), SIGNAL(contentsChanged()), &_marginWidget(), SLOT(setDirty()) );


    // margin
    _setLeftMargin( editor->_leftMargin() );

    return;

}

//_____________________________________________________________________
bool TextEditor::setActive( bool active )
{

    Debug::Throw( "TextEditor::setActive.\n" );

    // check if value is changed
    if( isActive() == active ) return false;
    active_ = active;
    return true;

}

//__________________________________________________________________
void TextEditor::showReplacements( int counts )
{

    Debug::Throw( "TextEditor::showReplacements.\n" );

    QString buffer;
    if( !counts ) buffer = QString( tr( "String not found." ) );
    else if( counts == 1 ) buffer = QString( tr( "1 replacement performed" ) );
    else buffer = QString( tr( "%1 replacements performed" ) ).arg( counts );
    InformationDialog( this, buffer ).setWindowTitle( QString( tr( "Replace in Text - %1" ) ).arg( qApp->applicationName() ) ).centerOnWidget( qApp->activeWindow() ).exec();

    return;

}

//__________________________________________________________________
void TextEditor::setTrackAnchors( bool value )
{
    Debug::Throw( "TextEditor::setTrackAnchors.\n" );
    trackAnchors_ = value;
    if( value ) setMouseTracking( true );
}

//__________________________________________________________________
void TextEditor::setReadOnly( bool readOnly )
{
    Debug::Throw( "TextEditor::setReadOnly.\n" );
    BaseEditor::setReadOnly( readOnly );
    _updateReadOnlyActions( readOnly );
    if( readOnly ) document()->setModified( false );
}

//____________________________________________________________________
void TextEditor::resetUndoRedoStack( void )
{
    if( isReadOnly() || !document()->isUndoRedoEnabled() ) return;

    Debug::Throw(" TextEditor::resetUndoRedoStack.\n");
    document()->setUndoRedoEnabled( false );
    document()->setUndoRedoEnabled( true );

}

//______________________________________________________________________________
void TextEditor::installContextMenuActions( BaseContextMenu* menu, bool allActions )
{

    Debug::Throw( "TextEditor::installContextMenuActions.\n" );

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

    Debug::Throw( "TextEditor::createFindWidget.\n" );
    if( !findWidget_ )
    {
        findWidget_ = new BaseFindWidget( this, compact );
        connect( findWidget_, SIGNAL(find(TextSelection)), SLOT(find(TextSelection)) );
        connect( this, SIGNAL(matchFound()), findWidget_, SLOT(matchFound()) );
        connect( this, SIGNAL(noMatchFound()), findWidget_, SLOT(noMatchFound()) );
        connect( this, SIGNAL(destroyed()), findWidget_, SLOT(deleteLater()) );
    }

    return;

}


//______________________________________________________________________
void TextEditor::createReplaceWidget( bool compact )
{

    Debug::Throw( "TextEditor::createReplaceWidget.\n" );
    if( !replaceWidget_ )
    {

        replaceWidget_ = new BaseReplaceWidget( this, compact );
        connect( replaceWidget_, SIGNAL(find(TextSelection)), SLOT(find(TextSelection)) );
        connect( replaceWidget_, SIGNAL(replace(TextSelection)), SLOT(replace(TextSelection)) );
        connect( replaceWidget_, SIGNAL(replaceInWindow(TextSelection)), SLOT(replaceInWindow(TextSelection)) );
        connect( replaceWidget_, SIGNAL(replaceInSelection(TextSelection)), SLOT(replaceInSelection(TextSelection)) );
        connect( replaceWidget_, SIGNAL(menuAboutToShow()), SLOT(_updateReplaceInSelection()) );
        connect( this, SIGNAL(matchFound()), replaceWidget_, SLOT(matchFound()) );
        connect( this, SIGNAL(noMatchFound()), replaceWidget_, SLOT(noMatchFound()) );
        connect( this, SIGNAL(destroyed()), replaceWidget_, SLOT(deleteLater()) );

    }

    return;

}

//________________________________________________
void TextEditor::createSelectLineWidget( bool compact )
{
    if( !selectLineWidget_ )
    {
        Debug::Throw( "TextEditor::createSelectLineWidget.\n" );
        selectLineWidget_ = new SelectLineWidget( this, compact );
        connect( selectLineWidget_, SIGNAL(lineSelected(int)), SLOT(selectLine(int)) );
        connect( this, SIGNAL(lineFound()), selectLineWidget_, SLOT(matchFound()) );
        connect( this, SIGNAL(lineNotFound()), selectLineWidget_, SLOT(noMatchFound()) );
        connect( this, SIGNAL(destroyed()), selectLineWidget_, SLOT(deleteLater()) );
   }
}

//___________________________________________________________________________
void TextEditor::setBackground( QTextBlock block, const QColor& color )
{

    Debug::Throw( "TextEditor::setBackground.\n" );

    // try retrieve data or create
    TextBlockData *data( static_cast<TextBlockData*>( block.userData() ) );
    if( !data ) block.setUserData( data = new TextBlockData() );

    // try assign color
    if( data->setBackground( color ) && updatesEnabled() )
    { document()->markContentsDirty(block.position(), block.length()-1); }
    return;

}

//___________________________________________________________________________
void TextEditor::clearBackground( QTextBlock block )
{

    Debug::Throw( "TextEditor::clearBackground.\n" );
    TextBlockData *data( static_cast<TextBlockData*>( block.userData() ) );
    if( data && data->hasFlag( TextBlock::HasBackground ) && data->setBackground( QColor() ) && updatesEnabled()  )
    { document()->markContentsDirty(block.position(), block.length()-1); }

    return;
}

//___________________________________________________________________________
void TextEditor::clearAllBackgrounds( void )
{
    Debug::Throw( "TextEditor::clearAllBackgrounds.\n" );
    for( QTextBlock block( document()->begin() ); block.isValid(); block = block.next() )
    { clearBackground( block ); }
}

//________________________________________________
void TextEditor::cut( void )
{

    Debug::Throw( "TextEditor::cut.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    if( boxSelection_.state() == BoxSelection::SelectionFinished )
    {
        boxSelection_.toClipboard( QClipboard::Clipboard );
        boxSelection_.removeSelectedText();
        boxSelection_.clear();
        emit copyAvailable( false );
    } else BaseEditor::cut();

    return;
}

//________________________________________________
void TextEditor::copy( void )
{
    Debug::Throw( "TextEditor::copy.\n" );
    if( boxSelection_.state() == BoxSelection::SelectionFinished ) boxSelection_.toClipboard( QClipboard::Clipboard );
    else BaseEditor::copy();
}

//________________________________________________
void TextEditor::paste( void )
{

    Debug::Throw( "TextEditor::paste.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    if( boxSelection_.state() == BoxSelection::SelectionFinished )
    {

        boxSelection_.fromClipboard( QClipboard::Clipboard );
        boxSelection_.clear();

    } else BaseEditor::paste();

}

//________________________________________________
void TextEditor::upperCase( void )
{
    Debug::Throw( "TextEditor::upperCase.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    QTextCursor cursor( textCursor() );
    if( cursor.hasSelection() )
    {

        // process standard selection
        cursor.insertText( cursor.selectedText().toUpper() );

    } else if( boxSelection_.state() == BoxSelection::SelectionFinished ) {

        // process box selection
        boxSelection_.toUpper();

    }

    return;

}

//________________________________________________
void TextEditor::lowerCase( void )
{

    Debug::Throw( "TextEditor::lowerCase.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    QTextCursor cursor( textCursor() );

    if( cursor.hasSelection() )
    {

        // process standard selection
        cursor.insertText( cursor.selectedText().toLower() );

    } else if( boxSelection_.state() == BoxSelection::SelectionFinished ) {

        // process box selection
        boxSelection_.toLower();

    }

    return;

}

//______________________________________________________________________
void TextEditor::find( TextSelection selection )
{
    Debug::Throw( "TextEditor::find.\n" );
    bool found( selection.flag( TextSelection::Backward ) ? _findBackward( selection, true ):_findForward( selection, true ) );
    if( found ) emit matchFound();
    else emit noMatchFound();
}

//______________________________________________________________________
void TextEditor::findSelectionForward( void )
{
    Debug::Throw( "TextEditor::findSelectionForward.\n" );
    _findForward( selection(), true );
}

//______________________________________________________________________
void TextEditor::findSelectionBackward( void )
{
    Debug::Throw( "TextEditor::findSelectionBackward.\n" );
    _findBackward( selection(), true );
}

//______________________________________________________________________
void TextEditor::findAgainForward( void )
{
    Debug::Throw( "TextEditor::findAgainForward.\n" );
    _findForward( lastSelection(), true );
}

//______________________________________________________________________
void TextEditor::findAgainBackward( void )
{
    Debug::Throw( "TextEditor::findAgainBackward.\n" );
    _findBackward( lastSelection(), true );
}

//______________________________________________________________________
void TextEditor::replace( TextSelection selection )
{

    Debug::Throw( "TextEditor::replace.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    // see if current selection match
    // perform replacement if yes
    QTextCursor cursor( textCursor() );
    bool accepted( true );
    accepted &= cursor.hasSelection();
    if( selection.flag( TextSelection::RegExp ) )
    {
        accepted &= QRegExp( selection.text() ).exactMatch( cursor.selectedText() );
    } else {

        accepted &= ( !cursor.selectedText().compare(
            selection.text(),
            selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive ) );

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
void TextEditor::replaceAgainForward( void )
{
    Debug::Throw( "TextEditor::replaceAgainForward.\n" );
    TextSelection selection( lastSelection() );
    selection.setFlag( TextSelection::Backward, false );
    replace( selection );
}

//______________________________________________________________________
void TextEditor::replaceAgainBackward( void )
{
    Debug::Throw( "TextEditor::replaceAgainBackward.\n" );
    TextSelection selection( lastSelection() );
    selection.setFlag( TextSelection::Backward, true );
    replace( selection );
}

//______________________________________________________________________
unsigned int TextEditor::replaceInSelection( TextSelection selection, bool showDialog )
{

    Debug::Throw( "TextEditor::replaceInSelection.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return 0;

    // progress dialog
    if( showDialog ) _createProgressDialog();

    unsigned int counts(0);

    if( boxSelection_.state() == BoxSelection::SelectionFinished )
    {

        Debug::Throw( "TextEditor::replaceInSelection - box selection.\n" );
        BoxSelection::CursorList cursors( boxSelection_.cursorList() );
        for( auto& cursor:cursors )
        { counts += _replaceInRange( selection, cursor, MoveCursor ); }

        boxSelection_.clear();

    } else {
        Debug::Throw( "TextEditor::replaceInSelection - normal selection.\n" );
        QTextCursor cursor( textCursor() );
        counts = _replaceInRange( selection, cursor, ExpandCursor );
    }

    Debug::Throw( "TextEditor::replaceInSelection - done.\n" );
    if( showDialog ) showReplacements( counts );
    return counts;

}

//______________________________________________________________________
unsigned int TextEditor::replaceInWindow( TextSelection selection, bool showDialog )
{

    Debug::Throw( "TextEditor::replaceInWindow.\n" );

    // progress dialog
    if( showDialog ) _createProgressDialog();

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return 0;

    QTextCursor cursor( textCursor() );
    cursor.movePosition( QTextCursor::Start );
    cursor.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
    unsigned int counts( _replaceInRange( selection, cursor, ExpandCursor ) );

    if( showDialog ) showReplacements( counts );
    return counts;

}

//________________________________________________
void TextEditor::selectLine( int index )
{

    Debug::Throw() << "TextEditor::selectLine - index: " << index << endl;
    int localIndex( 0 );
    QTextBlock block = document()->begin();
    for( ;localIndex < index && block.isValid(); block = block.next(), localIndex++ )
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

    Debug::Throw( "TextEditor::removeLine.\n" );

    // need to check for editability because apparently even if calling action is disabled,
    // the shortcut still can be called
    if( isReadOnly() ) return;

    QTextCursor cursor( textCursor() );
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
void TextEditor::clear( void )
{
    Debug::Throw( "TextEditor::clear.\n" );

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
            QHelpEvent* helpEvent( static_cast<QHelpEvent*>(event) );
            const QPoint position( helpEvent->pos() );
            const QPoint globalPosition( helpEvent->globalPos() );
            QString anchor;
            if( trackAnchors_ && !( anchor = anchorAt( position ) ).isEmpty() )
            {
                QRect rect;
                QTextCursor cursor( cursorForPosition( position ) );
                QTextBlock block( cursor.block() );

                for( auto&& it = block.begin(); !(it.atEnd()); ++it)
                {
                    QTextFragment fragment = it.fragment();
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
void TextEditor::enterEvent( QEvent* event )
{

    #if QT_VERSION < 0x040200
    Debug::Throw( "TextEditor::enterEvent.\n" );
    _updateClipboardActions( QClipboard::Clipboard );
    _updateClipboardActions( QClipboard::Selection );
    #endif

    BaseEditor::enterEvent( event );

}

//________________________________________________
void TextEditor::mousePressEvent( QMouseEvent* event )
{

    Debug::Throw( "TextEditor::mousePressEvent.\n" );

    // check button
    if( event->button() == Qt::LeftButton )
    {

        // increment multiple clicks
        QTextCursor cursor( textCursor() );

        // increment counter
        clickCounter_.increment( cursorForPosition( event->pos() ).position() );
        switch( clickCounter_.counts() )
        {

            case 1:
            {
                // if single click in existing box selection, store drag position
                if(
                    event->modifiers() == Qt::NoModifier &&
                    boxSelection_.state() == BoxSelection::SelectionFinished &&
                    boxSelection_.rect().contains( fromViewport( event->pos() ) )
                    )
                {
                    // store position for drag
                    dragStart_ = event->pos();
                    return BaseEditor::mousePressEvent( event );
                }

                // if single click outside of existing box selection, clear the selection
                if( event->button() == Qt::LeftButton && boxSelection_.state() == BoxSelection::SelectionFinished )
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

                return BaseEditor::mousePressEvent( event );
                break;

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
    if(  event->button() == Qt::MidButton )
    { setTextCursor( cursorForPosition( event->pos() ) ); }

}

//________________________________________________
void TextEditor::mouseDoubleClickEvent( QMouseEvent* event )
{

    Debug::Throw( "TextEditor::mouseDoubleClickEvent.\n" );

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

    Debug::Throw( 2, "TextEditor::mouseMoveEvent.\n" );

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
    if( event->buttons() == Qt::LeftButton && boxSelection_.isEnabled() && boxSelection_.state() == BoxSelection::SelectionStarted )
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
    if( event->buttons() == Qt::LeftButton && boxSelection_.state() == BoxSelection::SelectionFinished && (event->pos() - dragStart_ ).manhattanLength() > QApplication::startDragDistance() )
    {
        // start drag
        QDrag *drag = new QDrag(this);

        // store data
        QString text( boxSelection_.toString() );

        QMimeData *data = new QMimeData();
        data->setText( text );
        data->setData( BoxSelection::mimeType, qPrintable( text ) );
        drag->setMimeData( data );
        drag->start();

        return;
    }

    return BaseEditor::mouseMoveEvent( event );

}

//________________________________________________
void TextEditor::mouseReleaseEvent( QMouseEvent* event )
{

    Debug::Throw( "TextEditor::mouseReleaseEvent.\n" );

    autoScrollTimer_.stop();

    // no need to check for enability because there is no way for the box to start if disabled
    if( event->button() == Qt::LeftButton && boxSelection_.state() == BoxSelection::SelectionStarted )
    {

        boxSelection_.finish( event->pos() );
        _synchronizeBoxSelection();
        return BaseEditor::mouseReleaseEvent( event );

    }

    if( event->button() == Qt::LeftButton && boxSelection_.state() == BoxSelection::SelectionFinished )
    {

        boxSelection_.clear();
        _synchronizeBoxSelection();
        emit copyAvailable( false );
        return BaseEditor::mouseReleaseEvent( event );

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

    if( event->button() == Qt::MidButton  && boxSelection_.state() == BoxSelection::SelectionFinished )
    {
        boxSelection_.clear();
        boxSelection_.clear();
    }

    // process event
    BaseEditor::mouseReleaseEvent( event );

}

//________________________________________________
void TextEditor::dropEvent( QDropEvent* event )
{

    Debug::Throw( "TextEditor::dropEvent.\n" );

    // static empty mimeData used to pass to base class
    // so that drop events are finished properly even when actually doing nothing
    static QMimeData* empty_data( new QMimeData() );
    QDropEvent empty_event( event->pos(), event->possibleActions(), empty_data, Qt::NoButton, Qt::NoModifier );

    // if mimeData is block selection, block selection is enabled here
    // and there is no active selection (standard or box), insert new box selection
    // at cursor position
    if(
        event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
        boxSelection_.isEnabled() &&
        boxSelection_.state() == BoxSelection::SelectionEmpty &&
        !textCursor().hasSelection() )
    {

        Debug::Throw( "TextEditor::dropEvent - dropping box selection.\n" );

        // retrieve text from mimeType
        QString text( event->mimeData()->text() );
        QStringList input_list( text.split( "\n" ) );

        // create an empty boxSelection from current position with proper size
        boxSelection_.start( event->pos() );
        boxSelection_.finish( event->pos() );
        boxSelection_.fromString( text );
        boxSelection_.clear();

        event->acceptProposedAction();
        BaseEditor::dropEvent( &empty_event );
        return;

    }

    if(
        event->mimeData()->hasFormat( BoxSelection::mimeType ) &&
        boxSelection_.isEnabled() &&
        boxSelection_.state() == BoxSelection::SelectionFinished &&
        !toViewport( boxSelection_.rect() ).contains( event->pos() ) &&
        event->source() == this
        )
    {
        // drag is box selection and from this window. Move current block selection around.
        Debug::Throw( "TextEditor::dropEvent - [box] moving current box selection.\n" );

        // count rows in current selection
        int rowCount( boxSelection_.cursorList().size() - 1 );

        // store cursor at new insertion position
        QTextCursor newCursor( cursorForPosition( event->pos() ) );

        // remove current selection
        boxSelection_.removeSelectedText();
        boxSelection_.clear();

        // prepare new selection
        QRect rect( cursorRect( newCursor ) );
        QPoint start( rect.center().x(), rect.top() );
        QPoint end( rect.center().x(), rect.top() + rowCount*QFontMetrics( font() ).height() );

        boxSelection_.start( start );
        boxSelection_.finish( end );

        // join modifications with previous so that they appear as one entry in undo/redo list
        newCursor.joinPreviousEditBlock();

        // insert text in new box
        boxSelection_.fromString( event->mimeData()->text() );
        boxSelection_.clear();
        newCursor.endEditBlock();

        event->acceptProposedAction();
        BaseEditor::dropEvent( &empty_event );
        return;


    }

    // check if there is one valid box selection that contains the drop point
    if(
        event->mimeData()->hasText() &&
        boxSelection_.isEnabled() &&
        boxSelection_.state() == BoxSelection::SelectionFinished &&
        toViewport( boxSelection_.rect() ).contains( event->pos() ) )
    {

        if( event->source() == this )
        {

            // current selection is inserted in itself. Doing nothing
            Debug::Throw( "TextEditor::dropEvent - [box] doing nothing.\n" );
            event->acceptProposedAction();
            BaseEditor::dropEvent( &empty_event );
            return;

        } else {

            // insert mine data in current box selection
            Debug::Throw( "TextEditor::dropEvent - [box] inserting selection.\n" );
            boxSelection_.fromString( event->mimeData()->text() );
            setTextCursor( boxSelection_.cursorList().back() );
            boxSelection_.clear();
            event->acceptProposedAction();
            BaseEditor::dropEvent( &empty_event );
            return;

        }
    }

    // retrieve selection bounding rect
    if( event->mimeData()->hasText() && textCursor().hasSelection() )
    {
        QTextCursor cursor( textCursor() );
        QTextCursor newCursor( cursorForPosition( event->pos() ) );

        bool contained(
            newCursor.position() >= qMin( cursor.position(), cursor.anchor() ) &&
            newCursor.position() <= qMax( cursor.position(), cursor.anchor() ) );

        if( contained && event->source() != this )
        {

            // drag action is from another widget and ends in selection. Replace this selection
            Debug::Throw( "TextEditor::dropEvent - inserting selection.\n" );
            cursor.insertText( event->mimeData()->text() );
            event->acceptProposedAction();
            BaseEditor::dropEvent( &empty_event );
            return;

        }

        if( event->source() == this )
        {

            // drag action is from this widget
            // insert selection at current location and remove old selection
            Debug::Throw( "TextEditor::dropEvent - moving selection.\n" );
            cursor.beginEditBlock();
            cursor.removeSelectedText();
            cursor.setPosition( newCursor.position() );
            cursor.insertText( event->mimeData()->text() );
            cursor.endEditBlock();
            setTextCursor( cursor );

            event->acceptProposedAction();
            BaseEditor::dropEvent( &empty_event );
            return;

        }

    }

    // for all other cases, use default
    return BaseEditor::dropEvent( event );

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
    if( boxSelection_.state() == BoxSelection::SelectionFinished )
    {
        if(
            (event->key() >= Qt::Key_Shift &&  event->key() <= Qt::Key_ScrollLock) ||
            (event->key() >= Qt::Key_F1 &&  event->key() <= Qt::Key_F25) ||
            (event->key() >= Qt::Key_Super_L && event->key() <= Qt::Key_Direction_R ) ||
            (event->modifiers() != Qt::NoModifier && event->modifiers() != Qt::ShiftModifier ) )
        { return BaseEditor::keyPressEvent( event ); }

        // if cursor move clear selection
        if( event->key() >= Qt::Key_Home && event->key() <= Qt::Key_Down )
        {
            boxSelection_.clear();
            return BaseEditor::keyPressEvent( event );
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
    if( event->key() == Qt::Key_CapsLock ) changed = _setModifier( ModifierCapsLock, !modifier( ModifierCapsLock ) );
    else if( event->key() == Qt::Key_NumLock ) changed = _setModifier( ModifierNumLock, !modifier( ModifierNumLock ) );
    if( changed ) { emit modifiersChanged( modifiers() ); }

    return;
}

//_______________________________________________________
void TextEditor::focusInEvent( QFocusEvent* event )
{
    Debug::Throw() << "TextEditor::focusInEvent - " << key() << endl;

    if(
        _setModifier( ModifierCapsLock, KeyModifier( Qt::Key_CapsLock ).state() == KeyModifier::On ) ||
        _setModifier( ModifierNumLock, KeyModifier( Qt::Key_NumLock ).state() == KeyModifier::On ) )
    { emit modifiersChanged( modifiers() );}

    emit hasFocus( this );
    BaseEditor::focusInEvent( event );
}

//________________________________________________
void TextEditor::contextMenuEvent( QContextMenuEvent* event )
{

    Debug::Throw( "TextEditor::contextMenuEvent.\n" );
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
    if( !_hasLineNumberDisplay() ) return;

    // tell line number display to update at next draw
    lineNumberDisplay_->needUpdate();

}

//______________________________________________________________
void TextEditor::paintEvent( QPaintEvent* event )
{

    // handle block background
    QTextBlock first( cursorForPosition( event->rect().topLeft() ).block() );
    QTextBlock last( cursorForPosition( event->rect().bottomRight() ).block() );

    // create painter and translate from widget to viewport coordinates
    QPainter painter( viewport() );
    painter.setClipRect( event->rect() );

    painter.translate( -scrollbarPosition() );
    painter.setPen( Qt::NoPen );

    // loop over found blocks
    for( QTextBlock block( first ); block != last.next() && block.isValid(); block = block.next() )
    {

        // retrieve block data and check background
        // static cast is use because should be faster and safe enough here
        TextBlockData *data( static_cast<TextBlockData*>( block.userData() ) );
        if( !(data && data->hasFlag( TextBlock::HasBackground|TextBlock::CurrentBlock ) ) ) continue;

        // retrieve block rect
        QRectF blockRect( document()->documentLayout()->blockBoundingRect( block ) );
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

    if( boxSelection_.state() == BoxSelection::SelectionStarted || boxSelection_.state() == BoxSelection::SelectionFinished )
    {
        painter.setPen( boxSelection_.color() );
        painter.setBrush( boxSelection_.brush() );
        painter.drawRect( boxSelection_.rect().translated( 2, 2 ) );
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

        const QPoint globalPosition = QCursor::pos();
        const QPoint position = viewport()->mapFromGlobal(globalPosition);
        QMouseEvent mouseEvent(QEvent::MouseMove, position, globalPosition, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        mouseMoveEvent(&mouseEvent);

    } else return BaseEditor::timerEvent( event );

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
void TextEditor::_installActions( void )
{
    Debug::Throw( "TextEditor::_installActions.\n" );

    // create actions
    addAction( undoAction_ = new StandardAction( StandardAction::Undo, this ) );
    undoAction_->setEnabled( document()->isUndoAvailable() && !isReadOnly() );
    connect( undoAction_, SIGNAL(triggered()), document(), SLOT(undo()) );
    connect( this, SIGNAL(undoAvailable(bool)), this, SLOT(_updateUndoRedoActions()) );

    addAction( redoAction_ = new StandardAction( StandardAction::Redo, this ) );
    redoAction_->setEnabled( document()->isRedoAvailable() && !isReadOnly() );
    connect( redoAction_, SIGNAL(triggered()), document(), SLOT(redo()) );
    connect( this, SIGNAL(redoAvailable(bool)), this, SLOT(_updateUndoRedoActions()) );

    addAction( cutAction_ = new StandardAction( StandardAction::Cut, this ) );
    cutAction_->setShortcut( QKeySequence::Cut );
    connect( cutAction_, SIGNAL(triggered()), SLOT(cut()) );

    addAction( copyAction_ = new StandardAction( StandardAction::Copy, this ) );
    connect( copyAction_, SIGNAL(triggered()), SLOT(copy()) );

    addAction( pasteAction_ = new StandardAction( StandardAction::Paste, this ) );
    connect( pasteAction_, SIGNAL(triggered()), SLOT(paste()) );
    connect( qApp->clipboard(), SIGNAL(dataChanged()), SLOT(_updatePasteAction()) );
    _updatePasteAction();

    addAction( clearAction_ = new QAction( tr( "Clear" ), this ) );
    connect( clearAction_, SIGNAL(triggered()), SLOT(clear()) );

    addAction( selectAllAction_ = new QAction( tr( "Select All" ), this ) );
    selectAllAction_->setShortcut( QKeySequence::SelectAll );
    selectAllAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( selectAllAction_, SIGNAL(triggered()), SLOT(selectAll()) );

    addAction( upperCaseAction_ = new QAction( tr( "Upper Case" ), this ) );
    upperCaseAction_->setShortcut( Qt::CTRL + Qt::Key_U );
    upperCaseAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( upperCaseAction_, SIGNAL(triggered()), SLOT(upperCase()) );

    addAction( lowerCaseAction_ = new QAction( tr( "Lower Case" ), this ) );
    lowerCaseAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_U );
    lowerCaseAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( lowerCaseAction_, SIGNAL(triggered()), SLOT(lowerCase()) );

    addAction( findAction_ = new QAction( IconEngine::get( IconNames::Find ), tr( "Find..." ), this ) );
    findAction_->setShortcut( QKeySequence::Find );
    findAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAction_, SIGNAL(triggered()), SLOT(_findFromDialog()) );

    addAction( findAgainAction_ = new QAction( tr( "Find Again" ), this ) );
    findAgainAction_->setShortcut( Qt::CTRL + Qt::Key_G );
    findAgainAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainAction_, SIGNAL(triggered()), SLOT(findAgainForward()) );

    addAction( findAgainBackwardAction_ = new QAction( this ) );
    findAgainBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_G );
    findAgainBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findAgainBackwardAction_, SIGNAL(triggered()), SLOT(findAgainBackward()) );

    addAction( findSelectionAction_ = new QAction( tr( "Find Selection" ), this ) );
    findSelectionAction_->setShortcut( Qt::CTRL + Qt::Key_H );
    findSelectionAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionAction_, SIGNAL(triggered()), SLOT(findSelectionForward()) );

    addAction( findSelectionBackwardAction_ = new QAction( this ) );
    findSelectionBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_H );
    findSelectionBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( findSelectionBackwardAction_, SIGNAL(triggered()), SLOT(findSelectionBackward()) );

    addAction( replaceAction_ = new QAction( IconEngine::get( IconNames::Find ), tr( "Replace..." ), this ) );
    replaceAction_->setShortcut( QKeySequence::Replace );
    replaceAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( replaceAction_, SIGNAL(triggered()), SLOT(_replaceFromDialog()) );

    addAction( replaceAgainAction_ = new QAction( tr( "Replace Again" ), this ) );
    replaceAgainAction_->setShortcut( Qt::CTRL + Qt::Key_T );
    replaceAgainAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( replaceAgainAction_, SIGNAL(triggered()), SLOT(replaceAgainForward()) );

    addAction( replaceAgainBackwardAction_ = new QAction( this ) );
    replaceAgainBackwardAction_->setShortcut( Qt::SHIFT + Qt::CTRL + Qt::Key_T );
    replaceAgainBackwardAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( replaceAgainBackwardAction_, SIGNAL(triggered()), SLOT(replaceAgainBackward()) );

    addAction( gotoLineAction_ = new QAction( tr( "Goto Line Number..." ), this ) );
    gotoLineAction_->setShortcut( Qt::CTRL + Qt::Key_L );
    gotoLineAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( gotoLineAction_, SIGNAL(triggered()), SLOT(_selectLineFromDialog()) );

    // remove line action
    QAction* remove_line_action( new QAction( tr( "Remove Current Line" ), this ) );
    addAction( remove_line_action );
    remove_line_action->setShortcut( Qt::CTRL + Qt::Key_K );
    remove_line_action->setShortcutContext( Qt::WidgetShortcut );
    connect( remove_line_action, SIGNAL(triggered()), SLOT(removeLine()) );

    // current block highlight
    addAction( blockHighlightAction_ = new QAction( tr( "Highlight Current Paragraph" ), this ) );
    blockHighlightAction_->setCheckable( true );
    blockHighlightAction_->setChecked( blockHighlight_->isEnabled() );
    blockHighlightAction_->setShortcut( Qt::Key_F12 );
    blockHighlightAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( blockHighlightAction_, SIGNAL(toggled(bool)), SLOT(_toggleBlockHighlight(bool)) );

    // wrap mode
    addAction( wrapModeAction_ = new QAction( tr( "Wrap Text" ), this ) );
    wrapModeAction_->setCheckable( true );
    wrapModeAction_->setChecked( lineWrapMode() == BaseEditor::WidgetWidth );
    _setModifier( ModifierWrap, lineWrapMode() == BaseEditor::WidgetWidth );
    wrapModeAction_->setShortcut( Qt::Key_F10 );
    wrapModeAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( wrapModeAction_, SIGNAL(toggled(bool)), SLOT(_toggleWrapMode(bool)) );

    // tab emulation action
    addAction( tabEmulationAction_ = new QAction( tr( "Emulate Tabs" ), this ) );
    tabEmulationAction_->setCheckable( true );
    tabEmulationAction_->setChecked( hasTabEmulation_ );
    connect( tabEmulationAction_, SIGNAL(toggled(bool)), SLOT(_toggleTabEmulation(bool)) );

    // line number action
    addAction( showLineNumberAction_ =new QAction( tr( "Show Line Numbers" ), this ) );
    showLineNumberAction_->setToolTip( tr( "Show/hide line numbers" ) );
    showLineNumberAction_->setCheckable( true );
    showLineNumberAction_->setShortcut( Qt::Key_F11 );
    showLineNumberAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( showLineNumberAction_, SIGNAL(toggled(bool)), SLOT(_toggleShowLineNumbers(bool)) );

    // copy link
    addAction( copyLinkAction_ = new QAction( IconEngine::get( IconNames::Copy ), tr( "Copy Link Location" ), this ) );
    connect( copyLinkAction_, SIGNAL(triggered()), this, SLOT(_copyLinkLocation()) );
    // update actions that depend on the presence of a selection
    _updateSelectionActions( textCursor().hasSelection() );

    #if QT_VERSION >= 0x040200
    // update actions that depend on the content of the clipboard
    // this is available only starting from Qt 4.2
    connect( qApp->clipboard(), SIGNAL(changed(QClipboard::Mode)), SLOT(_updateClipboardActions(QClipboard::Mode)) );
    #endif

}

//______________________________________________________________________
void TextEditor::_createFindDialog( void )
{

    Debug::Throw( "TextEditor::_createFindDialog.\n" );
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
void TextEditor::_createReplaceDialog( void )
{

    Debug::Throw( "TextEditor::_createReplaceDialog.\n" );
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
void TextEditor::_createSelectLineDialog( void )
{
    if( !selectLineDialog_ )
    {
        Debug::Throw( "TextEditor::_createSelectLineDialog.\n" );
        selectLineDialog_ = new SelectLineDialog( this );
        if( !selectLineWidget_ ) createSelectLineWidget( false );
        selectLineDialog_->setSelectLineWidget( selectLineWidget_ );
    }

    return;
}

//__________________________________________________
void TextEditor::_createProgressDialog( void )
{

    Debug::Throw( "TextEditor::_createProgressDialog.\n" );

    // create dialog
    QProgressDialog* dialog = new QProgressDialog(0);
    dialog->setAttribute( Qt::WA_DeleteOnClose, true );
    dialog->setLabelText( tr( "Replace text in selection" ) );
    dialog->setWindowTitle( tr( "Replace in Text" ) );

    // connections
    connect( this, SIGNAL(busy(int)), dialog, SLOT(setMaximum(int)) );
    connect( this, SIGNAL(progressAvailable(int)), dialog, SLOT(setValue(int)) );
    connect( this, SIGNAL(idle()), dialog, SLOT(close()) );

    QtUtil::centerOnWidget( dialog, this );
    dialog->show();

}

//______________________________________________________________________
bool TextEditor::_findForward( const TextSelection& selection, bool rewind )
{
    Debug::Throw( "TextEditor::_findForward.\n" );
    if( selection.text().isEmpty() ) return false;

    // store selection
    setLastSelection( selection );

    // retrieve current cursor
    QTextCursor cursor( textCursor() );

    // if no_increment, start from the beginning of the possible current selection
    if( cursor.hasSelection() && selection.flag( TextSelection::NoIncrement ) )
    { cursor.setPosition( cursor.anchor() ); }

    if( selection.flag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        QRegExp regexp( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        regexp.setCaseSensitivity( selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive );

        // make a copy of current cursor
        QTextCursor found( cursor );

        // if current text has selection that match, make sure pointer is located at the end of it
        if( found.hasSelection() && regexp.exactMatch( found.selectedText() ) )
        { found.setPosition( qMax( found.position(), found.anchor() ) ); }

        // move the found to the end of the document
        // and retrieve selected text
        found.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
        QString text( found.selectedText() );

        // parse text
        int match = regexp.indexIn( text );
        int length = regexp.matchedLength();
        if( match < 0 )
        {
            // no match found
            // if not rewind, stop here
            if( !rewind ) return false;

            // update selection to the beginning of the document
            found.movePosition( QTextCursor::Start, QTextCursor::KeepAnchor );
            text = found.selectedText();
            match = regexp.indexIn( text );
            length = regexp.matchedLength();

        }

        // no match found. Return
        if( match < 0 ) return false;

        // match found. Update selection and return
        int position( match + qMin( found.anchor(), found.position() ) );
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
        if( selection.flag( TextSelection::CaseSensitive ) )  flags |= QTextDocument::FindCaseSensitively;
        if( selection.flag( TextSelection::EntireWord ) ) flags |= QTextDocument::FindWholeWords;

        QTextCursor found( document()->find( selection.text(), cursor, flags ) );

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

    Debug::Throw( "TextEditor::_findBackward.\n" );
    if( selection.text().isEmpty() ) return false;
    setLastSelection( selection );

    // retrieve current cursor
    QTextCursor cursor( textCursor() );

    // if no_increment, start from the beginning of the possible current selection
    if( cursor.hasSelection() && selection.flag( TextSelection::NoIncrement ) )
    { cursor.setPosition( cursor.anchor()+selection.text().size()+1 ); }

    if( selection.flag( TextSelection::RegExp ) )
    {

        // construct regexp and check
        QRegExp regexp( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        regexp.setCaseSensitivity( selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive );

        // make a copy of current cursor
        QTextCursor found( cursor );

        // if current text has selection that match, make sure pointer is located at the end of it
        if( found.hasSelection() && regexp.exactMatch( found.selectedText() ) )
        { found.setPosition( qMin( found.position(), found.anchor() ) ); }

        // move cursor to beginning of the text
        found.movePosition( QTextCursor::Start, QTextCursor::KeepAnchor );
        QString text( found.selectedText() );

        // parse text
        int match = regexp.lastIndexIn( text );
        int length = regexp.matchedLength();
        if( match < 0 )
        {
            // no match found
            // if not rewind, stop here
            if( !rewind ) return false;

            // update selection to the beginning of the document
            found.movePosition( QTextCursor::End, QTextCursor::KeepAnchor );
            text = found.selectedText();
            match = regexp.lastIndexIn( text );
            length = regexp.matchedLength();

        }

        // no match found. Return
        if( match < 0 ) return false;

        // match found. Update selection and return
        int position( match + qMin( found.anchor(), found.position() )+length );
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
        if( selection.flag( TextSelection::CaseSensitive ) )  flags |= QTextDocument::FindCaseSensitively;
        if( selection.flag( TextSelection::EntireWord ) ) flags |= QTextDocument::FindWholeWords;

        QTextCursor found( document()->find( selection.text(), cursor, flags ) );

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
unsigned int TextEditor::_replaceInRange( const TextSelection& selection, QTextCursor& cursor, CursorMode mode )
{

    Debug::Throw()
        << "TextEditor::_replaceInRange -"
        << " anchor: " << cursor.anchor()
        << " position: " << cursor.position()
        << " selection: " << selection.text()
        << " replacement: " << selection.replaceText()
        << endl;

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
    unsigned int found = 0;

    int savedAnchor( qMin( cursor.position(), cursor.anchor() ) );
    int savedPosition( qMax( cursor.position(), cursor.anchor() ) );
    int currentPosition( savedAnchor );

    // check if regexp should be used or not
    if( selection.flag( TextSelection::RegExp ) )
    {

        Debug::Throw( "TextEditor::_replaceInRange - regexp.\n" );

        // construct regexp and check
        QRegExp regexp( selection.text() );
        if( !regexp.isValid() )
        {
            InformationDialog( this, tr( "Invalid regular expression. Find canceled" ) ).exec();
            return false;
        }

        // case sensitivity
        regexp.setCaseSensitivity( selection.flag( TextSelection::CaseSensitive ) ? Qt::CaseSensitive : Qt::CaseInsensitive );

        // replace everything in selected text
        QString selectedText( cursor.selectedText() );
        emit busy( selectedText.size() );

        for( int position = 0; (position = regexp.indexIn( selectedText, position )) != -1; )
        {
            // replace in selected text
            selectedText.replace( position, regexp.matchedLength(), selection.replaceText() );

            // replace in cursor
            /* this is to allow for undoing the changes one by one */
            cursor.setPosition( savedAnchor + position );
            cursor.setPosition( savedAnchor + position + regexp.matchedLength(), QTextCursor::KeepAnchor );
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
        if( mode == ExpandCursor )
        {
            cursor.setPosition( savedAnchor );
            cursor.setPosition( savedAnchor + selectedText.length(), QTextCursor::KeepAnchor );

        } else if( mode == MoveCursor ) cursor.setPosition( currentPosition );

    } else {

        Debug::Throw( "TextEditor::_replaceInRange - normal replacement.\n" );

        emit busy( cursor.selectedText().size() );

        // changes local cursor to beginning of the selection
        cursor.setPosition( savedAnchor );

        // define search flags
        QTextDocument::FindFlags flags(0);
        if( selection.flag( TextSelection::CaseSensitive ) )  flags |= QTextDocument::FindCaseSensitively;
        if( selection.flag( TextSelection::EntireWord ) ) flags |= QTextDocument::FindWholeWords;

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

        if( mode == ExpandCursor )
        {
            cursor.setPosition( savedAnchor );
            cursor.setPosition( savedPosition, QTextCursor::KeepAnchor );
        } else if( mode == MoveCursor ) cursor.setPosition( currentPosition );

    }

    return found;

}

//_____________________________________________________________
void TextEditor::_synchronizeBoxSelection( void ) const
{

    if( !isSynchronized() ) return;

    // Debug::Throw( "TextEditor::_synchronizeBoxSelection.\n" );
    Base::KeySet<TextEditor> displays( this );
    for( const auto& editor:Base::KeySet<TextEditor>(this) )
    { editor->boxSelection_.synchronize( boxSelection_ ); }

}

//_____________________________________________________________
bool TextEditor::_setLeftMargin( const int& margin )
{

    Debug::Throw() << "TextEditor::_setLeftMargin - margin: " << margin << endl;
    if( margin == _leftMargin() ) return false;

    leftMargin_ = margin;
    setViewportMargins( _leftMargin(), 0, 0, 0 );
    marginWidget_->resize( _leftMargin(), marginWidget_->height() );
    return true;

}

//_____________________________________________________________
void TextEditor::_toggleOverwriteMode( void )
{

    Debug::Throw( "TextEditor::_toggleOverwriteMode.\n" );
    setOverwriteMode( !overwriteMode() );
    if( _setModifier( ModifierInsert, overwriteMode() ) ) emit modifiersChanged( modifiers() );
    return;

}

//________________________________________________
bool TextEditor::_setTabSize( const int& tabSize )
{

    Debug::Throw() << "TextEditor::_setTabSize - " << tabSize << endl;
    Q_ASSERT( tabSize > 0 );

    int stop_width( tabSize * QFontMetrics( font() ).width( " " ) );
    if( tabSize == emulatedTab_.size() && tabStopWidth() == stop_width )
    { return false; }

    // create strings and regular expressions
    // define normal tabs
    normalTab_ = "\t";
    normalTabRegexp_.setPattern( "^(\\t)+" );
    setTabStopWidth( stop_width );

    // define emulated tabs
    emulatedTab_ = QString( tabSize, ' ' );
    emulatedTabRegexp_.setPattern( QString( "^(%1)+" ).arg( emulatedTab_ ) );

    // update tab string according to tab emulation state
    if( _hasTabEmulation() ) tab_ = emulatedTab_;
    return true;
}

//_____________________________________________________________
void TextEditor::_insertTab( void )
{
    Debug::Throw( "TextEditor::_insertTab.\n" );

    // retrieve current cursor
    QTextCursor cursor( textCursor() );
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
bool TextEditor::_updateMargin( void )
{

    Debug::Throw( "TextEditor::_updateMargin.\n" );
    int left_margin( 0 );

    if( showLineNumberAction().isChecked() && showLineNumberAction().isVisible() )
    { left_margin += lineNumberDisplay_->width(); }

    return _setLeftMargin( left_margin );
    if( leftMargin_ == left_margin ) return false;
    return true;

}

//________________________________________________
void TextEditor::_updateConfiguration( void )
{

    Debug::Throw( "TextEditor::_updateConfiguration.\n" );

    // wrap mode
    if( wrapFromOptions() )
    { wrapModeAction().setChecked( XmlOptions::get().get<bool>( "WRAP_TEXT" ) ); }

    if( lineNumbersFromOptions() )
    { showLineNumberAction().setChecked( XmlOptions::get().get<bool>( "SHOW_LINE_NUMBERS" ) ); }

    // tab emulation
    _setTabSize( XmlOptions::get().get<int>("TAB_SIZE") );
    tabEmulationAction().setChecked( XmlOptions::get().get<bool>( "TAB_EMULATION" ) );

    // paragraph highlighting
    if( highlightBlockFromOptions_ )
    {
        blockHighlight_->setEnabled( XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );
        blockHighlightAction_->setEnabled( true );
        blockHighlightAction_->setChecked( XmlOptions::get().get<bool>( "HIGHLIGHT_PARAGRAPH" ) );
    }

    // update margins
    lineNumberDisplay_->updateWidth( document()->blockCount() );
    _updateMargin();
    marginWidget_->setDirty();

    // update box configuration
    // clear
    boxSelection_.updateConfiguration();
    if( !boxSelection_.isEnabled() && boxSelection_.state() != BoxSelection::SelectionEmpty )
    {
        boxSelection_.clear();
        _synchronizeBoxSelection();
        emit copyAvailable( false );
    }

    // cursor monitor
    cursorMonitor_.setEnabled( XmlOptions::get().get<int>( "AUTOHIDE_CURSOR_DELAY" ) > 0 );
    cursorMonitor_.setAutoHideDelay( XmlOptions::get().get<int>( "AUTOHIDE_CURSOR_DELAY" ) * 1000 );

    return;

}

//________________________________________________
void TextEditor::_synchronizeSelection( void )
{

    //Debug::Throw( "TextEditor::_synchronizeSelection.\n" );
    if( !isSynchronized() ) return;

    for( const auto& editor:Base::KeySet<TextEditor>(this) )
    {

        // check if textCursor is different
        if( editor->textCursor().position() == textCursor().position() &&
            editor->textCursor().anchor() == textCursor().anchor() )
            continue;

        // store scrollbar positions
        QPoint scrollbars( editor->scrollbarPosition() );

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
void TextEditor::_updateContentActions( void )
{

    Debug::Throw( "TextEditor::_updateContentActions.\n" );

    // actions
    clearAction_->setEnabled( !(document()->isEmpty() || isReadOnly() ) );
    selectAllAction_->setEnabled( !document()->isEmpty() );

}

//________________________________________________
void TextEditor::_updateReadOnlyActions( bool readOnly )
{

    Debug::Throw( "TextEditor::_updateReadOnlyActions.\n" );
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
void TextEditor::_updateUndoRedoActions( void )
{

    Debug::Throw( "TextEditor::_updateUndoRedoActions.\n" );
    undoAction_->setEnabled( document()->isUndoAvailable() && !isReadOnly() );
    redoAction_->setEnabled( document()->isRedoAvailable() && !isReadOnly() );

}

//________________________________________________
void TextEditor::_updateSelectionActions( bool hasSelection )
{

    Debug::Throw() << "TextEditor::_updateSelectionActions - hasSelection: " << hasSelection << endl;

    bool editable( !isReadOnly() );
    cutAction_->setEnabled( hasSelection && editable );
    copyAction_->setEnabled( hasSelection );
    upperCaseAction_->setEnabled( hasSelection && editable );
    lowerCaseAction_->setEnabled( hasSelection && editable );

    #if QT_VERSION < 0x040200
    // update clipboard actions, based on the clipboard content
    // this is done only for QT versions < 4.2. For higher versions
    // this is called directly from a QClipboard signal
    _updateClipboardActions( QClipboard::Clipboard );
    _updateClipboardActions( QClipboard::Selection );
    #endif
}

//________________________________________________
void TextEditor::_updateClipboardActions( QClipboard::Mode mode )
{
    Debug::Throw( "TextEditor::_updateClipboardActions.\n" );
    if( mode == QClipboard::Clipboard )
    {

        const bool hasClipboard( !qApp->clipboard()->text( QClipboard::Clipboard ).isEmpty() );
        pasteAction_->setEnabled( hasClipboard && !isReadOnly() );

    } else if( mode == QClipboard::Selection ) {

        const bool hasSelection( !qApp->clipboard()->text( QClipboard::Selection ).isEmpty() );
        findSelectionAction_->setEnabled( hasSelection );
        findSelectionBackwardAction_->setEnabled( hasSelection );

    }

}

//________________________________________________
void TextEditor::_updateClipboard( void )
{
    Debug::Throw( "TextEditor::_updateClipboard.\n" );

    // copy selected text to clipboard
    if( qApp->clipboard()->supportsSelection() && textCursor().hasSelection() )
    { qApp->clipboard()->setMimeData( createMimeDataFromSelection(), QClipboard::Selection ); }

}

//_____________________________________________
void TextEditor::_copyLinkLocation( void )
{
    Debug::Throw( "TextEditor::_copyLinkLocation.\n" );
    if( !trackAnchors_ ) return;

    const QString anchor( this->anchor() );
    if( anchor.isEmpty() ) return;

    // copy selected text to clipboard
    qApp->clipboard()->setText( anchor, QClipboard::Clipboard );
    if( qApp->clipboard()->supportsSelection() ) qApp->clipboard()->setText( anchor, QClipboard::Selection );
    if( qApp->clipboard()->supportsFindBuffer() ) qApp->clipboard()->setText( anchor, QClipboard::FindBuffer );
}

//________________________________________________
void TextEditor::_updatePasteAction( void )
{

    Debug::Throw( "TextEditor::_updatePasteAction.\n" );
    const bool hasClipboard( !qApp->clipboard()->text().isEmpty() );
    pasteAction_->setEnabled( hasClipboard && !isReadOnly() );

}

//_________________________________________________
void TextEditor::_toggleBlockHighlight( bool state )
{

    Debug::Throw( "TextEditor::_toggleBlockHighlight.\n" );

    // enable
    blockHighlight_->setEnabled( state );

    // update options
    XmlOptions::get().set<bool>( "HIGHLIGHT_PARAGRAPH", state );

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
    Debug::Throw() << "TextEditor::_toggleWrapMode - " << (state ? "True":"false") << endl;
    LineWrapMode mode( state ? BaseEditor::WidgetWidth : BaseEditor::NoWrap );
    if( mode == lineWrapMode() ) return false;

    setLineWrapMode( mode );
    if( _setModifier( ModifierWrap, state ) ) emit modifiersChanged( modifiers() );

    // propagate to associated display
    if( isSynchronized() )
    {

        // temporarely disable synchronization
        // to avoid infinite loop
        setSynchronized( false );
        for( const auto& editor:Base::KeySet<TextEditor>( this ) )
        { if( editor->isSynchronized() ) editor->wrapModeAction().setChecked( state ); }
        setSynchronized( true );

    }

    return true;

}

//________________________________________________
bool TextEditor::_toggleTabEmulation( bool state )
{

    Debug::Throw() << "TextEditor::_toggleTabEmulation - " << (state ? "True":"false") << endl;

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
        { if( editor->isSynchronized() ) editor->tabEmulationAction().setChecked( state ); }
        setSynchronized( true );

    }

    return true;

}

//_______________________________________________________
void TextEditor::_toggleShowLineNumbers( bool state )
{

    _updateMargin();

    // update options
    XmlOptions::get().set<bool>( "SHOW_LINE_NUMBERS", state );

    // propagate to other displays
    if( isSynchronized() )
    {
        // temporarely disable synchronization
        // to avoid infinite loop
        setSynchronized( false );

        for( const auto& editor:Base::KeySet<TextEditor>( this ) )
        { if( editor->isSynchronized() ) editor->showLineNumberAction().setChecked( state ); }
        setSynchronized( true );

    }

    return;
}

//________________________________________________________
void TextEditor::_blockCountChanged( int count )
{

    Debug::Throw( "TextEditor::_blockCountChanged.\n" );

    // margins
    if( !( _hasLineNumberDisplay() && lineNumberDisplay_->updateWidth( count ) ) ) return;
    if( !( hasLineNumberAction() && showLineNumberAction().isChecked() && showLineNumberAction().isVisible() ) ) return;
    _updateMargin();
    update();

}

//_____________________________________________________________________
void TextEditor::_findFromDialog( void )
{
    Debug::Throw( "TextEditor::_findFromDialog.\n" );

    // set default text
    // update find text
    QString text( selection().text() );
    if( !text.isEmpty() )
    {
        const int maxLength( 1024 );
        text = text.left( maxLength );
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
void TextEditor::_replaceFromDialog( void )
{
    Debug::Throw( "TextEditor::_replaceFromDialog.\n" );

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
void TextEditor::_updateReplaceInSelection( void )
{ if( replaceWidget_ ) replaceWidget_->enableReplaceInSelection( hasSelection() ); }

//________________________________________________
void TextEditor::_selectLineFromDialog( void )
{

    Debug::Throw( "TextEditor::_selectLineFromDialog.\n" );

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
    QWidget( parent ),
    Counter( "TextEditor::Container" ),
    editor_( new TextEditor() )
{ _initialize(); }

//_________________________________________________________
TextEditor::Container::Container( QWidget* parent, TextEditor* editor ):
    QWidget( parent ),
    Counter( "TextEditor::Container" ),
    editor_( editor )
{ _initialize(); }

//_________________________________________________________
void TextEditor::Container::_initialize( void )
{
    Debug::Throw( "TextEditor::Container::_initialize.\n" );
    editor_->setParent( this );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(0);
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
    connect( &editor_->findWidget_->closeButton(), SIGNAL(clicked()), editor_, SLOT(setFocus()) );
    connect( &editor_->replaceWidget_->closeButton(), SIGNAL(clicked()), editor_, SLOT(setFocus()) );
    connect( &editor_->selectLineWidget_->closeButton(), SIGNAL(clicked()), editor_, SLOT(setFocus()) );

}
