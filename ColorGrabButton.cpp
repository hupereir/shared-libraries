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
\file ColorGrabButton.cpp
\brief used to pick color from screen
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>

#include "ColorGrabButton.h"
#include "Debug.h"

using namespace std;

//______________________________________________
ColorGrabButton::ColorGrabButton( QWidget* parent ):
  QToolButton( parent ),
  Counter( "ColorGrabButton" ),
  locked_( false ),
  mouse_down_( false )
{
  Debug::Throw( "ColorGrabButton::ColorGrabButton.\n" );
  connect( this, SIGNAL( clicked( void ) ), SLOT( _grabColor( void ) ) );
}


//________________________________________________________
void ColorGrabButton::_grabColor( void )
{ 
  
  Debug::Throw( "ColorGrabButton::_grabColor.\n" );
  grabMouse(Qt::CrossCursor);
  locked_ = true;

}

//_____________________________________________________________
void ColorGrabButton::mousePressEvent( QMouseEvent *event )
{
  Debug::Throw( 0, "ColorGrabButton::mousePressEvent.\n" );
  
  // check button
  if( event->button() != Qt::LeftButton ) return QToolButton::mousePressEvent( event );
    
  // do nothing if mouse is not locked
  if( !locked_ ) return QToolButton::mousePressEvent( event );

  // do nothing if mouse is already down
  if( mouse_down_ ) return;
  mouse_down_ = true;

}
  
//_____________________________________________________________
void ColorGrabButton::mouseReleaseEvent( QMouseEvent *event )
{

  Debug::Throw( "ColorGrabButton::mouseReleaseEvent.\n" );
    
  // do nothing if mouse is not locked
  if( !locked_ ) return QToolButton::mouseReleaseEvent( event );
  
  // check button
  if( event->button() == Qt::LeftButton && mouse_down_ )
  {  
    // get color under mouse
    _selectColorFromMouseEvent( event );
  }
  
  mouse_down_ = false;
  locked_ = false;
  releaseMouse();
  
}

//_____________________________________________________________
void ColorGrabButton::mouseMoveEvent( QMouseEvent *event )
{
  
  Debug::Throw( "ColorGrabButton::mouseMoveEvent.\n" );
  
  // do nothing if mouse is not locked
  if( !( locked_ && mouse_down_ ) ) return QToolButton::mouseMoveEvent( event );
  _selectColorFromMouseEvent( event );

}

//_____________________________________________________________
void ColorGrabButton::_selectColorFromMouseEvent( QMouseEvent *event )
{
  Debug::Throw() << "ColorGrabButton::_selectColorFromMouseEvent - (" << event->globalX() << "," << event->globalY() << ")" << endl;
    
  // grab desktop window under cursor
  // convert to image.
  QImage image( QPixmap::grabWindow(QApplication::desktop()->winId(),event->globalX(), event->globalY(), 2, 2 ).toImage() );

  // ensure image is deep enough
  if (image.depth() != 32) image = image.convertToFormat(QImage::Format_RGB32);
  
  // assign color to the selection frame
  emit colorSelected( QColor( image.pixel( 1, 1 ) ).name() );
  
  return;
}
