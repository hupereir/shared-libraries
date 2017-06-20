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

#include "Counter.h"

#include <QBasicTimer>
#include <QObject>
#include <QSet>
#include <QTimerEvent>

namespace Network
{

    class ConnectionMonitor: public QObject, private Base::Counter<ConnectionMonitor>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit ConnectionMonitor( QObject* = nullptr );

        //*@name modifiers
        //@{

        //* enable
        void setEnabled( bool );

        //* time out
        void setTimeOut( int );

        //@}

        Q_SIGNALS:

        //* emmitted when a device is connected
        void deviceConnected( QString );

        protected:

        //* timer event
        void timerEvent( QTimerEvent* ) override;

        private:

        //* list of connected devices
        using DeviceSet = QSet<QString>;
        DeviceSet _connectedDevices( void ) const;

        //* check whether current device is active and select one if not
        void _checkDevice( void );

        //* enabled state
        bool enabled_ = false;

        //* timeOut (seconds)
        int timeOut_ = 10;

        //* device timer
        QBasicTimer deviceTimer_;

        //* route file name, used to get devices list
        QString routeFileName_;

        //* connected device
        QString device_;

    };

}

#endif
