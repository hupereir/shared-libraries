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

#include "NetworkConnectionMonitor.h"

#include "Debug.h"

#include <QFile>
#include <QTextStream>

//____________________________________________________________
namespace Network
{
    class Route
    {
        public:

        //* device
        QString device_;

        //* network
        QString network_;

        //* gateway
        QString gateway_;

        //* metric
        int metric_;

        //* validity
        bool valid_;

        // streamer
        friend QTextStream& operator >> ( QTextStream& in, Route& route )
        {
            int dummy;
            in >> route.device_ >> route.network_ >> route.gateway_ >> dummy >> dummy >> dummy >> route.metric_;
            route.valid_ = (in.status() == QTextStream::Ok );
            return in;
        }
    };

    //________________________________________________
    ConnectionMonitor::ConnectionMonitor( QObject* parent ):
        QObject( parent ),
        Counter( "Network::ConnectionMonitor" ),
        routeFileName_( "/proc/net/route" )
    {}

    //______________________________________________________________________
    void ConnectionMonitor::setEnabled( bool enabled )
    {
        if( enabled == enabled_ ) return;
        enabled_ = enabled;

        if( deviceTimer_.isActive() ) deviceTimer_.stop();
        if( enabled_ && timeOut_ >= 0 ) deviceTimer_.start( 1000*timeOut_, this );
    }

    //______________________________________________________________________
    void ConnectionMonitor::setTimeOut( int timeOut )
    {
        if( timeOut == timeOut_ ) return;

        timeOut_ = timeOut;
        if( !enabled_ ) return;

        if( deviceTimer_.isActive() ) deviceTimer_.stop();
        if( timeOut_ >= 0 ) deviceTimer_.start( 1000*timeOut_, this );
    }

    //______________________________________________________________________
    void ConnectionMonitor::timerEvent( QTimerEvent* event )
    {

        if( event->timerId() == deviceTimer_.timerId() ) _checkDevice();
        else return QObject::timerEvent( event );

    }

    //________________________________________________
    ConnectionMonitor::DeviceSet ConnectionMonitor::_connectedDevices( void ) const
    {
        Debug::Throw( "ConnectionMonitor::_connectedDevices.\n" );

        DeviceSet out;

        // parse routes
        QFile in( routeFileName_ );
        if( !in.open( QIODevice::ReadOnly ) ) return out;

        QStringList lines( QString( in.readAll() ).split( "\n" ) );
        for( auto line:lines )
        {

            QTextStream stream( &line );
            Route route;
            stream >> route;
            if( !route.valid_ ) continue;
            Debug::Throw() << "ConnectionMonitor::_connectedDevices - "
                << route.device_ << " "
                << route.network_ << " "
                << route.gateway_ << " "
                << route.metric_
                << endl;

            if( route.device_ == "lo" ) continue;

            // should put more checks
            out.insert( route.device_ );

        }

        return out;

    }

    //________________________________________________
    void ConnectionMonitor::_checkDevice( void )
    {

        Debug::Throw( "ConnectionMonitor::_checkDevice.\n" );

        // get connected devices
        auto devices( _connectedDevices() );

        // if no connected devices is found, clear current
        if( devices.isEmpty() )
        {
            device_ = QString();
            return;
        }

        // check if current device is in list found
        if( !( device_.isEmpty() || devices.find( device_ ) == devices.end() ) )
        { return; }

        // new device is found
        device_ = *devices.begin();
        emit deviceConnected( device_ );

    }

}
