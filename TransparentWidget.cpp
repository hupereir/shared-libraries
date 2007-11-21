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
  \file    TransparentWidget.cpp
  \brief   transparent widget
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <QPainter>

#include "BackgroundPixmap.h"
#include "BaseIcons.h"
#include "IconEngine.h"
#include "TransparentWidget.h"
#include "XmlOptions.h"

using namespace std;
using namespace TRANSPARENCY;


//____________________________________________________________________
TransparentWidget::TransparentWidget( QWidget *parent, Qt::WindowFlags flags ):
  QWidget( parent, flags ),
  Counter( "TransparentWidget" ),
  background_changed_( true ),
  highlighted_( false )
{ 
  Debug::Throw( "TransparentWidget::TransparentWidget.\n" ); 

  // pixmap path
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  if( !path_list.size() ) throw runtime_error( DESCRIPTION( "no path to pixmaps" ) );

  reload_background_action_ = new QAction( IconEngine::get( ICONS::RELOAD, path_list ), "&Reload background", this );
  reload_background_action_->setToolTip( "Reinitialize transparent background" );
  connect( reload_background_action_, SIGNAL( triggered() ), SLOT( _reloadBackground() ) );
}

//____________________________________________________________________
void TransparentWidget::setTint( const QColor& color )
{
  Debug::Throw( "TransparentWidget::tint.\n" ); 
  if( tint_color_ == color ) return;
  tint_color_ = color; 
  background_changed_ = true;
  update();
}

//____________________________________________________________________
void TransparentWidget::setHighlight( const QColor& color )
{
  Debug::Throw( "TransparentWidget::highlight.\n" ); 
  if( highlight_color_ == color ) return;
  highlight_color_ = color;   
}

//____________________________________________________________________
void TransparentWidget::paintEvent( QPaintEvent* event )
{
  
  if( background_changed_ ) _updateBackgroundPixmap();
  if( !_backgroundPixmap().isNull() )
  {
    QPainter painter( this );
    QRect rect( TransparentWidget::rect()&event->rect() );
    painter.drawPixmap( rect, _backgroundPixmap(), rect );
    
    if( highlighted_ && highlight_color_.isValid() )
    {
      painter.setPen( Qt::NoPen );
      painter.setBrush( highlight_color_ );
      painter.drawRect( rect );
    }
  }
  return QWidget::paintEvent( event );  
  
}


//____________________________________________________________________
void TransparentWidget::_updateBackgroundPixmap( void )
{
  Debug::Throw( "TransparentWidget::_updateBackgroundPixmap.\n" );
  _backgroundPixmap() = BackgroundPixmap::get().pixmap( QRect( mapToGlobal( QPoint(0,0) ), size() ) );
  if( !_backgroundPixmap().isNull() && tint_color_.isValid() )
  {
    QPainter painter( &_backgroundPixmap() );
    painter.setPen( Qt::NoPen );
    painter.setBrush( tint_color_ );
    painter.drawRect( _backgroundPixmap().rect() );
  }
  
  background_changed_ = false;
  
  return;
  
}
