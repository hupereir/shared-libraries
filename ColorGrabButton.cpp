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


#include "ColorGrabButton.h"
#include "Debug.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>
#include <QMouseEvent>

#if QT_VERSION >= 0x050000
#include <QScreen>
#endif

//______________________________________________
ColorGrabButton::ColorGrabButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( "ColorGrabButton" ),
    locked_( false ),
    mouseDown_( false )
{
    Debug::Throw( "ColorGrabButton::ColorGrabButton.\n" );
    connect( this, SIGNAL( clicked( void ) ), SLOT( _grabColor( void ) ) );
}


//________________________________________________________
void ColorGrabButton::_grabColor( void )
{

    Debug::Throw( "ColorGrabButton::_grabColor.\n" );

    #if QT_VERSION >= 0x050000
    // need to explicitely override cursor for Qt5
    qApp->setOverrideCursor( Qt::CrossCursor );
    #endif

    grabMouse(Qt::CrossCursor);
    locked_ = true;

}

//_____________________________________________________________
void ColorGrabButton::mousePressEvent( QMouseEvent *event )
{
    Debug::Throw( "ColorGrabButton::mousePressEvent.\n" );

    // check button
    if( event->button() != Qt::LeftButton ) return QToolButton::mousePressEvent( event );

    // do nothing if mouse is not locked
    if( !locked_ ) return QToolButton::mousePressEvent( event );

    // do nothing if mouse is already down
    if( mouseDown_ ) return;
    mouseDown_ = true;

}

//_____________________________________________________________
void ColorGrabButton::mouseReleaseEvent( QMouseEvent *event )
{

    Debug::Throw( "ColorGrabButton::mouseReleaseEvent.\n" );

    // do nothing if mouse is not locked
    if( !locked_ ) return QToolButton::mouseReleaseEvent( event );

    // check button
    if( event->button() == Qt::LeftButton && mouseDown_ )
    {
        // get color under mouse
        _selectColorFromMouseEvent( event );
    }

    mouseDown_ = false;
    locked_ = false;
    releaseMouse();

    #if QT_VERSION >= 0x050000
    // need to explicitely release cursor for Qt5
    qApp->restoreOverrideCursor();
    #endif


}

//_____________________________________________________________
void ColorGrabButton::mouseMoveEvent( QMouseEvent *event )
{

    Debug::Throw( "ColorGrabButton::mouseMoveEvent.\n" );

    // do nothing if mouse is not locked
    if( !( locked_ && mouseDown_ ) ) return QToolButton::mouseMoveEvent( event );
    _selectColorFromMouseEvent( event );

}

//_____________________________________________________________
void ColorGrabButton::_selectColorFromMouseEvent( QMouseEvent *event )
{
    Debug::Throw() << "ColorGrabButton::_selectColorFromMouseEvent - (" << event->globalX() << "," << event->globalY() << ")" << endl;

    // grab desktop window under cursor
    // convert to image.
    #if QT_VERSION >= 0x050000
    QImage image( QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(),event->globalX(), event->globalY(), 2, 2 ).toImage() );
    #else
    QImage image( QPixmap::grabWindow(QApplication::desktop()->winId(),event->globalX(), event->globalY(), 2, 2 ).toImage() );
    #endif

    // ensure image is deep enough
    if (image.depth() != 32) image = image.convertToFormat(QImage::Format_RGB32);

    // assign color to the selection frame
    emit colorSelected( QColor( image.pixel( 1, 1 ) ).name() );

    return;
}
