
// $Id$

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
SystemNotifications::SystemNotifications( QObject* parent, const QString& applicationName, const QIcon& icon ):
    QObject( parent ),
    Counter( "SystemNotifications" ),
    notify_( new SystemNotificationsP( applicationName, icon ) )
{ Debug::Throw() << "SystemNotifications::SystemNotifications" << endl; }

//____________________________________________
SystemNotifications::~SystemNotifications( void )
{
    _showMessageQueue();
    delete notify_;
}

//____________________________________________
bool SystemNotifications::isSupported( void )
{
    #if WITH_LIBNOTIFY
    return true;
    #else
    return false;
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

    QString message = messageQueue_.join( "\n" );
    messageQueue_.clear();
    notify_->send( summary_, message );
}
