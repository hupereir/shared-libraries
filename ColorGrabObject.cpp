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
#include "ColorGrabObject.moc"
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
    captureWidget_( 0x0 ),
    mouseDown_( false )
{
    Debug::Throw( "ColorGrabObject::ColorGrabObject.\n" );
    Q_CHECK_PTR( parent );

    connect( parent, SIGNAL(clicked()), SLOT(_grabColor()) );

}

//________________________________________________________
void ColorGrabObject::_grabColor( void )
{

    Debug::Throw( "ColorGrabObject::_grabColor.\n" );

    _clearCapture();
    captureWidget_ = new QDialog( 0, Qt::X11BypassWindowManagerHint );
    captureWidget_->installEventFilter( this );
    captureWidget_->move( -1000, -1000 );
    captureWidget_->setModal( true );
    captureWidget_->show();
    captureWidget_->grabMouse( Qt::CrossCursor );

    #if QT_VERSION >= 0x050000
    // need to explicitely override cursor for Qt5
    qApp->setOverrideCursor( Qt::CrossCursor );
    #endif

}

//_____________________________________________________________
bool ColorGrabObject::eventFilter( QObject* object, QEvent* event )
{

    // check object
    if( object != captureWidget_ ) return false;

    switch( event->type() )
    {

        case QEvent::MouseButtonPress:
        {
            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>( event ) );
            if( mouseEvent->button() == Qt::LeftButton ) mouseDown_ = true;
            return true;
        }

        case QEvent::MouseMove:
        {
            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>( event ) );
            if( mouseDown_ ) _selectColorFromMouseEvent( mouseEvent );
            return true;
        }

        case QEvent::MouseButtonRelease:
        {
            // delete grabber
            _clearCapture();

            #if QT_VERSION >= 0x050000
            // need to explicitely release cursor for Qt5
            qApp->restoreOverrideCursor();
            #endif

            QMouseEvent* mouseEvent( static_cast<QMouseEvent*>( event ) );
            if( mouseEvent->button() == Qt::LeftButton )
            { _selectColorFromMouseEvent( mouseEvent ); }

            return true;

        }

        default: return false;
    }

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

//_________________________________________________________
void ColorGrabObject::_clearCapture( void )
{
    if( captureWidget_ )
    {

        Debug::Throw( "ColorGrabObject::_clearCapture.\n" );

        // release mouse and delete widget
        captureWidget_->releaseMouse();
        captureWidget_->deleteLater();
        captureWidget_ = 0;
    }

    mouseDown_ = false;
}
