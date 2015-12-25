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
#include "ImageData.h"

#include "Debug.h"
#include "IconSize.h"

#include <QCoreApplication>

#ifndef QT_NO_DBUS
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusPendingCall>
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
SystemNotifications::SystemNotifications( QObject* parent, const QString& applicationName, const QIcon& icon ):
    QObject( parent ),
    Counter( "SystemNotifications" )
{
    Debug::Throw() << "SystemNotifications::SystemNotifications" << endl;
    if( !icon.isNull() ) setIcon( icon );
}

//____________________________________________
SystemNotifications::~SystemNotifications( void )
{ _showMessageQueue(); }

//____________________________________________
void SystemNotifications::setIcon( const QIcon& icon )
{
    if( icon.isNull() ) imageData_ = Notifications::ImageData();
    else imageData_ = Notifications::ImageData( icon.pixmap( IconSize( IconSize::Maximum ) ).toImage() );
}

//____________________________________________
bool SystemNotifications::isSupported( void )
{
    #ifdef QT_NO_DBUS
    return false;
    #else
    return true;
    // return (qApp && QDBusConnection::sessionBus().isConnected());
    #endif
}

//____________________________________________
void SystemNotifications::processMessage( const QString& summary, const QString& message )
{

    // flush, if summary has changed
    if( !( messageQueue_.empty() || summary == summary_ ) )
    {
        timer_.stop();
        _showMessageQueue();
    }

    // append message and restart timer
    summary_ = summary;
    messageQueue_.append( message );
    if( !timer_.isActive() ) { timer_.start( 100, this ); }
}

//____________________________________________
void SystemNotifications::timerEvent( QTimerEvent* event )
{

    Debug::Throw( "SystemNotifications::timerEvent.\n" );
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        _showMessageQueue();

    } else return QObject::timerEvent( event );

}

//____________________________________________
void SystemNotifications::_showMessageQueue( void )
{
    Debug::Throw( "SystemNotifications::_showMessageQueue.\n" );
    if( messageQueue_.empty() ) return;

    #ifndef QT_NO_DBUS
    QDBusInterface interface(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        QDBusConnection::sessionBus() );

    QString message = messageQueue_.join( "\n" );

    if( imageData_.isValid() )
    {

        if( !typeId_ ) typeId_ = qDBusRegisterMetaType<Notifications::ImageData>();
        QVariantMap hints;
        hints.insert( "image-data", QVariant( typeId_, &imageData_ ) );
        interface.asyncCall( "Notify", "TestNotifications", (uint)0, QString(), summary_, message, QStringList(), hints, -1 );

    } else {

        interface.asyncCall( "Notify", "TestNotifications", (uint)0, QString(), summary_, message, QStringList(), QVariantMap(), -1 );

    }
    #endif
    messageQueue_.clear();

}
