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

#include "SystemNotifications.h"
#include "SystemNotifications_p.h"

#include "Debug.h"

//____________________________________________
SystemNotifications::SystemNotifications( QObject* parent, const QString& applicationName, const QIcon& applicationIcon ):
    QObject( parent ),
    Counter( "SystemNotifications" )
{

    d_ = new SystemNotificationsP( this );
    connect( d_, SIGNAL(actionInvoked(quint32,QString)), this, SIGNAL(actionInvoked(quint32,QString)) );

    if( !applicationName.isNull() ) setApplicationName( applicationName );
    if( !applicationIcon.isNull() ) setApplicationIcon( applicationIcon );

}

//____________________________________________
SystemNotifications::~SystemNotifications( void )
{ _sendNotification(); }

//____________________________________________
void SystemNotifications::setApplicationName( const QString& value )
{ d_->setApplicationName( value ); }

//____________________________________________
void SystemNotifications::setApplicationIcon( const QIcon& value )
{ d_->setApplicationIcon( value ); }


//____________________________________________
void SystemNotifications::addAction( const QString& key, const QString& name )
{ d_->addAction( key, name ); }

//____________________________________________
void SystemNotifications::setActions( const QStringList& actions )
{ d_->setActions( actions ); }

//____________________________________________
void SystemNotifications::clearActions( void )
{ d_->clearActions(); }

//____________________________________________
bool SystemNotifications::isSupported( void )
{
    #ifdef QT_NO_DBUS
    return false;
    #else
    return true;
    #endif
}

//____________________________________________
void SystemNotifications::processNotification( const Notification& notification )
{

    // flush, if notifications cannot be merged
    if( !notification_.canMerge( notification ) )
    {

        timer_.stop();
        _sendNotification();
        notification_ = notification;

    } else {

        // append to current
        notification_.merge( notification );

    }

    // restart timer
    if( !timer_.isActive() ) { timer_.start( 100, this ); }

}

//____________________________________________
void SystemNotifications::timerEvent( QTimerEvent* event )
{

    Debug::Throw( "SystemNotifications::timerEvent.\n" );
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        _sendNotification();

    } else return QObject::timerEvent( event );

}

//____________________________________________
void SystemNotifications::_sendNotification( void )
{
    Debug::Throw( "SystemNotifications::_sendNotification.\n" );
    if( notification_.isValid() )
    {
        // send
        d_->send( notification_ );

        // clean
        notification_ = Notification();
    }

    return;

}
