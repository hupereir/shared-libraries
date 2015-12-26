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

#include "SystemNotifications_p.h"

#include "Debug.h"
#include "IconSize.h"

#ifndef QT_NO_DBUS
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCall>
#include <QDBusReply>
#endif

#ifndef QT_NO_DBUS

//____________________________________________________________
QDBusArgument &operator << (QDBusArgument &argument, const Notifications::ImageData &imageData)
{
    argument.beginStructure();
    argument
        << imageData.width
        << imageData.height
        << imageData.rowStride
        << imageData.hasAlpha
        << imageData.bitsPerSample
        << imageData.channels
        << imageData.data;
    argument.endStructure();
    return argument;
}

//____________________________________________________________
const QDBusArgument & operator >>(const QDBusArgument &argument, Notifications::ImageData &imageData)
{
    argument.beginStructure();
    argument
        >> imageData.width
        >> imageData.height
        >> imageData.rowStride
        >> imageData.hasAlpha
        >> imageData.bitsPerSample
        >> imageData.channels
        >> imageData.data;
    argument.endStructure();
    return argument;
}
#endif

//____________________________________________
SystemNotificationsP::SystemNotificationsP( QObject* parent ):
    QObject( parent ),
    Counter( "SystemNotificationsP" )
{}

//____________________________________________
void SystemNotificationsP::setApplicationName( const QString& value )
{ applicationName_ = value; }

//____________________________________________
void SystemNotificationsP::setApplicationIcon( const QIcon& icon )
{
    if( icon.isNull() ) imageData_ = Notifications::ImageData();
    else imageData_ = Notifications::ImageData( icon.pixmap( IconSize( IconSize::Maximum ) ).toImage() );
}

//____________________________________________
void SystemNotificationsP::send( const QString& summary, const QString& message )
{

    #ifndef QT_NO_DBUS
    QDBusConnection dbus( QDBusConnection::sessionBus() );
    QDBusInterface interface(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        dbus );

    if( !initialized_ )
    {
        dbus.connect( "org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications","NotificationClosed", this, SLOT(_notificationClosed(quint32,quint32)) );
        dbus.connect( "org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications","ActionInvoked", this, SLOT(_checkActionInvoked(quint32,QString)) );
        initialized_ = true;
    }

    QDBusPendingCallWatcher* watcher = nullptr;
    if( imageData_.isValid() )
    {

        if( !typeId_ ) typeId_ = qDBusRegisterMetaType<Notifications::ImageData>();
        QVariantMap hints;
        hints.insert( "image-data", QVariant( typeId_, &imageData_ ) );

        QDBusPendingCall pendingCall = interface.asyncCall( "Notify", "TestNotifications", (uint)0, applicationName_, summary, message, actions_, hints, -1 );
        watcher = new QDBusPendingCallWatcher( pendingCall, this );

    } else {

        QDBusPendingCall pendingCall = interface.asyncCall( "Notify", "TestNotifications", (uint)0, applicationName_, summary, message, actions_, QVariantMap(), -1 );
        watcher = new QDBusPendingCallWatcher( pendingCall, this );
    }

    connect( watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), this, SLOT(_pendingCallFinished(QDBusPendingCallWatcher*)));

    #endif

}

#ifndef QT_NO_DBUS
//____________________________________________
void SystemNotificationsP::_pendingCallFinished( QDBusPendingCallWatcher* watcher )
{
    QDBusReply<quint32> reply( *watcher );
    if( reply.isValid() ) notificationIds_.insert( reply.value() );
    watcher->deleteLater();
}

//____________________________________________
void SystemNotificationsP::_notificationClosed( quint32 id, quint32 reason )
{ notificationIds_.remove( id ); }


//____________________________________________
void SystemNotificationsP::_checkActionInvoked( quint32 id, QString key )
{
    if( notificationIds_.contains( id ) && actions_.contains( key ) )
    { emit actionInvoked( id, key ); }
}

#endif
