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
  
  int offset( QStyle::PM_DefaultFrameWidth/2 );
  setContentsMargins( offset, offset, 50, offset );
  
  //setContentsMargins( offset, offset, offset, offset );
  setFrame( false );
  
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
    int offset( 2 );
    rect.adjust( offset, offset, -offset, -offset );
    painter.drawRect( rect );

    // paint the button at the correct place
    rect.adjust( 0, -1, 0, 1 );
    clear_icon_.paint( &painter, rect, Qt::AlignRight|Qt::AlignVCenter );
    
    painter.end();
    
  }
  
  // normal painting (without frame)
  LineEditor::paintEvent( event );
 
  {
    // draw frame
    QPainter painter( this );
    
    QStyleOptionFrameV2 panel;
    panel.initFrom( this );
    
    QRect rect( ImprovedLineEditor::rect() );  
    panel.rect = rect;
    panel.state |= QStyle::State_Sunken;
    if( hasFocus() ) panel.state |= QStyle::State_HasFocus;
    style()->drawPrimitive(QStyle::PE_Frame, &panel, &painter, this);

    painter.end();
    
  }
    
}

//________________________________________________
void ImprovedLineEditor::_clear( void )
{
  Debug::Throw(0, "ImprovedLineEditor::_clear.\n");
}
