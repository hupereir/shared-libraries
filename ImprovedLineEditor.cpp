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
  clear_icon_( IconEngine::get( ICONS::CLEAR ) )
{
  Debug::Throw( "ImprovedLineEditor::ImprovedLineEditor.\n" );
  
  // set proper margin
  int offset( QStyle::PM_DefaultFrameWidth );
  setContentsMargins( offset, offset, offset + fontMetrics().lineSpacing(), offset );

  // disable QLineEdit frame
  QLineEdit::setFrame( false );
  
}

//________________________________________________
void ImprovedLineEditor::mouseMoveEvent( QMouseEvent* event )
{
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
  if( contentsRect().contains( event->pos() ) || text().isEmpty() ) 
  { LineEditor::mousePressEvent( event ); }
  
}

//________________________________________________
void ImprovedLineEditor::mouseReleaseEvent( QMouseEvent* event )
{
  if( contentsRect().contains( event->pos() ) || text().isEmpty() ) { LineEditor::mouseReleaseEvent( event ); } 
  else clear();
}

//________________________________________________
void ImprovedLineEditor::paintEvent( QPaintEvent* event )
{
  
  Debug::Throw( "ImprovedLineEditor::paintEvent.\n" );

  {
    // draw white background
    QPainter painter( this );
    painter.setPen( Qt::NoPen );
    painter.setBrush( palette().color( QPalette::Base ) );
    QRect rect( ImprovedLineEditor::rect() );
    int offset( QStyle::PM_DefaultFrameWidth );
    rect.adjust( offset, offset, -offset, -offset );
    painter.drawRect( rect );

    // paint the button at the correct place
    if( !(text().isNull() || text().isEmpty() ) )
    {
      rect.adjust( 0, 0, -offset/2, -offset/2 );
      clear_icon_.paint( &painter, rect, Qt::AlignRight|Qt::AlignVCenter );
    }
    
    painter.end();
  }

  // normal painting (without frame)
  LineEditor::paintEvent( event );
  
  {
    // draw frame
    QPainter painter( this );    
    
    QRect rect( ImprovedLineEditor::rect() );  
    QStyleOptionFrameV2 panel;
    panel.initFrom( this );
    panel.rect = rect;
    panel.state |= QStyle::State_Sunken;
    if( hasFocus() ) panel.state |= QStyle::State_HasFocus;
    style()->drawPrimitive(QStyle::PE_FrameLineEdit, &panel, &painter, this);
    
    painter.end();
    
  }

}
