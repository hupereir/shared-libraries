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
    Counter( QStringLiteral("SystemNotifications") )
{

    d = new Private::SystemNotificationsP( this );
    connect( d, &Private::SystemNotificationsP::notificationSent, this, &SystemNotifications::notificationSent );
    connect( d, &Private::SystemNotificationsP::notificationClosed, this, &SystemNotifications::notificationClosed );
    connect( d, &Private::SystemNotificationsP::actionInvoked, this, &SystemNotifications::actionInvoked );

    if( !applicationName.isNull() ) setApplicationName( applicationName );
    if( !applicationIcon.isNull() ) setApplicationIcon( applicationIcon );

}

//____________________________________________
SystemNotifications::~SystemNotifications()
{ _sendPendingNotification(); }

//____________________________________________
void SystemNotifications::setApplicationName( const QString& value )
{ d->setApplicationName( value ); }

//____________________________________________
void SystemNotifications::initialize()
{ d->initialize(); }

//____________________________________________
void SystemNotifications::setApplicationIcon( const QIcon& value )
{ d->setApplicationIcon( value ); }

//____________________________________________
bool SystemNotifications::isSupported() const
{ return d->isSupported(); }

//____________________________________________
void SystemNotifications::sendNotification( const Notification& notification )
{

    // flush, if notifications cannot be merged
    if( !notification_.canMerge( notification ) )
    {

        timer_.stop();
        _sendPendingNotification();
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

    Debug::Throw( QStringLiteral("SystemNotifications::timerEvent.\n") );
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        _sendPendingNotification();

    } else return QObject::timerEvent( event );

}

//____________________________________________
void SystemNotifications::_sendPendingNotification()
{
    Debug::Throw( QStringLiteral("SystemNotifications::_sendPendingNotification.\n") );
    if( notification_.isValid() )
    {
        // send
        d->send( notification_ );

        // clean
        notification_ = Notification();
    }

    return;

}
