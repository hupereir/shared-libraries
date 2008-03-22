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

#include <assert.h>
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
  transparent_( false ),
  background_changed_( true ),
  highlighted_( false )
{ 
  Debug::Throw( "TransparentWidget::TransparentWidget.\n" ); 

  // actions
  _installActions();
  
}

//____________________________________________________________________
void TransparentWidget::setTint( const QColor& color )
{
  Debug::Throw( "TransparentWidget::tint.\n" ); 
  if( tint_color_ == color ) return;
  tint_color_ = color;
  background_changed_ = true;
}

//____________________________________________________________________
void TransparentWidget::setHighlight( const QColor& color )
{
  Debug::Throw( "TransparentWidget::highlight.\n" ); 
  if( highlight_color_ == color ) return;
  highlight_color_ = color;   
}


//____________________________________________________________________
void TransparentWidget::moveEvent( QMoveEvent* event )
{
  if( transparent() && !( tintColor().isValid() && tintColor().alpha() == 255 )  ) 
  {
    _setBackgroundChanged( true );
    update();
  }
  QWidget::moveEvent( event );
}


//____________________________________________________________________
void TransparentWidget::resizeEvent( QResizeEvent* event )
{
  _setBackgroundChanged( true );
  QWidget::resizeEvent( event );
}

//____________________________________________________________________
void TransparentWidget::showEvent( QShowEvent* event )
{
  _setBackgroundChanged( true );
  update(); 
  QWidget::showEvent( event );
}
    
//____________________________________________________________________
void TransparentWidget::enterEvent( QEvent* event )
{
  if( !highlighted_ && highlight_color_.isValid() ) 
  {
    highlighted_ = true;
    update();
  }
  QWidget::enterEvent( event );
}

//____________________________________________________________________
void TransparentWidget::leaveEvent( QEvent* event )
{
  if( highlighted_ && highlight_color_.isValid() ) 
  {
    highlighted_ = false;
    update();
  }
  QWidget::leaveEvent( event );
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
  
  QWidget::paintEvent( event );  
  
}

//____________________________________________________________________
void TransparentWidget::_updateConfiguration( void )
{
  
  Debug::Throw( "TransparentWidget::_updateConfiguration.\n" );
  
  // use transparency
  setTransparent( XmlOptions::get().get<bool>( "TRANSPARENT" ) );
  
  // tint
  QColor tint_color( XmlOptions::get().get<string>( "TRANSPARENCY_TINT_COLOR" ).c_str() );
  unsigned int tint_intensity(  XmlOptions::get().get<unsigned int>( "TRANSPARENCY_TINT_INTENSITY" ) );
  if( tint_color.isValid() && tint_intensity )
  {
    tint_color.setAlpha( tint_intensity );
    setTint( tint_color );
  } else setTint();
   
  // highlight
  QColor highlight_color( XmlOptions::get().get<string>( "TRANSPARENCY_HIGHLIGHT_COLOR" ).c_str() );
  unsigned int highlight_intensity(  XmlOptions::get().get<unsigned int>( "TRANSPARENCY_HIGHLIGHT_INTENSITY" ) );
  if( highlight_color.isValid() && highlight_intensity )
  {
    highlight_color.setAlpha( highlight_intensity );
    setHighlight( highlight_color );
  } else setHighlight();
 
}
  
//____________________________________________________________________
void TransparentWidget::_updateBackgroundPixmap( void )
{
  
  Debug::Throw( "TransparentWidget::_updateBackgroundPixmap.\n" );
  
  if( ( !transparent_ ) ||  ( tintColor().isValid() && tintColor().alpha() == 255 ) )
  {
    // solid background
    _backgroundPixmap() = QPixmap( size() );
    _backgroundPixmap().fill( palette().color( backgroundRole() ) );
    
  } else {
    
    // transparent background
    _backgroundPixmap() = BackgroundPixmap::get().pixmap( QRect( mapToGlobal( QPoint(0,0) ), size() ) );
    if( _backgroundPixmap().isNull() )
    {
      _backgroundPixmap() = QPixmap( size() );
      _backgroundPixmap().fill( palette().color( backgroundRole() ) );
    }
    
  }
  
  // tint
  if( tintColor().isValid() )
  {
    
    QPainter painter( &_backgroundPixmap() );
    painter.setPen( Qt::NoPen );
    painter.setBrush( tintColor() );
    painter.drawRect( _backgroundPixmap().rect() );
    
  }
  
  background_changed_ = false;
  
  Debug::Throw( "TransparentWidget::_updateBackgroundPixmap - done.\n" );
  return;
  
}


//____________________________________________________________________
void TransparentWidget::_installActions( void )
{
  Debug::Throw( "TransparentWidget::_installAction.\n" );
  // pixmap path
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  assert( !path_list.empty() );
  
  addAction( update_background_action_ = new QAction( "&Update background", this ) );
  connect( update_background_action_, SIGNAL( triggered() ), SLOT( _updateBackgroundPixmap() ) );

  reload_background_action_ = new QAction( IconEngine::get( ICONS::RELOAD, path_list ), "&Reload background", this );
  reload_background_action_->setToolTip( "Reinitialize transparent background" );
  connect( reload_background_action_, SIGNAL( triggered() ), &BackgroundPixmap::get(), SLOT( reload() ) );
  connect( &BackgroundPixmap::get(), SIGNAL( backgroundChanged() ), SLOT( _reloadBackground() ) );
};
