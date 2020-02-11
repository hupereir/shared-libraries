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
#include <QScreen>
#include <QWindow>

//______________________________________________
ColorGrabObject::ColorGrabObject( QAbstractButton* parent ):
    QObject( parent ),
    Counter( QStringLiteral("ColorGrabObject") )
{
    Debug::Throw( QStringLiteral("ColorGrabObject::ColorGrabObject.\n") );
    connect( parent, &QAbstractButton::clicked, this, [this](bool){ _grabColor(); } );
}

//________________________________________________________
void ColorGrabObject::_grabColor()
{

    Debug::Throw( QStringLiteral("ColorGrabObject::_grabColor.\n") );

    _clearCapture();
    captureWidget_.reset( new QDialog( nullptr, Qt::X11BypassWindowManagerHint ) );
    captureWidget_->installEventFilter( this );
    captureWidget_->move( -1000, -1000 );
    captureWidget_->setModal( true );
    captureWidget_->show();
    captureWidget_->grabMouse( Qt::CrossCursor );

    // need to explicitely override cursor for Qt5
    qApp->setOverrideCursor( Qt::CrossCursor );

}

//_____________________________________________________________
bool ColorGrabObject::eventFilter( QObject* object, QEvent* event )
{

    // check object
    if( object != captureWidget_.get() ) return false;

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

            // need to explicitely release cursor for Qt5
            qApp->restoreOverrideCursor();

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
    QPoint globalPosition( event->globalPos() );

    const auto dpiRatio( QGuiApplication::primaryScreen()->devicePixelRatio() );
    globalPosition.rx()*=dpiRatio;
    globalPosition.ry()*=dpiRatio;
    QImage image( QGuiApplication::primaryScreen()->grabWindow(QApplication::desktop()->winId(), globalPosition.x(), globalPosition.y(), 2, 2 ).toImage() );

    // ensure image is deep enough
    if(image.depth() != 32) image = image.convertToFormat(QImage::Format_RGB32);

    // assign color to the selection frame
    emit colorSelected( QColor( image.pixel( 1, 1 ) ) );

    return;
}

//_________________________________________________________
void ColorGrabObject::_clearCapture()
{
    captureWidget_.reset();
    mouseDown_ = false;
}
