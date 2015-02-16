// $Id$

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

#include "LineEditor.h"
#include "LineEditor_p.h"

#include "BaseContextMenu.h"
#include "BaseIconNames.h"
#include "Debug.h"
#include "IconEngine.h"

#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QStyleOptionFrame>

#include <QEvent>

namespace Private
{

    //____________________________________________________________
    LineEditorStyle::LineEditorStyle( QStyle* parent ):
        QProxyStyle( parent ),
        Counter( "Private::LineEditorStyle" )
    {}

    //____________________________________________________________
    QRect LineEditorStyle::subElementRect( SubElement subElement, const QStyleOption* option, const QWidget* widget ) const
    {

        // check subelement
        if( subElement != SE_LineEditContents ) return QProxyStyle::subElementRect( subElement, option, widget );

        // check editor
        const LineEditor* editor( qobject_cast<const LineEditor*>( widget ) );
        if( !( editor && editor->hasClearButton() ) )
        { return QProxyStyle::subElementRect( subElement, option, widget ); }

        // adjust sub element rect
        QRect rect( QProxyStyle::subElementRect( subElement, option, widget ) );
        return visualRect( option->direction, rect, rect.adjusted( 0, 0, -editor->clearButtonWidth(), 0 ) );

    }

    //____________________________________________________________
    QSize LineEditorStyle::sizeFromContents( ContentsType contentsType, const QStyleOption* option, const QSize& contentsSize, const QWidget* widget ) const
    {

        if( contentsType != CT_LineEdit ) return QProxyStyle::sizeFromContents( contentsType, option, contentsSize, widget );

        // check editor
        const LineEditor* editor( qobject_cast<const LineEditor*>( widget ) );
        if( !( editor && editor->hasClearButton() ) )
        { return QProxyStyle::sizeFromContents( contentsType, option, contentsSize, widget ); }

        const int buttonWidth( editor->clearButtonWidth() );

        QSize size( contentsSize );
        size.rwidth() += buttonWidth;
        size.setHeight( qMax( size.height(), buttonWidth ) );

        return QProxyStyle::sizeFromContents( contentsType, option, contentsSize, widget );

    }

    //____________________________________________________________
    LineEditorButton::LineEditorButton( QWidget* parent ):
        QAbstractButton( parent ),
        Counter( "Private::LineEditorButton" )
    {
        setText( QString() );
        setIcon( IconEngine::get( IconNames::EditClear ) );
        setAutoFillBackground( false );
        setToolTip( tr( "Clear text" ) );
        setCursor( Qt::ArrowCursor );
        setFocusPolicy( Qt::NoFocus );
    }

    //____________________________________________________________
    void LineEditorButton::paintEvent( QPaintEvent* event )
    {
        if( icon().isNull() ) return;

        QStyleOptionButton option;
        option.initFrom( this );

        const int iconWidth( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
        const QSize iconSize( iconWidth, iconWidth );
        const QPixmap pixmap( icon().pixmap( iconSize ) );

        const QRect rect( this->rect() );
        const QRect iconRect( QPoint( rect.x() + (rect.width() - iconWidth)/2, rect.y() + (rect.height() - iconWidth)/2 ), iconSize );

        QPainter painter( this );
        painter.setClipRegion( event->region() );
        painter.drawPixmap( iconRect, pixmap );
    }

}

//____________________________________________________________
LineEditor::LineEditor( QWidget* parent ):
    QLineEdit( parent ),
    Counter( "LineEditor" ),
    proxyStyle_( new Private::LineEditorStyle() )
{

    Debug::Throw( "LineEditor::LineEditor.\n" );

    // actions
    _installActions();

    // modification state call-back
    connect( this, SIGNAL(textChanged(QString)), SLOT(_modified(QString)) );

    setHasClearButton( true );
    setStyle( proxyStyle_.data() );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

}

//_____________________________________________________________________
int LineEditor::clearButtonWidth( void ) const
{
    // adjust contents size
    QStyleOptionFrame option;
    option.initFrom( this );
    initStyleOption( &option );
    const int iconSize( style()->pixelMetric( QStyle::PM_SmallIconSize, &option, this ) );
    const int margin( 2 );
    return iconSize + 2*margin;
}

//_____________________________________________________________________
void LineEditor::setReadOnly( bool value )
{

    if( value )
    {

        if( clearButton_ ) clearButton_->hide();

    } else if( clearButton_ && !clearButton_->isVisible() && !text().isEmpty() ) {

        clearButton_->show();
        _updateClearButton();

    }

    QLineEdit::setReadOnly( value );
    return;

}

//_____________________________________________________________________
void LineEditor::setModified( const bool& value )
{
    Debug::Throw( "LineEditor::setModified.\n" );
    if( value != modified_ )
    {
        modified_ = value;
        if( !value ) backup_ = text();
        emit modificationChanged( value );
    }
}

//______________________________________________________________
void LineEditor::setHasClearButton( const bool& value )
{
    if( value )
    {

        if( !clearButton_ )
        {
            clearButton_ = new Private::LineEditorButton( this );
            connect( clearButton_, SIGNAL(clicked()), SLOT(clear()) );
            clearButton_->hide();
        }

        if( !text().isEmpty() && !clearButton_->isVisible() )
        {
            clearButton_->show();
            _updateClearButton();
        }

    } else if( clearButton_ ) {

        clearButton_->hide();

    }
}

//______________________________________________________________________________
void LineEditor::installContextMenuActions( BaseContextMenu* menu )
{
    Debug::Throw( "TextEditor::installContextMenuActions.\n" );
    menu->addAction( undoAction_ );
    menu->addAction( redoAction_ );
    menu->addSeparator();

    menu->addAction( cutAction_ );
    menu->addAction( copyAction_ );
    menu->addAction( pasteAction_ );
    menu->addAction( clearAction_ );
    menu->addSeparator();

    menu->addAction( selectAllAction_ );
    menu->addAction( upperCaseAction_ );
    menu->addAction( lowerCaseAction_ );
}

//_____________________________________________________________________
void LineEditor::lowerCase( void )
{

    // do nothing if object is read only
    if( isReadOnly() ) return;

    // do nothing if selection is not valid
    if( !hasSelectedText() ) return;

    // do nothing if selection is not valid
    QString selection = selectedText().toLower();
    cut();
    insert( selection );
}

//_____________________________________________________________________
void LineEditor::upperCase( void )
{

    // do nothing if object is read only
    if( isReadOnly() ) return;

    // do nothing if selection is not valid
    if( !hasSelectedText() ) return;

    // get uppercased selection
    QString selection = selectedText().toUpper();
    cut();
    insert( selection );
}

//_______________________________________________________
bool LineEditor::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::StyleChange:
        {

            QStyle* currentStyle( style() );

            // do nothing is current style already match or is a style sheet
            if( qobject_cast<Private::LineEditorStyle*>( currentStyle ) ||
                currentStyle->inherits( "QStyleSheetStyle" ) )
                { break; }

            // delete old style
            if( !proxyStyle_ ) proxyStyle_ = new Private::LineEditorStyle( currentStyle );
            else proxyStyle_.data()->setBaseStyle( currentStyle );

            // assign
            setStyle( proxyStyle_.data() );
            break;
        }

        case QEvent::Show:
        case QEvent::Resize:
        {
            if( clearButton_ )
            { _updateClearButton(); }
            break;
        }

        default: break;
    }

    return QLineEdit::event( event );

}

//_______________________________________________________________
void LineEditor::contextMenuEvent(QContextMenuEvent *event)
{
    Debug::Throw( "TextEditor::contextMenuEvent.\n" );

    // menu
    BaseContextMenu menu( this );
    menu.setHideDisabledActions( true );
    installContextMenuActions( &menu );
    menu.exec( event->globalPos() );
}

//_____________________________________________
void LineEditor::keyPressEvent( QKeyEvent* event )
{

    // process base class function
    QLineEdit::keyPressEvent( event );

    // emit signal
    emit cursorPositionChanged( cursorPosition() );

}

//____________________________________________________________
void LineEditor::_updateClearButton( void ) const
{
    if( !clearButton_ ) return;

    QStyleOptionFrame option;
    option.initFrom( this );
    initStyleOption( &option );

    const int buttonWidth( clearButtonWidth() );

    const QRect textRect( style()->subElementRect( QStyle::SE_LineEditContents, &option, this ) );
    QRect buttonRect( textRect.topRight()+QPoint( 1, 0 ), QSize( buttonWidth, textRect.height() ) );

    // handle right to left
    buttonRect = style()->visualRect( option.direction, option.rect, buttonRect );
    clearButton_->setGeometry( buttonRect );

}

//____________________________________________________________
void LineEditor::_modified( const QString& text )
{
    // modification signal
    bool modified( text != backup_ );
    if( modified != modified_ )
    {
        modified_ = modified;
        emit modificationChanged( modified_ );
    }

    // clear actions enability
    clearAction_->setEnabled( !(isReadOnly() || text.isEmpty() ) );
    if( clearButton_ )
    {
        if( text.isEmpty() ) clearButton_->hide();
        else if( !clearButton_->isVisible() )
        {

            clearButton_->show();
            _updateClearButton();
        }
    }

    // emit cleared signal
    if( text.isEmpty() ) emit cleared();

}

//____________________________________________________________
void LineEditor::paintEvent( QPaintEvent* event )
{
    // base class method
    QLineEdit::paintEvent( event );

    #if QT_VERSION < 0x050000
    if( text().isEmpty() && !placeholderText().isEmpty() && hasFocus() )
    {

        // get text rect
        QStyleOptionFrameV2 frameOption;
        initStyleOption(&frameOption);
        QRect textRect( style()->subElementRect(QStyle::SE_LineEditContents, &frameOption, this) );

        // alignment
        const Qt::Alignment alignment( QStyle::visualAlignment( frameOption.direction, this->alignment() ) );

        // font metrics
        const QFontMetrics fontMetrics( this->fontMetrics() );

        // adjust text rect
        const int minBearing = qMax(0, -fontMetrics.minLeftBearing());
        const int horizontalOffset = 2;
        textRect.adjust( horizontalOffset + minBearing, 0, 0, 0 );

        // text
        const QString elidedText = fontMetrics.elidedText( placeholderText(), Qt::ElideRight, textRect.width() );

        // color
        QColor textColor( palette().text().color() );
        textColor.setAlpha(128);

        QPainter painter( this );
        painter.setPen( textColor );
        painter.drawText( textRect, alignment, elidedText );

    }
    #endif

}

//__________________________________________________________
void LineEditor::_installActions( void )
{
    Debug::Throw( "LineEditor::_installActions.\n" );

    // create actions
    addAction( undoAction_ = new QAction( IconEngine::get( IconNames::Undo ), tr( "Undo" ), this ) );
    undoAction_->setShortcut( QKeySequence::Undo );
    undoAction_->setShortcutContext( Qt::WidgetShortcut );
    undoAction_->setEnabled( isUndoAvailable() );
    connect( undoAction_, SIGNAL(triggered()), SLOT(undo()) );

    addAction( redoAction_ = new QAction( IconEngine::get( IconNames::Redo ), tr( "Redo" ), this ) );
    redoAction_->setShortcut( QKeySequence::Redo );
    redoAction_->setEnabled( isRedoAvailable() );
    redoAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( redoAction_, SIGNAL(triggered()), SLOT(redo()) );

    addAction( cutAction_ = new QAction( IconEngine::get( IconNames::Cut ), tr( "Cut" ), this ) );
    cutAction_->setShortcut( QKeySequence::Cut );
    cutAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( cutAction_, SIGNAL(triggered()), SLOT(cut()) );

    addAction( copyAction_ = new QAction( IconEngine::get( IconNames::Copy ), tr( "Copy" ), this ) );
    copyAction_->setShortcut( QKeySequence::Copy );
    copyAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( copyAction_, SIGNAL(triggered()), SLOT(copy()) );

    addAction( pasteAction_ = new QAction( IconEngine::get( IconNames::Paste ), tr( "Paste" ), this ) );
    pasteAction_->setShortcut( QKeySequence::Paste );
    pasteAction_->setShortcutContext( Qt::WidgetShortcut );
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
    upperCaseAction_->setShortcut( Qt::CTRL+Qt::Key_U );
    connect( upperCaseAction_, SIGNAL(triggered()), SLOT(upperCase()) );

    addAction( lowerCaseAction_ = new QAction( tr( "Lower Case" ), this ) );
    lowerCaseAction_->setShortcut( Qt::SHIFT+Qt::CTRL+Qt::Key_U );
    connect( lowerCaseAction_, SIGNAL(triggered()), SLOT(lowerCase()) );

    // update actions that depend on the presence of a selection
    connect( this, SIGNAL(textChanged(QString)), SLOT(_updateUndoRedoActions()) );
    connect( this, SIGNAL(selectionChanged()), SLOT(_updateSelectionActions()) );
    _updateUndoRedoActions();
    _updateSelectionActions();

}

//________________________________________________
void LineEditor::_updateUndoRedoActions( void )
{
    Debug::Throw( "LineEditor::_updateUndoRedoActions.\n" );
    undoAction_->setEnabled( isUndoAvailable() );
    redoAction_->setEnabled( isRedoAvailable() );
}

//________________________________________________
void LineEditor::_updateSelectionActions()
{

    Debug::Throw( "LineEditor::_updateSelectionActions.\n" );

    bool hasSelection( hasSelectedText() );
    bool editable( !isReadOnly() );

    cutAction_->setEnabled( hasSelection && editable );
    copyAction_->setEnabled( hasSelection );
    upperCaseAction_->setEnabled( hasSelection && editable );
    lowerCaseAction_->setEnabled( hasSelection && editable );

}

//________________________________________________
void LineEditor::_updatePasteAction( void )
{

    Debug::Throw( "LineEditor::_updatePasteAction.\n" );
    bool editable( !isReadOnly() );
    bool has_clipboard( !qApp->clipboard()->text().isEmpty() );
    pasteAction_->setEnabled( editable && has_clipboard );

}
