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

#include "BaseCoreApplication.h"

#include "Debug.h"
#include "ErrorHandler.h"
#include "InterruptionHandler.h"
#include "XmlOptions.h"

#include <signal.h>

//____________________________________________
BaseCoreApplication::BaseCoreApplication( QObject* parent, CommandLineArguments arguments ) :
    QObject( parent ),
    arguments_( arguments )
{

    Debug::Throw( "BaseCoreApplication::BaseCoreApplication.\n" );

    // install interuption handler
    InterruptionHandler::initialize();

    // install error handler
    ErrorHandler::initialize();

    // configuration
    connect( this, &BaseCoreApplication::configurationChanged, this, &BaseCoreApplication::_updateConfiguration );

}

//____________________________________________
BaseCoreApplication::~BaseCoreApplication()
{
    Debug::Throw( "BaseCoreApplication::~BaseCoreApplication.\n" );
    emit saveConfiguration();
    XmlOptions::write();
    ErrorHandler::get().exit();
}

//____________________________________________
bool BaseCoreApplication::initApplicationManager()
{

    Debug::Throw() << "BaseCoreApplication::initApplicationManager - arguments: " << arguments_.get().join( " " ) << endl;

    // check if already initialized
    if( _hasApplicationManager() ) return true;

    // assign application name to qapplication
    qApp->setApplicationName( applicationName() );

    // parse arguments
    auto parser( commandLineParser( arguments_ ) );
    if( parser.hasFlag( "--help" ) )
    {

        usage();
        return false;

    } else if( parser.hasFlag( "--version" ) ) {

        QTextStream(stdout)
            << "Qt: " << qVersion() << endl
            <<  applicationName() << ": " << applicationVersion() << endl;
        return false;

    } else if( parser.hasFlag( "--no-server" ) ) {

        realizeWidget();
        return true;

    }

    // create application manager
    applicationManager_.reset( new Server::ApplicationManager( this ) );
    applicationManager_->setApplicationName( applicationName() );

    // connections
    connect( applicationManager_.get(), &Server::ApplicationManager::commandRecieved, this, &BaseCoreApplication::_processCommand );

    // initialization
    applicationManager_->initialize( arguments_ );
    return true;

}

//____________________________________________
bool BaseCoreApplication::realizeWidget()
{
    Debug::Throw( "BaseCoreApplication::realizeWidget.\n" );

    //* check if the method has already been called.
    if( realized_ ) return false;
    realized_ = true;
    return true;

}

//_______________________________________________
CommandLineParser BaseCoreApplication::commandLineParser( CommandLineArguments arguments, bool ignoreWarnings ) const
{

    Debug::Throw() << "BaseCoreApplication::commandLineParser" << endl;

    CommandLineParser out( Server::ApplicationManager::commandLineParser() );

    out.setGroup( CommandLineParser::applicationGroupName );
    out.registerFlag( CommandLineParser::Tag( "--help", "-h" ), QObject::tr( "print this help and exit" ) );
    out.registerFlag( CommandLineParser::Tag( "--version", "-v" ), QObject::tr( "print application version and exits" ) );

    if( !arguments.isEmpty() )
    { out.parse( arguments, ignoreWarnings ); }

    return out;

}

//__________________________________________________________________
void BaseCoreApplication::sendServerCommand( Server::ServerCommand::CommandType type )
{
    Debug::Throw( "BaseCoreApplication::sendServerCommand.\n" );
    if( _hasApplicationManager() ) sendServerCommand( Server::ServerCommand( _applicationManager().id(), type ) );
}

//__________________________________________________________________
void BaseCoreApplication::sendServerCommand( Server::ServerCommand command )
{
    Debug::Throw( "BaseCoreApplication::sendServerCommand.\n" );
    if( _hasApplicationManager() ) _applicationManager().client().sendCommand( command );
}

//________________________________________________
bool BaseCoreApplication::_processCommand( Server::ServerCommand command )
{

    Debug::Throw() << "BaseCoreApplication::_processCommand: " << command.commandName() << endl;
    switch( command.command() )
    {

        case Server::ServerCommand::CommandType::Accepted:
        realizeWidget();
        return true;

        case Server::ServerCommand::CommandType::Abort:
        case Server::ServerCommand::CommandType::Denied:
        qApp->quit();
        return true;

        default:
        break;

    }

    return false;

}

//_______________________________________________
void BaseCoreApplication::_updateConfiguration()
{
    Debug::Throw( "BaseCoreApplication::_updateConfiguration.\n" );

    // debug
    Debug::setLevel( XmlOptions::get().get<int>( "DEBUG_LEVEL" ) );

    // emit signal to propagate changes to other widgets
    Debug::Throw( "BaseCoreApplication::_updateConfiguration - done.\n" );

}

//_______________________________________________
void BaseCoreApplication::_usage( QString application, QString options ) const
{
    QTextStream( stdout ) << tr( "Usage: " ) << endl;
    QTextStream( stdout ) << "  " << application << " " << options << endl;
}
