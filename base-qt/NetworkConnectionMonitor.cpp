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

#if !defined(Q_OS_WIN)
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#endif

#include <unistd.h>

//____________________________________________________________
namespace Network
{
    //________________________________________________
    class EncapsulatedSocket
    {
        public:

        //* constructor
        EncapsulatedSocket( int fileDescriptor ):
            fileDescriptor_( fileDescriptor )
        {}

        //* copy constructor
        EncapsulatedSocket( const EncapsulatedSocket& other ) = delete;

        //* assignment operator
        EncapsulatedSocket& operator = ( const EncapsulatedSocket& other ) = delete;

        //* destructor
        ~EncapsulatedSocket( void )
        { if( fileDescriptor_ >= 0 ) ::close( fileDescriptor_ ); }

        int fileDescriptor( void ) const
        { return fileDescriptor_; }

        bool isValid( void ) const
        { return fileDescriptor_ >= 0; }

        private:

        int fileDescriptor_;

    };

    //________________________________________________
    ConnectionMonitor::ConnectionMonitor( QObject* parent ):
        QObject( parent ),
        Counter( "Network::ConnectionMonitor" )
    {}

    //______________________________________________________________________
    void ConnectionMonitor::setEnabled( bool enabled )
    {
        #if !defined(Q_OS_WIN)
        if( enabled == enabled_ ) return;
        enabled_ = enabled;

        if( deviceTimer_.isActive() ) deviceTimer_.stop();
        if( enabled_ && timeOut_ >= 0 ) deviceTimer_.start( 1000*timeOut_, this );
        #endif
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

    //________________________________________________
    ConnectionMonitor::DeviceSet ConnectionMonitor::devices( ConnectionMonitor::DeviceType type )
    {
        Debug::Throw( "Network::ConnectionMonitor::devices.\n" );
        #if defined(Q_OS_WIN)
        return DeviceSet();
        #else
        DeviceSet out;
        for( int index = 1;;++index )
        {

            EncapsulatedSocket socket( ::socket(AF_INET, SOCK_DGRAM, 0) );
            if( !socket.isValid() ) break;

            struct ifreq ifr;
            ifr.ifr_ifindex = index;
            if( ioctl( socket.fileDescriptor(), SIOCGIFNAME, &ifr, sizeof(ifr) ) ) break;
            if( ioctl( socket.fileDescriptor(), SIOCGIFFLAGS, &ifr, sizeof(ifr) ) ) break;

            // skip loopback
            if( ifr.ifr_flags & IFF_LOOPBACK ) continue;

            // check status
            if( type == DeviceType::Connected )
            {

                // check running status
                if( !( ifr.ifr_flags & IFF_RUNNING ) ) continue;

                // check bound address
                if( ioctl( socket.fileDescriptor(), SIOCGIFADDR, &ifr, sizeof(ifr) ) ) continue;

            }

            QString device( ifr.ifr_name );
            Debug::Throw() << "Network::ConnectionMonitor::devices - adding " << device << endl;
            out.insert( device );
        }

        return out;
        #endif
    }

    //________________________________________________
    void ConnectionMonitor::checkDevice( void )
    {

        Debug::Throw( "ConnectionMonitor::checkDevice.\n" );

        // get connected devices
        auto devices( connectedDevices() );

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

    //______________________________________________________________________
    void ConnectionMonitor::timerEvent( QTimerEvent* event )
    {

        if( event->timerId() == deviceTimer_.timerId() ) checkDevice();
        else return QObject::timerEvent( event );

    }

}
