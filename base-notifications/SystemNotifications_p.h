#ifndef SystemNotifications_p_h
#define SystemNotifications_p_h

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

#include "Counter.h"
#include "ImageData.h"
#include "Notification.h"
#include "TimeStamp.h"

#include <QIcon>
#include <QObject>
#include <QStringList>
#include <QSet>

#ifndef QT_NO_DBUS
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#endif

#include <memory>

namespace Private
{

    class SystemNotificationsP: public QObject, private Base::Counter<SystemNotificationsP>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit SystemNotificationsP( QObject* = nullptr );

        //* destructor
        ~SystemNotificationsP() override;

        //* @name modifiers
        //@{

        //* initialize
        void initialize();

        //* application name
        void setApplicationName( const QString& );

        //* set icon
        void setApplicationIcon( const QIcon& );

        //* process message
        void send( Notification );

        //@}

        //* @name accessors
        //@{

        //* true if available
        bool isSupported() const;

        //@}

        Q_SIGNALS:

        //* notification sent
        void notificationSent( quint32 );

        //* notification closed
        void notificationClosed( quint32 );

        //* action called
        void actionInvoked( quint32, QString );

        #ifndef QT_NO_DBUS
        private Q_SLOTS:

        //* pending dbus call finished
        void _pendingCallFinished(QDBusPendingCallWatcher*);

        //* notification closed
        void _notificationClosed( quint32, quint32 );

        //* check action invoked
        void _checkActionInvoked( quint32, const QString &);

        #endif

        private:

        //* application name
        QString applicationName_;

        //* initialized
        bool initialized_ = false;

        #ifndef QT_NO_DBUS

        //* dbus interface
        std::unique_ptr<QDBusInterface> dbusInterface_;
        #endif

        //* image data
        Notifications::ImageData imageData_;

        //* notifications and associated actions
        QHash<quint32, QStringList> notificationIds_;

        //* action list
        QStringList lastNotificationActions_;

        //* last notification id
        quint32 lastNotificationId_ = 0;

        //* last notification time
        TimeStamp lastNotificationTime_;

    };

}

#endif
