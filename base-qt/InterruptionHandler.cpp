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

#include "InterruptionHandler.h"
#include "Debug.h"

#include <QApplication>

#include <csignal>
#include <cstring>

//_____________________________________________________________
InterruptionHandler::InterruptionHandler()
{}

//_____________________________________________________________
InterruptionHandler& InterruptionHandler::get()
{
    static  InterruptionHandler singleton;
    return singleton;
}

//_____________________________________________________________
void InterruptionHandler::setInterruptionHandlerFunction( InterruptionHandlerFuntion function )
{ if( !initialized_ ) function_ = function; }

//_____________________________________________________________
void InterruptionHandler::initialize()
{
    if( get().initialized_ ) return;
    if( !get().function_ ) return;

    get().initialized_ = true;

    // setup interruptions
    #if !defined(Q_OS_WIN)
    struct sigaction interruptAction;
    memset (&interruptAction, '\0', sizeof(interruptAction));
    interruptAction.sa_handler = get().function_;
    interruptAction.sa_flags = 0;

    sigaction( SIGINT, &interruptAction, nullptr );
    sigaction( SIGTERM, &interruptAction, nullptr );
    #else
    signal(SIGINT,  get().function_);
    signal(SIGTERM, get().function_);
    #endif
}

//_____________________________________________________________
void InterruptionHandler::_handleInterruption( int signal )
{
    #if !defined(Q_OS_WIN)
    Debug::Throw(0) << "InterruptionHandler::_handleInterruption - recieved " << strsignal( signal ) << " (" << signal << ")" << Qt::endl;
    #endif
    qApp->quit();
}
