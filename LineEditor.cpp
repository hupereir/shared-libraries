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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
\file LineEditor.cpp
\brief customized QLineEdit object
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "LineEditor.h"
#include "BaseIcons.h"
#include "Debug.h"
#include "IconEngine.h"
#include "Str.h"

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionFrameV2>
#include <QtGui/QToolTip>

#include <QtCore/QEvent>

//____________________________________________________________
LineEditor::LineEditor( QWidget* parent ):
    QLineEdit( parent ),
    Counter( "LineEditor" ),
    modified_( false ),
    hasClearButton_( false ),
    clearButtonVisible_( false ),
    hasFrame_( true ),
    triggered_( false ),
    clearIcon_( IconEngine::get( ICONS::EDIT_CLEAR ) )
{

    Debug::Throw( "LineEditor::LineEditor.\n" );

    // actions
    _installActions();

    // modification state call-back
    connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( _modified( const QString& ) ) );

    // set clear button visible
    setHasClearButton( true );
    setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

}

//_____________________________________________________________________
void LineEditor::setReadOnly( bool value )
{

    if( value == isReadOnly() ) return;
    QLineEdit::setReadOnly( value );

    if( isReadOnly() )
    {

        // reset frame
        QLineEdit::setFrame( hasFrame_ );

        // reset contents margins
        setContentsMargins( 0, 0, 0, 0 );

    } else if( _hasClearButton() ) {

        // set frame flag from base class
        setFrame( QLineEdit::hasFrame() );

        // disable QLineEdit frame
        QLineEdit::setFrame( false );

        // reset contents margins
        int offset( hasFrame() ? _frameWidth():0 );
        setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing() + 1, offset );

    }

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

    Debug::Throw() << "LineEditor::setHasClearButton - value: " << value << endl;

    if( value == hasClearButton_ ) return;
    hasClearButton_ = value;

    if( _hasClearButton() )
    {

        // set frame flag from base class
        setFrame( QLineEdit::hasFrame() );

        // disable QLineEdit frame
        QLineEdit::setFrame( false );

        // reset contents margins
        int offset( hasFrame() ? _frameWidth():0 );
        setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing() + 3, offset );

    } else {

        // reset frame
        QLineEdit::setFrame( hasFrame_ );

        // reset contents margins
        setContentsMargins( 0, 0, 0, 0 );

    }

    update();

}

//______________________________________________________________
void LineEditor::setFrame( const bool& value )
{

    Debug::Throw() << "LineEditor::setFrame - value: " << value << endl;

    // do nothing if value is unchanged
    if( value == hasFrame() ) return;

    hasFrame_ = value;
    if( !_hasClearButton() ) QLineEdit::setFrame( value );
    else {

        // reset contents margins
        int offset( hasFrame() ? _frameWidth():0 );
        setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing() + 1, offset );

    }

}

//_____________________________________________________________________
void LineEditor::lowerCase( void )
{
    Debug::Throw( "LineEditor::lowerCase.\n" );

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
    Debug::Throw( "LineEditor::upperCase.\n" );

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

    // check that all needed widgets/actions are valid and checked.
    switch (event->type())
    {

        case QEvent::ToolTip:
        {

            // check if button is available
            if( isReadOnly() || ( !_hasClearButton() ) || text().isEmpty() ) break;

            // cast
            QHelpEvent *help_event = static_cast<QHelpEvent*>(event);
            if( contentsRect().contains( help_event->pos() ) ) break;

            // set appropriate tooltip
            QToolTip::showText( help_event->globalPos(), "Clear text" );
            return true;

        }

        break;

        default: break;
    }

    return QLineEdit::event( event );

}

//_______________________________________________________________
void LineEditor::contextMenuEvent(QContextMenuEvent *event)
{

    // menu
    QMenu menu( this );
    menu.addAction( undoAction_ );
    menu.addAction( redoAction_ );
    menu.addSeparator();

    menu.addAction( cutAction_ );
    menu.addAction( copyAction_ );
    menu.addAction( pasteAction_ );
    menu.addAction( clearAction_ );
    menu.addSeparator();

    menu.addAction( selectAllAction_ );
    menu.addAction( upperCaseAction_ );
    menu.addAction( lowerCaseAction_ );

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


//________________________________________________
void LineEditor::mouseMoveEvent( QMouseEvent* event )
{

    // check clear button
    if( !_hasClearButton() ) return QLineEdit::mouseMoveEvent( event );

    // check event position vs button location
    if( !_clearButtonRect().contains( event->pos() ) || text().isEmpty() )
    {

        // make sure cursor is properly set
        if( cursor().shape() != Qt::IBeamCursor ) setCursor( Qt::IBeamCursor );
        return QLineEdit::mouseMoveEvent( event );

    } else if( cursor().shape() == Qt::IBeamCursor ) unsetCursor();

}

//________________________________________________
void LineEditor::mousePressEvent( QMouseEvent* event )
{

    Debug::Throw( "LineEditor::mousePressEvent.\n" );

    // check clear button
    if( !_hasClearButton() ) return QLineEdit::mousePressEvent( event );

    // check if within clear button (if any)
    if( !( isReadOnly() || text().isEmpty() ) && _clearButtonRect().contains( event->pos() ) )   triggered_ = true;
    else QLineEdit::mousePressEvent( event );

    return;

}

//_____________________________________________
void LineEditor::mouseReleaseEvent( QMouseEvent* event )
{

    Debug::Throw( "LineEditor::mouseReleaseEvent.\n" );
    if( !( isReadOnly() || text().isEmpty() ) && _hasClearButton() && _clearButtonRect().contains( event->pos() ) && triggered_ )
    {

        clear();
        emit cleared();

    } else {

        QLineEdit::mouseReleaseEvent( event );
        emit cursorPositionChanged( cursorPosition( ) );

    }

    triggered_ = false;
    return;

}

//________________________________________________
void LineEditor::paintEvent( QPaintEvent* event )
{

    // check clear button
    if( isReadOnly() || !_hasClearButton() ) return QLineEdit::paintEvent( event );

    // paint the button at the correct place
    _toggleClearButton( !(isReadOnly() || text().isNull() || text().isEmpty() ) );

    // initialize option
    QStyleOptionFrameV2 panel;
    panel.initFrom( this );
    panel.rect = LineEditor::rect();
    panel.lineWidth = (hasFrame()) ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth):0;
    panel.state |= QStyle::State_Sunken;
    if( hasFocus() ) panel.state |= QStyle::State_HasFocus;

    // draw white background
    {
        QPainter painter( this );
        painter.setClipRect( event->rect() );
        style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &painter, this);
        painter.end();
    }

    // normal painting (without frame)
    QLineEdit::paintEvent( event );

    // draw clear button
    {
        QPainter painter( this );
        _paintClearButton( painter );
        painter.end();
    }

}

//____________________________________________________________
void LineEditor::_modified( const QString& text )
{
    Debug::Throw( "LineEditor::_modified.\n" );

    // modification signal
    bool modified( text != backup_ );
    if( modified != modified_ )
    {
        modified_ = modified;
        emit modificationChanged( modified_ );
    }

    // clear actiosn enability
    if( modified ) clearAction_->setEnabled( !(isReadOnly() || text.isEmpty() ) );

}

//__________________________________________________________
void LineEditor::_installActions( void )
{
    Debug::Throw( "LineEditor::_installActions.\n" );

    // create actions
    addAction( undoAction_ = new QAction( IconEngine::get( ICONS::UNDO ), "Undo", this ) );
    undoAction_->setShortcut( QKeySequence::Undo );
    undoAction_->setShortcutContext( Qt::WidgetShortcut );
    undoAction_->setEnabled( isUndoAvailable() );
    connect( undoAction_, SIGNAL( triggered() ), SLOT( undo() ) );

    addAction( redoAction_ = new QAction( IconEngine::get( ICONS::REDO ), "Redo", this ) );
    redoAction_->setShortcut( QKeySequence::Redo );
    redoAction_->setEnabled( isRedoAvailable() );
    redoAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( redoAction_, SIGNAL( triggered() ), SLOT( redo() ) );

    addAction( cutAction_ = new QAction( IconEngine::get( ICONS::CUT ), "Cut", this ) );
    cutAction_->setShortcut( QKeySequence::Cut );
    cutAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( cutAction_, SIGNAL( triggered() ), SLOT( cut() ) );

    addAction( copyAction_ = new QAction( IconEngine::get( ICONS::COPY ), "Copy", this ) );
    copyAction_->setShortcut( QKeySequence::Copy );
    copyAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( copyAction_, SIGNAL( triggered() ), SLOT( copy() ) );

    addAction( pasteAction_ = new QAction( IconEngine::get( ICONS::PASTE ), "Paste", this ) );
    pasteAction_->setShortcut( QKeySequence::Paste );
    pasteAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( pasteAction_, SIGNAL( triggered() ), SLOT( paste() ) );
    connect( qApp->clipboard(), SIGNAL( dataChanged() ), SLOT( _updatePasteAction() ) );
    _updatePasteAction();

    addAction( clearAction_ = new QAction( "Clear", this ) );
    connect( clearAction_, SIGNAL( triggered() ), SLOT( clear() ) );

    addAction( selectAllAction_ = new QAction( "Select All", this ) );
    selectAllAction_->setShortcut( QKeySequence::SelectAll );
    selectAllAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( selectAllAction_, SIGNAL( triggered() ), SLOT( selectAll() ) );

    addAction( upperCaseAction_ = new QAction( "Upper Case", this ) );
    upperCaseAction_->setShortcut( Qt::CTRL+Qt::Key_U );
    upperCaseAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( upperCaseAction_, SIGNAL( triggered() ), SLOT( upperCase() ) );

    addAction( lowerCaseAction_ = new QAction( "Lower Case", this ) );
    lowerCaseAction_->setShortcut( Qt::SHIFT+Qt::CTRL+Qt::Key_U );
    lowerCaseAction_->setShortcutContext( Qt::WidgetShortcut );
    connect( lowerCaseAction_, SIGNAL( triggered() ), SLOT( lowerCase() ) );

    // update actions that depend on the presence of a selection
    connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( _updateUndoRedoActions() ) );
    connect( this, SIGNAL( selectionChanged() ), SLOT( _updateSelectionActions() ) );
    _updateUndoRedoActions();
    _updateSelectionActions();

}

//________________________________________________
bool LineEditor::_toggleClearButton( const bool& value )
{
    if( value == _clearButtonVisible() ) return false;
    _setClearButtonVisible( value );
    return true;
}

//________________________________________________
void LineEditor::_paintClearButton( QPainter& painter, const bool& check )
{

    if( check && !_clearButtonVisible() ) return;

    // get widget rect an adjust
    QRect rect( LineEditor::rect() );
    if( hasFrame() ) rect.adjust( 0, _frameWidth(), -_frameWidth()-1, -_frameWidth() );

    // set the proper right margin, so that button rect is a square
    rect.setLeft( rect.right() - rect.height() );

    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    _clearIcon().paint(
        &painter, rect,
        Qt::AlignRight|Qt::AlignVCenter,
        isEnabled() ? QIcon::Normal : QIcon::Disabled );

    _setClearButtonRect( rect );

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

//______________________________________________________________
int LineEditor::_frameWidth( void ) const
{ return style()->pixelMetric( QStyle::PM_DefaultFrameWidth, 0, this ); }
