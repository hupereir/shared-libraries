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

namespace Private
{

    // timeout
    static const int constTimeout=5000;

    //____________________________________________
    SystemNotificationsP::SystemNotificationsP( QObject* parent ):
        QObject( parent ),
        Counter( QStringLiteral("SystemNotificationsP") )
    {
        #ifndef QT_NO_DBUS
        qDBusRegisterMetaType<Notifications::ImageData>();
        #endif
    }

    //____________________________________________
    SystemNotificationsP::~SystemNotificationsP() = default;

    //____________________________________________
    void SystemNotificationsP::setApplicationName( const QString& value )
    { applicationName_ = value; }

    //____________________________________________
    void SystemNotificationsP::setApplicationIcon( const QIcon& icon )
    {
        if( icon.isNull() ) imageData_ = Notifications::ImageData();
        else imageData_ = Notifications::ImageData( icon.pixmap( IconSize::get( IconSize::Maximum ) ).toImage() );
    }

    //____________________________________________
    void SystemNotificationsP::initialize()
    {

        Debug::Throw( QStringLiteral("SystemNotificationsP::initialize.\n") );

        if( initialized_ ) return;
        initialized_ = true;

        #ifndef QT_NO_DBUS

        // check session bus
        auto dbus( QDBusConnection::sessionBus() );
        if( !dbus.isConnected() ) return;

        // connections
        dbus.connect( "org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications","NotificationClosed", this, SLOT(_notificationClosed(quint32,quint32)) );
        dbus.connect( "org.freedesktop.Notifications", "/org/freedesktop/Notifications", "org.freedesktop.Notifications","ActionInvoked", this, SLOT(_checkActionInvoked(quint32,QString)) );

        // create new interface
        dbusInterface_.reset( new QDBusInterface(
            "org.freedesktop.Notifications",
            "/org/freedesktop/Notifications",
            "org.freedesktop.Notifications",
            dbus ) );

        #endif

    }

    //____________________________________________
    bool SystemNotificationsP::isSupported() const
    {

        Debug::Throw( QStringLiteral("SystemNotificationsP::isSupported.\n") );
        #ifndef QT_NO_DBUS
        return initialized_ && QDBusConnection::sessionBus().isConnected() && dbusInterface_->isValid();
        #else
        return false;
        #endif

    }

    //____________________________________________
    void SystemNotificationsP::send( Notification notification )
    {

        #ifndef QT_NO_DBUS

        // initialize
        initialize();

        // setup hints
        QVariantMap hints;
        static const QString iconDataString( "icon_data" );
        if( !notification.icon().isNull() )
        {

            hints.insert( iconDataString, QVariant::fromValue( Notifications::ImageData( notification.icon().pixmap( IconSize::get( IconSize::Maximum ) ).toImage() ) ) );

        } else if( imageData_.isValid() ) {

            hints.insert( iconDataString, QVariant::fromValue( imageData_ ) );

        }

        if( notification.flags() & Notification::Transient )
        { hints.insert( "transient", QVariant( true ) ); }

        if( !notification.category().isEmpty() )
        { hints.insert( "category", QVariant( notification.category() ) ); }

        // copy application name
        if( notification.applicationName().isEmpty() ) notification.setApplicationName( applicationName_ );

        // store actions
        lastNotificationActions_ = notification.actionList();

        const auto currentTime( TimeStamp::now() );

        // reuse old id if existing popup is still visible
        quint32 id = 0;
        if( lastNotificationTime_.isValid() && (currentTime.unixTime() - lastNotificationTime_.unixTime()) * 1000 < constTimeout )
        { id = lastNotificationId_; }

        // send
        auto pendingCall = dbusInterface_->asyncCall( "Notify", "Notification", (uint) id,
            notification.applicationName(),
            notification.summary(),
            notification.body(),
            notification.actionList(),
            hints,
            constTimeout );

        auto watcher = new QDBusPendingCallWatcher( pendingCall, this );
        connect( watcher, &QDBusPendingCallWatcher::finished, this, &SystemNotificationsP::_pendingCallFinished);

        #endif

    }

    #ifndef QT_NO_DBUS
    //____________________________________________
    void SystemNotificationsP::_pendingCallFinished( QDBusPendingCallWatcher* watcher )
    {
        QDBusReply<quint32> reply( *watcher );
        if( reply.isValid() )
        {
            notificationIds_.insert( reply.value(), lastNotificationActions_ );
            emit notificationSent( reply.value() );

            lastNotificationId_ = reply.value();
            lastNotificationTime_ = TimeStamp::now();
        }
        lastNotificationActions_.clear();
        watcher->deleteLater();
    }

    //____________________________________________
    void SystemNotificationsP::_notificationClosed( quint32 id, quint32 reason )
    {
        emit notificationClosed( id );
        notificationIds_.remove( id );
    }

    //____________________________________________
    void SystemNotificationsP::_checkActionInvoked( quint32 id, QString key )
    {
        // find matching notification in map, and check action key
        auto iter = notificationIds_.find( id );
        if( iter != notificationIds_.end() && iter.value().contains( key ) )
        { emit actionInvoked( id, key ); }
    }

    #endif

}
