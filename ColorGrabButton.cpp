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

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>

//______________________________________________
ColorGrabButton::ColorGrabButton( QWidget* parent ):
    QToolButton( parent ),
    Counter( "ColorGrabButton" ),
    locked_( false ),
    mouseDown_( false ),
    grabber_( 0x0 )
{
    Debug::Throw( "ColorGrabButton::ColorGrabButton.\n" );
    connect( this, SIGNAL( clicked( void ) ), SLOT( _grabColor( void ) ) );
}


//________________________________________________________
void ColorGrabButton::_grabColor( void )
{

    Debug::Throw( "ColorGrabButton::_grabColor.\n" );
    grabber_ = new QDialog( 0, Qt::X11BypassWindowManagerHint );
    grabber_->move( -1000, -1000 );
    grabber_->setModal( true );
    grabber_->show();
    grabber_->grabMouse( Qt::CrossCursor );
    grabber_->installEventFilter( this );

    locked_ = true;

}

//_____________________________________________________________
bool ColorGrabButton::eventFilter( QObject* object, QEvent* event )
{
    if( object != grabber_ ) return false;
    switch( event->type() )
    {

        case QEvent::MouseButtonPress:
        filterMouseButtonPressEvent( static_cast<QMouseEvent*>( event ) );
        return true;

        case QEvent::MouseButtonRelease:
        filterMouseButtonReleaseEvent( static_cast<QMouseEvent*>( event ) );
        return true;

        case QEvent::MouseMove:
        filterMouseMoveEvent( static_cast<QMouseEvent*>( event ) );
        return true;

        default: return false;

    }

}

//_____________________________________________________________
void ColorGrabButton::filterMouseButtonPressEvent( QMouseEvent *event )
{
    Debug::Throw( "ColorGrabButton::filterMouseButtonPressEvent.\n" );
    if( event->button() == Qt::LeftButton && locked_ && !mouseDown_ )
    { mouseDown_ = true; }
}

//_____________________________________________________________
void ColorGrabButton::filterMouseButtonReleaseEvent( QMouseEvent *event )
{

    Debug::Throw( "ColorGrabButton::filterMouseButtonReleaseEvent.\n" );

    // do nothing if mouse is not locked
    if( !locked_ ) return;

    // check button
    if( event->button() == Qt::LeftButton && mouseDown_ )
    { _selectColorFromMouseEvent( event ); }

    mouseDown_ = false;
    locked_ = false;
    grabber_->deleteLater();
    grabber_ = 0;

}

//_____________________________________________________________
void ColorGrabButton::filterMouseMoveEvent( QMouseEvent *event )
{
    Debug::Throw( "ColorGrabButton::filterMouseMoveEvent.\n" );
    if( locked_ && mouseDown_ ) { _selectColorFromMouseEvent( event ); }
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
