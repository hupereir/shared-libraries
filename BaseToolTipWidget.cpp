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
*******************************************************************************/

#include "BaseToolTipWidget.h"
#include "Debug.h"
#include "GridLayout.h"
#include "Singleton.h"
#include "TimeStamp.h"
#include "ToolTipWidgetItem.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFrame>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QToolTip>

//_______________________________________________________
BaseToolTipWidget::BaseToolTipWidget( QWidget* parent ):
    QWidget( parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint ),
    Counter( "BaseToolTipWidget" ),
    enabled_( false ),
    followMouse_( false )
{

    Debug::Throw( "BaseToolTipWidget::BaseToolTipWidget.\n" );
    setAttribute( Qt::WA_TranslucentBackground, true );

    // event filter on parent
    if( parent ) parent->installEventFilter( this );

    // change palete
    setPalette( QToolTip::palette() );
    setBackgroundRole( QPalette::ToolTipBase );
    setForegroundRole( QPalette::ToolTipText );

    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged( void ) ), SLOT( _updateConfiguration( void ) ) );
    _updateConfiguration();

}

//_____________________________________________
void BaseToolTipWidget::setEnabled( bool value )
{
    Debug::Throw( "BaseToolTipWidget::setEnabled.\n" );
    if( enabled_ == value ) return;
    enabled_ = value;
    if( !enabled_ )
    {
        if( isVisible() ) hide();
        timer_.stop();
    }

}

//_______________________________________________________
bool BaseToolTipWidget::eventFilter( QObject* object, QEvent* event )
{

    if( object != parent() ) return QWidget::eventFilter( object, event );
    switch( event->type() )
    {
        case QEvent::Leave:
        case QEvent::HoverLeave:
        hide();
        break;

        default: break;
    }

    return QWidget::eventFilter( object, event );
}

//_______________________________________________________
void BaseToolTipWidget::hide( void )
{
    timer_.stop();
    QWidget::hide();
}

//_______________________________________________________
void BaseToolTipWidget::show( void )
{
    // stop timer
    timer_.stop();

    // check mouse is still in relevant rect
    if( !_checkMousePosition() ) return;

    // adjust position and show
    _adjustPosition();
    QWidget::show();
}

//_______________________________________________________
void BaseToolTipWidget::showDelayed( int delay )
{
    if( !enabled_ ) return;
    if( isVisible() ) hide();
    timer_.start( delay, this );
}

//_______________________________________________________
void BaseToolTipWidget::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOptionFrame opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &painter, this );
    return QWidget::paintEvent( event );
}

//_______________________________________________________
void BaseToolTipWidget::mousePressEvent( QMouseEvent* event )
{

    hide();
    return QWidget::mousePressEvent( event );

}

//_____________________________________________
void BaseToolTipWidget::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        show();
        return;

    } else return QWidget::timerEvent( event );
}

//_______________________________________________________
bool BaseToolTipWidget::_checkMousePosition( void ) const
{ return rect_.contains( QCursor::pos() ); }

//_______________________________________________________
void BaseToolTipWidget::_adjustPosition( void )
{

    // get tooltip size
    const QSize size( sizeHint() );

    // desktop size
    QDesktopWidget* desktop( qApp->desktop() );
    QRect desktopGeometry( desktop->screenGeometry( desktop->screenNumber( parentWidget() ) ) );

    // set geometry
    int left = followMouse_ ?
        QCursor::pos().x():
        rect_.left() + ( rect_.width() - size.width() )/2;

    left = qMax( left, desktopGeometry.left() );
    left = qMin( left, desktopGeometry.right() - size.width() );

    // first try placing widget below item
    const int margin = 5;
    int top = rect_.bottom() + margin;
    if( top > desktopGeometry.bottom() - size.height() ) top = rect_.top() - margin - size.height();

    move( QPoint( left, top ) );

}

//_____________________________________________
void BaseToolTipWidget::_updateConfiguration( void )
{
    Debug::Throw( "BaseToolTipWidget::_updateConfiguration.\n" );
    if( XmlOptions::get().contains( "SHOW_TOOLTIPS" ) ) setEnabled( XmlOptions::get().get<bool>( "SHOW_TOOLTIPS" ) );
}
