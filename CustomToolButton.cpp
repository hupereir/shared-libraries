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
  \file CustomToolButton.cpp
  \brief customized tool button to display tooltip in a dedicated label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QPixmap>
#include <QPainter>
#include <QStylePainter>
#include <QStyleOptionToolButton>

#include <sstream>

#include "CustomToolButton.h"
#include "XmlOptions.h"

using namespace std;

//___________________________________________________________________
const QSize CustomToolButton::BigIconSize = QSize( 32, 32 );
const QSize CustomToolButton::SmallIconSize = QSize( 24, 24 );

//___________________________________________________________________
CustomToolButton::CustomToolButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( "CustomToolButton" ),
    big_icon_size_( BigIconSize ),
    small_icon_size_( SmallIconSize ),
    rotation_( NONE )
{

  Debug::Throw( "CustomToolButton::CustomToolButton.\n" );
  
  _updateConfiguration();
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  
}

//______________________________________________________________________
QSize CustomToolButton::sizeHint( void ) const
{ 
  QSize size( QToolButton::sizeHint() );
  if( _rotation() != NONE ) size.transpose();
  return size;
}

//______________________________________________________________________
void CustomToolButton::paintEvent( QPaintEvent* event )
{ 
  
  // default implementation if not rotated
  if( _rotation() == NONE ) return QToolButton::paintEvent( event );
  
  // rotated paint
  QStylePainter painter(this);
  QStyleOptionToolButton option;
  initStyleOption(&option);
  
  // first draw normal frame and not text/icon
  option.text = QString();
  option.icon = QIcon();  
  painter.drawComplexControl(QStyle::CC_ToolButton, option);
  
  // rotate the options
  QSize size( option.rect.size() );
  size.transpose();
  option.rect.setSize( size );
  
  // rotate the painter
  if( _rotation() == COUNTERCLOCKWISE )
  {
    painter.translate( 0, height() );
    painter.rotate( -90 );
  } else {
    painter.translate( width(), 0 );
    painter.rotate( 90 );
  }
  
  // paint text
  option.text = text();
  option.icon = icon();
  painter.drawControl(QStyle::CE_ToolButtonLabel, option);
  painter.end();
  
  return;
}

//_________________________________________________________________
void CustomToolButton::_updateConfiguration( void )
{
  Debug::Throw( "CustomToolButton::_updateConfiguration.\n");

  // auto-raise
  setAutoRaise( true );

  // pixmap size
  QToolButton::setIconSize(  XmlOptions::get().get<bool>("USE_BIG_PIXMAP" ) ? bigIconSize():smallIconSize() );
  
  // text labels
  if( XmlOptions::get().get<bool>("USE_TEXT_LABEL" ) ) setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
  else setToolButtonStyle( Qt::ToolButtonIconOnly );
  
  adjustSize();
  
}
