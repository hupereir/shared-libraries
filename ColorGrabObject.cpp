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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "ColorGrabObject.h"
#include "Debug.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QCursor>
#include <QMouseEvent>

#if QT_VERSION >= 0x050000
#include <QScreen>
#endif

//______________________________________________
ColorGrabObject::ColorGrabObject( QWidget* parent ):
    QObject( parent ),
    Counter( "ColorGrabObject" ),
    locked_( false ),
    mouseDown_( false )
{
    Debug::Throw( "ColorGrabObject::ColorGrabObject.\n" );
    Q_CHECK_PTR( parent );

    connect( parent, SIGNAL( clicked( void ) ), SLOT( _grabColor( void ) ) );
    parent->installEventFilter( this );

}

//________________________________________________________
void ColorGrabObject::_grabColor( void )
{

    Debug::Throw( "ColorGrabObject::_grabColor.\n" );

    #if QT_VERSION >= 0x050000
    // need to explicitely override cursor for Qt5
    qApp->setOverrideCursor( Qt::CrossCursor );
    #endif

    static_cast<QWidget*>(parent())->grabMouse(Qt::CrossCursor);
    locked_ = true;

}

//_____________________________________________________________
bool ColorGrabObject::eventFilter( QObject* object, QEvent* event )
{

    // check object
    if( object != parent() ) return false;

    switch( event->type() )
    {

        case QEvent::MouseButtonPress:
        return _mousePressEvent( static_cast<QMouseEvent*>( event ) );

        case QEvent::MouseButtonRelease:
        return _mouseReleaseEvent( static_cast<QMouseEvent*>( event ) );

        case QEvent::MouseMove:
        return _mouseMoveEvent( static_cast<QMouseEvent*>( event ) );

        default: break;
    }

    return false;
}

//_____________________________________________________________
bool ColorGrabObject::_mousePressEvent( QMouseEvent *event )
{
    Debug::Throw( "ColorGrabObject::_mousePressEvent.\n" );

    // check button
    if( event->button() != Qt::LeftButton ) return false;

    // do nothing if mouse is not locked
    if( !locked_ ) return false;

    // do nothing if mouse is already down
    if( !mouseDown_ ) mouseDown_ = true;
    return true;

}

//_____________________________________________________________
bool ColorGrabObject::_mouseReleaseEvent( QMouseEvent *event )
{

    Debug::Throw( "ColorGrabObject::mouseReleaseEvent.\n" );

    // do nothing if mouse is not locked
    if( !locked_ ) return false;

    // check button
    if( event->button() == Qt::LeftButton && mouseDown_ )
    {
        // get color under mouse
        _selectColorFromMouseEvent( event );
    }

    mouseDown_ = false;
    locked_ = false;
    static_cast<QWidget*>(parent())->releaseMouse();

    #if QT_VERSION >= 0x050000
    // need to explicitely release cursor for Qt5
    qApp->restoreOverrideCursor();
    #endif

    return true;

}

//_____________________________________________________________
bool ColorGrabObject::_mouseMoveEvent( QMouseEvent *event )
{

    Debug::Throw( "ColorGrabObject::_mouseMoveEvent.\n" );

    // do nothing if mouse is not locked
    if( !( locked_ && mouseDown_ ) ) return false;
    _selectColorFromMouseEvent( event );
    return true;

}

//_____________________________________________________________
void ColorGrabObject::_selectColorFromMouseEvent( QMouseEvent *event )
{
    Debug::Throw() << "ColorGrabObject::_selectColorFromMouseEvent - (" << event->globalX() << "," << event->globalY() << ")" << endl;

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
    emit colorSelected( QColor( image.pixel( 1, 1 ) ) );

    return;
}
