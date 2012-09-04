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

#include "ToolTipManager.h"

#include "Debug.h"
#include "Singleton.h"
#include "ToolTipWidget.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

//_____________________________________________
ToolTipManager::ToolTipManager( QWidget* parent ):
    QObject( parent ),
    Counter( "ToolTipManager" ),
    enabled_( false ),
    widget_( 0x0 )
{
    Debug::Throw( "ToolTipManager::ToolTipManager.\n" );

    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged( void ) ), SLOT( _updateConfiguration( void ) ) );
    _updateConfiguration();
}

//_____________________________________________
void ToolTipManager::setEnabled( bool value )
{
    Debug::Throw( "ToolTipManager::setEnabled.\n" );
    if( enabled_ == value ) return;
    enabled_ = value;
    if( !enabled_ )
    {
        if( widget_ && widget_->isVisible() ) widget_->hide();
        timer_.stop();
    }

}

//_____________________________________________
void ToolTipManager::show( const BaseFileInfo& fileInfo, const QIcon& icon, const QRect& rect )
{
    Debug::Throw( "ToolTipManager::show.\n" );

    if( !enabled_ ) return;

    // create widget and assign
    if( !widget_ ) widget_ = new ToolTipWidget( qobject_cast<QWidget*>( parent() ) );
    widget_->setBaseFileInfo( fileInfo, icon );

    // get tooltip size
    const QSize size( widget_->sizeHint() );

    // desktop size
    QDesktopWidget* desktop( qApp->desktop() );
    QRect desktopGeometry( desktop->screenGeometry( desktop->screenNumber( widget_ ) ) );

    // set geometry
    int left = rect.left() + ( rect.width() - size.width() )/2;
    left = qMax( left, desktopGeometry.left() );
    left = qMin( left, desktopGeometry.right() - size.width() );

    // first try placing widget below item
    const int margin = 5;
    int top = rect.bottom() + margin;
    if( top > desktopGeometry.bottom() - size.height() ) top = rect.top() - margin - size.height();

    widget_->move( QPoint( left, top ) );

    // start timer
    if( widget_->isVisible() ) widget_->hide();
    timer_.start( 500, this );

}

//_____________________________________________
void ToolTipManager::hide( void )
{

    Debug::Throw( "ToolTipManager::hide.\n" );
    if( !enabled_ ) return;

    if( widget_ && widget_->isVisible() ) widget_->hide();
    timer_.stop();
}

//_____________________________________________
void ToolTipManager::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        widget_->show();
        return;

    } else return QObject::timerEvent( event );
}

//_____________________________________________
void ToolTipManager::_updateConfiguration( void )
{
    Debug::Throw( "ToolTipManager::_updateConfiguration.\n" );
    setEnabled( XmlOptions::get().get<bool>( "SHOW_TOOLTIPS" ) );
}
