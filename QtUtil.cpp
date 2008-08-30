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
  \file QtUtil.cpp
  \brief Qt utility static methods
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/


#include <assert.h>
#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QFontMetrics>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QPainter>

#include "BaseIcons.h"
#include "BaseDialog.h"
#include "PixmapEngine.h"
#include "Debug.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "Util.h"

using namespace std;

//____________________________________________________________
const unsigned int QtUtil::max_opacity_ = 0xffffffff;
const char* QtUtil::opacity_prop_name_ = "_NET_WM_WINDOW_OPACITY";

//____________________________________________________________
QRegion QtUtil::round( const QRect& region, const unsigned int& corners )
{

  QRegion out( region );

  // top left
  if( corners & TOP_LEFT )
  {
    const QPoint& point( region.topLeft() );
    out -= QRegion(point.x(), point.y()+0, 5, 1);
    out -= QRegion(point.x(), point.y()+1, 3, 1);
    out -= QRegion(point.x(), point.y()+2, 2, 1);
    out -= QRegion(point.x(), point.y()+3, 1, 2);
  }

  // top right
  if( corners & TOP_RIGHT )
  {
    const QPoint& point( region.topRight() );
    out -= QRegion(point.x()-4, point.y()+0, 5, 1);
    out -= QRegion(point.x()-2, point.y()+1, 3, 1);
    out -= QRegion(point.x()-1, point.y()+2, 2, 1);
    out -= QRegion(point.x()-0, point.y()+3, 1, 2);
  }

  // bottom left
  if( corners & BOTTOM_LEFT )
  {
    const QPoint& point( region.bottomLeft() );
    out -= QRegion(point.x(), point.y()-0, 5, 1);
    out -= QRegion(point.x(), point.y()-1, 3, 1);
    out -= QRegion(point.x(), point.y()-2, 2, 1);
    out -= QRegion(point.x(), point.y()-4, 1, 2);
  }


  // bottom right
  if( corners & BOTTOM_RIGHT )
  {
    const QPoint& point( region.bottomRight() );
    out -= QRegion(point.x()-4, point.y()-0, 5, 1);
    out -= QRegion(point.x()-2, point.y()-1, 3, 1);
    out -= QRegion(point.x()-1, point.y()-2, 2, 1);
    out -= QRegion(point.x()-0, point.y()-4, 1, 2);
  }

  return out;

}

//____________________________________________________________
QPixmap QtUtil::round( const QPixmap& pixmap, const unsigned int& corners )
{

  QPixmap out( pixmap );
  QPainter painter( &out );
  QColor color( QRgb(0) );
  painter.setBrush( color );

  // top left
  if( corners & TOP_LEFT )
  {
    const QPoint& point( out.rect().topLeft() );
    painter.drawRect(point.x(), point.y()+0, 5, 1);
    painter.drawRect(point.x(), point.y()+1, 3, 1);
    painter.drawRect(point.x(), point.y()+2, 2, 1);
    painter.drawRect(point.x(), point.y()+3, 1, 2);
  }

  // top right
  if( corners & TOP_RIGHT )
  {
    const QPoint& point( out.rect().topRight() );
    painter.drawRect(point.x()-4, point.y()+0, 5, 1);
    painter.drawRect(point.x()-2, point.y()+1, 3, 1);
    painter.drawRect(point.x()-1, point.y()+2, 2, 1);
    painter.drawRect(point.x()-0, point.y()+3, 1, 2);
  }

  // bottom left
  if( corners & BOTTOM_LEFT )
  {
    const QPoint& point( out.rect().bottomLeft() );
    painter.drawRect(point.x(), point.y()-0, 5, 1);
    painter.drawRect(point.x(), point.y()-1, 3, 1);
    painter.drawRect(point.x(), point.y()-2, 2, 1);
    painter.drawRect(point.x(), point.y()-4, 1, 2);
  }


  // bottom right
  if( corners & BOTTOM_RIGHT )
  {
    const QPoint& point( out.rect().bottomRight() );
    painter.drawRect(point.x()-4, point.y()-0, 5, 1);
    painter.drawRect(point.x()-2, point.y()-1, 3, 1);
    painter.drawRect(point.x()-1, point.y()-2, 2, 1);
    painter.drawRect(point.x()-0, point.y()-4, 1, 2);
  }

  painter.end();
  
  return out;

}

//____________________________________________________________
void QtUtil::moveWidget( QWidget* widget, QPoint position )
{

  Debug::Throw( "QtUtil::moveWidget.\n" );
  if( !widget ) return;
  QDesktopWidget* desktop( qApp->desktop() );
  QRect geometry( desktop->screenGeometry( desktop->screenNumber( widget ) ) );
  if( position.y() + widget->height() > geometry.bottom()+1 ) position.setY( geometry.bottom() + 1 - widget->height() );

  widget->move( position );
}

//____________________________________________________________
QPoint QtUtil::centerOnPointer( const QSize& size )
{
  Debug::Throw( "QtUtil::centerOnPointer.\n" );

  // get cursor position
  QPoint point( QCursor::pos() );

  point.setX( point.x() - size.width()/2 );
  point.setY( point.y() - size.height()/2 );

  // retrieve desktop
  QDesktopWidget *desktop( qApp->desktop() );

  // check point against desktop size
  if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
  if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );

  // check point against (0,0)
  if( point.x() < 0 ) point.setX( 0 );
  if( point.y() < 0 ) point.setY( 0 );
  return point;
}


//____________________________________________________________
QPoint QtUtil::centerOnWidget( const QSize& size, QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnWidget.\n" );
  if( !widget ) return centerOnDesktop( size );

  Debug::Throw() << "QtUtil::centerOnWidget - size: (" << size.width() << "," << size.height() << ")" << endl;

  // get parent position and size
  QPoint point( widget->pos() );
  if( widget != widget->window() ) point = widget->mapToGlobal( point );
  
  QSize parent_size( widget->frameSize() );

  Debug::Throw() << "QtUtil::centerOnWidget - parent size: (" << parent_size.width() << "," << parent_size.height() << ")" << endl;
  Debug::Throw() << "QtUtil::centerOnWidget - parent position: (" << point.x() << "," << point.y() << ")" << endl;

  point.setX( point.x() + ( parent_size.width() - size.width() )/2 );
  point.setY( point.y() + ( parent_size.height() - size.height() )/2 );

  // retrieve desktop
  QDesktopWidget *desktop( qApp->desktop() );

  // check point against desktop size
  if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
  if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );

  // check point against (0,0)
  if( point.x() < 0 ) point.setX( 0 );
  if( point.y() < 0 ) point.setY( 0 );
  return point;

}

//____________________________________________________________
QPoint QtUtil::centerOnDesktop( const QSize& size )
{
  Debug::Throw( "QtUtil::centerOnDesktop.\n" );

  // retrieve desktop
  QDesktopWidget *desktop( qApp->desktop() );
  QSize deskmain_window_size( desktop->frameSize() );

  // get parent position and size
  QPoint point( desktop->pos() );

  point.setX( point.x() + ( deskmain_window_size.width() - size.width() )/2 );
  point.setY( point.y() + ( deskmain_window_size.height() - size.height() )/2 );

  // check point against desktop size
  if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
  if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );

  // check point against (0,0)
  if( point.x() < 0 ) point.setX( 0 );
  if( point.y() < 0 ) point.setY( 0 );
  return point;
}

//____________________________________________________________
void QtUtil::centerOnPointer( QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnPointer.\n" );
  assert( widget );

  // move widget
  widget->move( centerOnPointer( widget->frameSize() ) );
  qApp->processEvents();
  return;
}

//____________________________________________________________
void QtUtil::centerOnParent( QWidget* widget )
{
  assert( widget );
  return centerOnWidget( widget, widget->parentWidget() );
}


//____________________________________________________________
void QtUtil::centerOnWidget( QWidget* widget, QWidget* parent )
{
  Debug::Throw( "QtUtil::centerOnParent.\n" );
  assert( widget );

  // get parent widget
  if( !( parent && parent->window() ) ) centerOnDesktop( widget );
  else widget->move( centerOnWidget( widget->frameSize(), parent->window() ) );
  return;
}


//____________________________________________________________
void QtUtil::centerOnDesktop( QWidget* widget )
{
  Debug::Throw( "QtUtil::centerOnDesktop.\n" );
  assert( widget );
  widget->move( centerOnDesktop( widget->frameSize() ) );
  return;
}

//____________________________________________________________
void QtUtil::uniconify( QWidget *widget )
{

  Debug::Throw( "QtUtil::uniconify.\n" );
  if( !widget->isTopLevel() ) return;

  // this is portable but may not work on old enough X11 Qt versions
  if( widget->window()->isMinimized() )
  { widget->window()->hide(); }

  widget->activateWindow();
  widget->window()->show();
  widget->window()->raise();

  return;

}

//___________________________________________________________
unsigned int QtUtil::opacity( const QWidget* widget )
{
  Debug::Throw( "QtUtil::opacity.\n" );
  return (unsigned int)( max_opacity_*widget->windowOpacity() );
}

//__________________________________________________________
void QtUtil::setOpacity( QWidget* widget, const double& value )
{
  Debug::Throw( "QtUtil::setOpacity.\n" );
  widget->setWindowOpacity( value );
}

