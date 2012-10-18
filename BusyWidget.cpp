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

#include "BusyWidget.h"
#include "Debug.h"

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

//__________________________________________________________________________
BusyWidget::BusyWidget( QWidget* parent, Location location ):
    QWidget( parent ),
    Counter( "BusyWidget" ),
    location_( location ),
    radius_( 18 ),
    thickness_( 5.5 ),
    margin_( 10 ),
    angle_( 0 )
{
    Debug::Throw( "BusyWidget::BusyWidget.\n" );

    // setup widget
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setContextMenuPolicy(Qt::NoContextMenu);
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setFixedSize( sizeHint() );
    setOpacity( 200 );

    // setup animation
    animation_ = new QPropertyAnimation( this );
    animation_->setStartValue( 0 );
    animation_->setEndValue( 360 );
    animation_->setPropertyName( "angle" );
    animation_->setTargetObject( this );
    animation_->setEasingCurve( QEasingCurve::Linear );
    animation_->setDuration( 2000 );
    animation_->setLoopCount( -1 );

    // setup painter path
    const QRectF outerBoundingRect( -radius_, -radius_, 2*radius_, 2*radius_ );
    const QRectF innerBoundingRect( outerBoundingRect.adjusted( thickness_, thickness_, -thickness_, -thickness_ ) );
    const QRectF boundingRect( 0, 0, thickness_, thickness_ );

    path_.moveTo( radius_, 0 );
    path_.arcTo( outerBoundingRect, 0, 90 );
    path_.arcTo( boundingRect.translated( -thickness_/2, -radius_ ), 90, 180 );
    path_.arcTo( innerBoundingRect, 90, -90 );
    path_.arcTo( boundingRect.translated( radius_-thickness_, -thickness_/2 ), 180, 180 );

    // setup event filter
    parent->installEventFilter( this );
    _adjustPosition();

}

//_____________________________________________________________
void BusyWidget::setOpacity( int value )
{

    if( opacity_ == value ) return;
    opacity_ = value;

    // update palette
    QPalette palette( this->palette() );
    QColor foreground( palette.color( foregroundRole() ) );
    QColor background( palette.color( backgroundRole() ) );
    foreground.setAlpha( opacity_ );
    background.setAlpha( opacity_ );
    palette.setColor( foregroundRole(), foreground );
    palette.setColor( backgroundRole(), background );
    setPalette( palette );

}

//__________________________________________________________________________
bool BusyWidget::eventFilter( QObject* object, QEvent* event )
{

    switch( event->type() )
    {
        case QEvent::Resize:
        _adjustPosition();
        break;

        default: break;
    }

    return QWidget::eventFilter( object, event );

}

//__________________________________________________________________________
void BusyWidget::start( void )
{
    Debug::Throw( "BusyWidget::start.\n" );
    if( animation_->state() == QAbstractAnimation::Stopped )
    { animation_->start(); }
}

//__________________________________________________________________________
void BusyWidget::stop( void )
{
    Debug::Throw( "BusyWidget::stop.\n" );
    animation_->stop();
}

//__________________________________________________________________________
void BusyWidget::paintEvent( QPaintEvent* event )
{

    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.setRenderHints( QPainter::Antialiasing );

    // indicator
    painter.translate( 0.5+width()/2, 0.5+height()/2 );
    QColor foreground( palette().color( foregroundRole() ) );
    painter.setPen( foreground );

    foreground.setAlpha( foreground.alphaF() * 220 );
    painter.setBrush( foreground );

    painter.rotate( -angle_ );
    painter.drawPath( path_ );

    painter.rotate( 180 );
    painter.drawPath( path_ );

}

//_____________________________________________________________
void BusyWidget::_adjustPosition( void )
{
    QWidget* parent( parentWidget() );
    if( !parent ) return;

    switch( location_ )
    {

        case Center: move( (parent->width()-width())/2, (parent->height()-height())/2 ); break;
        case TopLeft: move( margin_, margin_ ); break;
        case TopRight: move( parent->width()-width()-margin_, margin_ ); break;
        case BottomLeft: move( margin_, parent->height()-height()-margin_ ); break;
        case BottomRight: move( parent->width()-width()-margin_, parent->height()-height()-margin_ ); break;

    }

}
