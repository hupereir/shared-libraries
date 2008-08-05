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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/
 
/*!
  \file ImprovedLineEditor.cpp
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrameV2>

#include "BaseIcons.h"
#include "Debug.h"
#include "IconEngine.h"
#include "ImprovedLineEditor.h"
#include "QtUtil.h"

using namespace std;

//______________________________________________________________
ImprovedLineEditor::ImprovedLineEditor( QWidget* parent ):
  LineEditor( parent ),
  has_clear_button_( false ),
  has_frame_( true ),
  clear_icon_( IconEngine::get( ICONS::CLEAR ) )
{ 

  Debug::Throw( "ImprovedLineEditor::ImprovedLineEditor.\n" ); 
  
  QFrame frame(0);
  frame.setFrameStyle( QFrame::StyledPanel|QFrame::Sunken );
  frame_width_ = frame.frameWidth();
  
}

//______________________________________________________________
void ImprovedLineEditor::setHasClearButton( const bool& value )
{
  if( value == has_clear_button_ ) return;
  has_clear_button_ = value;
  
  if( has_clear_button_ )
  {

    // set frame flag from base class
    setFrame( QLineEdit::hasFrame() );
    
    // disable QLineEdit frame
    QLineEdit::setFrame( false );

    // reset contents margins
    int offset( hasFrame() ? frame_width_:0 );
    setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing(), offset );

  } else {
    
    // reset frame
    QLineEdit::setFrame( has_frame_ );
    
    // reset contents margins
    setContentsMargins( 0, 0, 0, 0 );
  
  }
  
  update();
  
}
  
//______________________________________________________________
void ImprovedLineEditor::setFrame( const bool& value )
{
  
  // do nothing if value is unchanged
  if( value == hasFrame() ) return;
  
  has_frame_ = value;
  if( !_hasClearButton() ) QLineEdit::setFrame( value );
  else {
    
    // reset contents margins
    int offset( hasFrame() ? frame_width_:0 );
    setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing(), offset );
    
  }
  
}

//________________________________________________
void ImprovedLineEditor::mouseMoveEvent( QMouseEvent* event )
{
  
  // check clear button
  if( !_hasClearButton() ) return LineEditor::mouseMoveEvent( event );
  
  // check event position vs button location
  if( contentsRect().contains( event->pos() ) || text().isEmpty() ) 
  { 
    if( cursor().shape() != Qt::IBeamCursor ) setCursor( Qt::IBeamCursor ); 
    return LineEditor::mouseMoveEvent( event ); 
  } else if( cursor().shape() == Qt::IBeamCursor ) unsetCursor();
  
}

//________________________________________________
void ImprovedLineEditor::mousePressEvent( QMouseEvent* event )
{

  // check clear button
  if( !_hasClearButton() ) return LineEditor::mousePressEvent( event );
  
  // check mouse position
  if( contentsRect().contains( event->pos() ) || text().isEmpty() ) 
  { LineEditor::mousePressEvent( event ); }
  
}

//________________________________________________
void ImprovedLineEditor::mouseReleaseEvent( QMouseEvent* event )
{

  // check clear button
  if( !_hasClearButton() ) return LineEditor::mouseReleaseEvent( event );
  
  // check mouse position
  if( contentsRect().contains( event->pos() ) || text().isEmpty() ) { LineEditor::mouseReleaseEvent( event ); } 
  else {
    clear();
    emit cleared();
  }
}

//________________________________________________
void ImprovedLineEditor::paintEvent( QPaintEvent* event )
{
  Debug::Throw( "ImprovedLineEditor::paintEvent.\n" );

  
  // check clear button
  if( !_hasClearButton() ) return LineEditor::paintEvent( event );

  {
    // draw white background
    QPainter painter( this );
    painter.setPen( Qt::NoPen );
    painter.setBrush( palette().color( QPalette::Base ) );
    QRect rect( ImprovedLineEditor::rect() );
    
    if( hasFrame() )
    {
      // adjust rect to account for the frame
      int offset( frame_width_ );
      rect.adjust( offset, offset, -offset, -offset );
    }
    
    painter.drawRect( rect );

    // paint the button at the correct place
    if( !(text().isNull() || text().isEmpty() ) )
    {
      rect.adjust( 0, 0, -1, -1 );
      clear_icon_.paint( &painter, rect, Qt::AlignRight|Qt::AlignVCenter );
    }
    
    painter.end();
  }

  // normal painting (without frame)
  LineEditor::paintEvent( event );
  
  if( hasFrame() ) 
  {
    // draw frame
    QPainter painter( this );    
    
    QStyleOptionFrameV2 panel;
    panel.initFrom( this );
    panel.rect = ImprovedLineEditor::rect();
    panel.state |= QStyle::State_Sunken;
    if( hasFocus() ) panel.state |= QStyle::State_HasFocus;
    
    // here one would prefer PE_FrameLineEdit over PE_Frame, but we are unable
    // to make it work for both oxygen and plastik themes.
    style()->drawPrimitive(QStyle::PE_Frame, &panel, &painter, this);
   
    painter.end();
    
  }

}
