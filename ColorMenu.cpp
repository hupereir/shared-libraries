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
  \file ColorMenu.cpp
  \brief color picker
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QColorDialog>
#include <QLabel>
#include <QPainter>
#include <QWidgetAction>

#include "ColorMenu.h"
#include "Str.h"

using namespace std;

//________________________________________________
const QSize ColorMenu::PixmapSize = QSize( 20, 20 );
const std::string ColorMenu::NONE = "None";

//_______________________________________________
ColorMenu::ColorMenu( QWidget* parent ):
  QMenu( parent ),
  Counter( "ColorMenu" )
{ 
  Debug::Throw( "ColorMenu::ColorMenu.\n" );
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _selected( QAction* ) ) );
  connect( this, SIGNAL( aboutToShow() ), SLOT( _display() ) );
}

//_______________________________________________
void ColorMenu::add( const string& colorname )
{ 
  Debug::Throw( "ColorMenu::add.\n" );
  
  if( colorname != NONE ) _add( QColor( colorname.c_str() ) );
  
}

//_______________________________________________
ColorMenu::ColorSet ColorMenu::colors( void ) const
{ 
  
  ColorSet out;
  for( ColorMap::const_iterator iter = colors_.begin(); iter != colors_.end(); iter++ )
  { out.insert( iter->first ); }

  return out;
}

//_______________________________________________
void ColorMenu::paintEvent( QPaintEvent* event )
{ 
  
  static const int margin = 2;
  
  // default paint
  QMenu::paintEvent( event );
  
  // loop over actions associated to existing colors
  QPainter painter( this );
  painter.setPen( Qt::NoPen );
  for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); iter++ )
  {
    QRect action_rect( actionGeometry( iter->first ) );
    if( !event->rect().intersects( action_rect ) ) continue;
    action_rect.adjust( 2*margin + 20, margin, -2*margin-1, -margin );
    action_rect.setWidth( action_rect.height() );
    painter.setBrush( colors_[iter->second] );
    //painter.drawRect( action_rect.intersected( event->rect() ) );
    painter.setRenderHints(QPainter::Antialiasing );
    painter.drawEllipse( action_rect );
  }
  
  painter.end();
  
}

//_______________________________________________
void ColorMenu::_display( void )
{
  
  Debug::Throw( "ColorMenu::_display.\n" );
  
  // clear menu
  clear();
  
  // new color action
  addAction( "&New", this, SLOT( _new() ) );
  
  // default color action
  addAction( "&Default", this, SLOT( _default() ) );
  
  // clear actions
  actions_.clear();
  
  for( ColorMap::iterator iter = colors_.begin(); iter != colors_.end(); iter++ )
  {
    
    // create pixmap if not done already
    if( iter->second == Qt::NoBrush )
    {
      
      //QRectF rect( QPointF(0,0), QSize( sizeHint().width()-6, PixmapSize.height() ) );
      QRectF rect( QPointF(0,0), PixmapSize );
      QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
      gradient.setColorAt(0, iter->first );
      gradient.setColorAt(1, iter->first.light(135));
      iter->second = QBrush( gradient ); 
      
    }
    
    // create action
    QAction* action = new QAction( this );
    actions_.insert( make_pair( action, iter->first ) );
    addAction( action );
   
  };
    
  return;

}

//_______________________________________________
void ColorMenu::_new( void )
{
  
  Debug::Throw( "ColorMenu::_new.\n" );
  QColor color( QColorDialog::getColor( Qt::white, this ) );
  if( color.isValid() )
  {
    _add( color );
    last_color_ = color; 
    emit selected( color );
  }
  
}

//_______________________________________________
void ColorMenu::_default( void )
{
  
  Debug::Throw( "ColorMenu::_default.\n" );
  last_color_ = QColor(); 
  emit selected( QColor() );
  
}

//_______________________________________________
void ColorMenu::_selected( QAction* action )
{
  Debug::Throw( "ColorMenu::_selected.\n" );
  std::map<QAction*,QColor>::iterator iter = actions_.find( action );
  if( iter != actions_.end() ) 
  {
    last_color_ = iter->second;
    emit selected( iter->second );
  }
}

//_______________________________________________
void ColorMenu::_add( const QColor& color )
{
  
  if( color.isValid() && colors_.find( color ) == colors_.end() )
  { colors_.insert( make_pair( color, QPixmap() ) ); }

  return;
}
