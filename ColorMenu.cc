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
  \file ColorMenu.cc
  \brief color picker
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QColorDialog>
#include <QPainter>

#include "ColorMenu.h"
#include "CustomPixmap.h"
#include "Str.h"

using namespace std;

//________________________________________________
const QSize ColorMenu::PixmapSize = QSize( 21, 21 );
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
    
    // create action
    QAction* action = addAction( iter->first.name() );
    action->setIcon( iter->second );
    actions_.insert( make_pair( action, iter->first ) );
    
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
  {
    QPixmap pixmap( CustomPixmap().empty( PixmapSize ) );

    QLinearGradient gradient(QPointF(0, 0), pixmap.rect().bottomRight());
    gradient.setColorAt(0, color);
    gradient.setColorAt(1, color.light(135));
    
    QPainter painter( &pixmap );
    painter.fillRect( pixmap.rect(), gradient );
    painter.end();

    colors_.insert( make_pair( color, pixmap ) );
    
  }

  return;
}
