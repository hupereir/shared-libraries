#ifndef NetworkConnectionMonitor_h
#define NetworkConnectionMonitor_h

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

#include "base_qt_export.h"
#include "Counter.h"

#include <QBasicTimer>
#include <QObject>
#include <QSet>
#include <QTimerEvent>

namespace Network
{

    class BASE_QT_EXPORT ConnectionMonitor: public QObject, private Base::Counter<ConnectionMonitor>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ConnectionMonitor( QObject* = nullptr );

        //*@name accessors
        //@{

        enum class DeviceType
        {
            All,
            Connected
        };

        //* list of connected devices
        using DeviceSet = QSet<QString>;
        static DeviceSet connectedDevices()
        { return devices( DeviceType::Connected ); }

        //* list of devices
        static DeviceSet devices( DeviceType = DeviceType::All );

        //@}

        //*@name modifiers
        //@{

        //* enable
        void setEnabled( bool );

        //* time out
        void setTimeOut( int );

        //* check whether current device is active and select one if not
        void checkDevice();

        //@}

        Q_SIGNALS:

        //* emitted when a device is connected
        void deviceConnected( QString );

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        private:

        //* enabled state
        bool enabled_ = false;

        //* timeOut (seconds)
        int timeOut_ = 10;

        //* device timer
        QBasicTimer deviceTimer_;

        //* connected device
        QString device_;

    };

}

#endif
