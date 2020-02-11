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

#include "BaseToolTipWidget.h"
#include "Debug.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QFrame>
#include <QLayout>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QToolTip>

//_______________________________________________________
BaseToolTipWidget::BaseToolTipWidget( QWidget* parent ):
    QWidget( parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint ),
    Counter( QStringLiteral("BaseToolTipWidget") )
{

    Debug::Throw( QStringLiteral("BaseToolTipWidget::BaseToolTipWidget.\n") );
    setAttribute( Qt::WA_TranslucentBackground, true );

    // event filter on parent
    if( parent ) parent->installEventFilter( this );

    // change palete
    setPalette( QToolTip::palette() );
    setBackgroundRole( QPalette::ToolTipBase );
    setForegroundRole( QPalette::ToolTipText );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &BaseToolTipWidget::_updateConfiguration );
    _updateConfiguration();

}

//_____________________________________________
void BaseToolTipWidget::setEnabled( bool value )
{
    Debug::Throw( QStringLiteral("BaseToolTipWidget::setEnabled.\n") );
    if( enabled_ == value ) return;
    enabled_ = value;
    if( !enabled_ )
    {
        if( isVisible() ) hide();
        timer_.stop();
    }

}

//_____________________________________________
void BaseToolTipWidget::setFollowMouse( bool value )
{ followMouse_ = value; }

//_____________________________________________
void BaseToolTipWidget::setDefaultDelay( int value )
{ if( value >= 0 ) defaultDelay_ = value; }

//_____________________________________________
void BaseToolTipWidget::setIndexRect( QRect rect )
{
    rect_ = rect;
    if( isVisible() && !hiddenTimer_.isActive() ) _adjustPosition();
}

//_____________________________________________
void BaseToolTipWidget::setPreferredPosition( Position value )
{
    if( preferredPosition_ == value ) return;
    preferredPosition_ = value;
    if( isVisible() && !hiddenTimer_.isActive() ) _adjustPosition();
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
void BaseToolTipWidget::setVisible( bool visible )
{
    Debug::Throw() << "BaseToolTipWidget::setVisible - visible: " << visible << endl;
    timer_.stop();

    if( !visible )
    {

        if( isVisible() )
        { hiddenTimer_.start( 200, this ); }

    } else {

        // check mouse is still in relevant rect
        if( !_checkMousePosition() ) return;

        // adjust position and show
        _adjustPosition();

    }

    QWidget::setVisible( visible );
}

//_______________________________________________________
void BaseToolTipWidget::showDelayed( int delay )
{
    if( !enabled_ ) return;

    if( hiddenTimer_.isActive() ) hiddenTimer_.stop();
    if( isVisible() ) hide();
    timer_.start( delay >= 0 ? delay:defaultDelay_, this );

    return;

}

//_______________________________________________________
void BaseToolTipWidget::enterEvent( QEvent* )
{ hide(); }

//_______________________________________________________
void BaseToolTipWidget::leaveEvent( QEvent* )
{ hide(); }

//_______________________________________________________
void BaseToolTipWidget::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOptionFrame opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &painter, this );
    QWidget::paintEvent( event );
}

//_______________________________________________________
void BaseToolTipWidget::mousePressEvent( QMouseEvent* event )
{ hide(); }

//_____________________________________________
void BaseToolTipWidget::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        show();

    } else if( event->timerId() == hiddenTimer_.timerId() ) {

        hiddenTimer_.stop();

    } else QWidget::timerEvent( event );
}

//_______________________________________________________
bool BaseToolTipWidget::_checkMousePosition() const
{ return rect_.contains( QCursor::pos() ); }

//_______________________________________________________
void BaseToolTipWidget::_adjustPosition()
{

    // get tooltip size
    adjustSize();
    const QSize size( sizeHint() );

    // desktop size
    const auto desktopGeometry( QtUtil::desktopGeometry( this ) );

    // set geometry
    int top(0);
    int left(0);
    const int margin = 5;
    if( preferredPosition_ == Position::Top || preferredPosition_ == Position::Bottom )
    {
        left = followMouse_ ? QCursor::pos().x():rect_.left() + ( rect_.width() - size.width() )/2;
        left = qMax( left, desktopGeometry.left() );
        left = qMin( left, desktopGeometry.right() - size.width() );

        if( preferredPosition_ == Position::Bottom )
        {

            top = rect_.bottom() + margin;
            if( top > desktopGeometry.bottom() - size.height() ) top = rect_.top() - margin - size.height();

        } else {

            top = rect_.top() - margin - size.height();
            if( top < desktopGeometry.top() ) top = rect_.bottom() + margin;

        }

    } else {

        top = followMouse_ ? QCursor::pos().y():rect_.top() + ( rect_.height() - size.height() )/2;
        top = qMax( top, desktopGeometry.top() );
        top = qMin( top, desktopGeometry.bottom() - size.height() );

        if( preferredPosition_ == Position::Right )
        {

            left = rect_.right()+margin;
            if( left > desktopGeometry.right() - size.width() ) left = rect_.left() - margin - size.width();

        } else {

            left = rect_.left() - margin - size.width();
            if( left < desktopGeometry.left() ) left = rect_.right()+margin;

        }

    }

    move( QPoint( left, top ) );

}

//_____________________________________________
void BaseToolTipWidget::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("BaseToolTipWidget::_updateConfiguration.\n") );
    if( XmlOptions::get().contains( QStringLiteral("SHOW_TOOLTIPS") ) ) setEnabled( XmlOptions::get().get<bool>( QStringLiteral("SHOW_TOOLTIPS") ) );
    if( XmlOptions::get().contains( QStringLiteral("TOOLTIP_DELAY") ) ) setDefaultDelay( XmlOptions::get().get<int>( QStringLiteral("TOOLTIP_DELAY") ) );
}
