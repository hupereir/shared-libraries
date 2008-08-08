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

#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrameV2>
#include <QToolTip>

#include "BaseIcons.h"
#include "LineEditor.h"
#include "Debug.h"
#include "IconEngine.h"
#include "Str.h"

using namespace std;
using namespace Qt;

//____________________________________________________________
LineEditor::LineEditor( QWidget* parent ):
  QLineEdit( parent ),
  Counter( "LineEditor" ),
  modified_( false ),
  has_clear_button_( false ),
  has_frame_( true ),
  clear_icon_( IconEngine::get( ICONS::EDIT_CLEAR ) )
{    
  Debug::Throw( "LineEditor::LineEditor.\n" );
    
  // actions
  _installActions();
  
  // modification state call-back
  connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( _modified( const QString& ) ) );
    
  // set clear button visible
  setHasClearButton( true );
  
}

//_____________________________________________________________________
void LineEditor::setReadOnly( bool value )
{ 
  
  if( value == isReadOnly() ) return;
  QLineEdit::setReadOnly( value );
  
  if( isReadOnly() )
  {
    
    // reset frame
    QLineEdit::setFrame( has_frame_ );
    
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
  
  if( value == has_clear_button_ ) return;
  has_clear_button_ = value;
  
  if( _hasClearButton() )
  {

    // set frame flag from base class
    setFrame( QLineEdit::hasFrame() );
    
    // disable QLineEdit frame
    QLineEdit::setFrame( false );

    // reset contents margins
    int offset( hasFrame() ? _frameWidth():0 );
    setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing() + 1, offset );

  } else {
    
    // reset frame
    QLineEdit::setFrame( has_frame_ );
    
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
  
  has_frame_ = value;
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
  Str selection = Str( qPrintable( selectedText() ) ).lowerCase();
  
  // remove current selection
  cut();
  insert( selection.c_str() );
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
  Str selection = Str( qPrintable( selectedText() ) ).upperCase();
  
  // remove current selection
  cut();
  insert( selection.c_str() );
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
  menu.addAction( undo_action_ );
  menu.addAction( redo_action_ );
  menu.addSeparator();

  menu.addAction( cut_action_ );
  menu.addAction( copy_action_ );
  menu.addAction( paste_action_ );
  menu.addAction( clear_action_ );
  menu.addSeparator();
 
  menu.addAction( select_all_action_ ); 
  menu.addAction( upper_case_action_ );
  menu.addAction( lower_case_action_ );
  
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

  // check if outside of clear button (if any)
  if( isReadOnly() || text().isEmpty() || !_clearButtonRect().contains( event->pos() ) ) 
  { return QLineEdit::mousePressEvent( event ); }

  return;
  
}

//_____________________________________________
void LineEditor::mouseReleaseEvent( QMouseEvent* event )
{
  
  Debug::Throw( "LineEditor::mouseReleaseEvent.\n" );
  if( isReadOnly() || text().isEmpty() || !(_hasClearButton() && _clearButtonRect().contains( event->pos() ) ) ) 
  { 
    
    QLineEdit::mouseReleaseEvent( event );
    emit cursorPositionChanged( cursorPosition( ) );
    
  } else {
    
    clear();
    emit cleared();
    
  }
  
}

//________________________________________________
void LineEditor::paintEvent( QPaintEvent* event )
{
  
  // check clear button
  if( isReadOnly() || !_hasClearButton() ) return QLineEdit::paintEvent( event );

  // initialize option
  QStyleOptionFrameV2 panel;
  panel.initFrom( this );
  panel.rect = LineEditor::rect();
  panel.lineWidth = (hasFrame()) ? style()->pixelMetric(QStyle::PM_DefaultFrameWidth):0;
  panel.state |= QStyle::State_Sunken;
  if( hasFocus() ) panel.state |= QStyle::State_HasFocus;
  
  // draw white background
  QPainter painter( this );
  style()->drawPrimitive(QStyle::PE_PanelLineEdit, &panel, &painter, this);
  
  // paint the button at the correct place
  if( !(isReadOnly() || text().isNull() || text().isEmpty() ) )
  {
    
    // get widget rect an adjust
    QRect rect( LineEditor::rect() );
    int offset( hasFrame() ? _frameWidth():0 );
    if( hasFrame() ) { rect.adjust( 0, offset-1, -offset-1, -offset-1 ); }   
    
    // set the proper right margin
    rect.setLeft( rect.right() - fontMetrics().lineSpacing() -1 );
    
    clear_icon_.paint( 
      &painter, rect, 
      Qt::AlignRight|Qt::AlignVCenter, 
      isEnabled() ? QIcon::Normal : QIcon::Disabled );
    
    _setClearButtonRect( rect );
    
  }
    
  painter.end();

  // normal painting (without frame)
  QLineEdit::paintEvent( event );
  
  if( hasFrame() ) 
  {

    // draw frame
    QPainter painter( this );    
    style()->drawPrimitive(QStyle::PE_FrameLineEdit, &panel, &painter, this);
    painter.end();
    
  }

}

//____________________________________________________________
void LineEditor::_modified( const QString& text )
{
  Debug::Throw( "LineEditor::_modified.\n" );
  bool modified( text != backup_ );
  if( modified != modified_ )
  { 
    modified_ = modified;
    emit modificationChanged( modified_ );
  }
  
}

//__________________________________________________________
void LineEditor::_installActions( void )
{
  Debug::Throw( "LineEditor::_installActions.\n" );

  // create actions
  addAction( undo_action_ = new QAction( IconEngine::get( ICONS::UNDO ), "&Undo", this ) );
  undo_action_->setShortcut( CTRL+Key_Z );
  undo_action_->setShortcutContext( WidgetShortcut );
  undo_action_->setEnabled( isUndoAvailable() );
  connect( undo_action_, SIGNAL( triggered() ), SLOT( undo() ) );

  addAction( redo_action_ = new QAction( IconEngine::get( ICONS::REDO ), "&Redo", this ) );
  redo_action_->setShortcut( SHIFT+CTRL+Key_Z );
  redo_action_->setEnabled( isRedoAvailable() );
  redo_action_->setShortcutContext( WidgetShortcut );
  connect( redo_action_, SIGNAL( triggered() ), SLOT( redo() ) );

  addAction( cut_action_ = new QAction( IconEngine::get( ICONS::CUT ), "Cu&t", this ) );
  cut_action_->setShortcut( CTRL+Key_X );
  cut_action_->setShortcutContext( WidgetShortcut );
  connect( cut_action_, SIGNAL( triggered() ), SLOT( cut() ) );

  addAction( copy_action_ = new QAction( IconEngine::get( ICONS::COPY ), "&Copy", this ) );
  copy_action_->setShortcut( CTRL+Key_C );
  copy_action_->setShortcutContext( WidgetShortcut );
  connect( copy_action_, SIGNAL( triggered() ), SLOT( copy() ) );

  addAction( paste_action_ = new QAction( IconEngine::get( ICONS::PASTE ), "&Paste", this ) );
  paste_action_->setShortcut( CTRL+Key_V );
  paste_action_->setShortcutContext( WidgetShortcut );
  connect( paste_action_, SIGNAL( triggered() ), SLOT( paste() ) );
  connect( qApp->clipboard(), SIGNAL( dataChanged() ), SLOT( _updatePasteAction() ) );
  _updatePasteAction();

  addAction( clear_action_ = new QAction( "&Clear", this ) );
  connect( clear_action_, SIGNAL( triggered() ), SLOT( clear() ) );

  addAction( select_all_action_ = new QAction( "Select all", this ) );
  select_all_action_->setShortcut( CTRL+Key_A );
  select_all_action_->setShortcutContext( WidgetShortcut );
  connect( select_all_action_, SIGNAL( triggered() ), SLOT( selectAll() ) );
  
  addAction( upper_case_action_ = new QAction( "&Upper case", this ) );
  upper_case_action_->setShortcut( CTRL+Key_U );
  upper_case_action_->setShortcutContext( WidgetShortcut );
  connect( upper_case_action_, SIGNAL( triggered() ), SLOT( upperCase() ) );

  addAction( lower_case_action_ = new QAction( "&Lower case", this ) );
  lower_case_action_->setShortcut( SHIFT+CTRL+Key_U );
  lower_case_action_->setShortcutContext( WidgetShortcut );
  connect( lower_case_action_, SIGNAL( triggered() ), SLOT( lowerCase() ) );

  // update actions that depend on the presence of a selection
  connect( this, SIGNAL( textChanged( const QString& ) ), SLOT( _updateUndoRedoActions() ) );
  connect( this, SIGNAL( selectionChanged() ), SLOT( _updateSelectionActions() ) );
  _updateUndoRedoActions();
  _updateSelectionActions();

}

//________________________________________________
void LineEditor::_updateUndoRedoActions( void )
{
  Debug::Throw( "LineEditor::_updateUndoRedoActions.\n" );
  undo_action_->setEnabled( isUndoAvailable() );
  redo_action_->setEnabled( isRedoAvailable() );
}

//________________________________________________
void LineEditor::_updateSelectionActions()
{
  
  Debug::Throw( "LineEditor::_updateSelectionActions.\n" );

  bool has_selection( hasSelectedText() );
  bool editable( !isReadOnly() );
  
  cut_action_->setEnabled( has_selection && editable );
  copy_action_->setEnabled( has_selection );
  upper_case_action_->setEnabled( has_selection && editable );
  lower_case_action_->setEnabled( has_selection && editable );
  
}

//________________________________________________
void LineEditor::_updatePasteAction( void )
{
  
  Debug::Throw( "LineEditor::_updatePasteAction.\n" );
  bool editable( !isReadOnly() );
  bool has_clipboard( !qApp->clipboard()->text().isEmpty() );
  paste_action_->setEnabled( editable && has_clipboard );
  
}

//______________________________________________________________
int LineEditor::_frameWidth( void ) const
{
  // what should appear here is style().pixelMetric( QStyle::PM_DefaultFrameWidth )
  // but for some reason the later does not work with Oxygen style.
  // for other styles, this results in having the LineEditors drawn too large.
  return QStyle::PM_DefaultFrameWidth;
}
