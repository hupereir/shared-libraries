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

#include "BaseCoreApplication.h"
#include "BaseCoreApplication.moc"

#include "ErrorHandler.h"
#include "Debug.h"
#include "XmlOptions.h"

//____________________________________________
BaseCoreApplication::BaseCoreApplication( QObject* parent, CommandLineArguments arguments ) :
    QObject( parent ),
    applicationManager_( 0 ),
    arguments_( arguments ),
    realized_( false )
{

    Debug::Throw( "BaseCoreApplication::BaseCoreApplication.\n" );
    connect( this, SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );

}

//____________________________________________
BaseCoreApplication::~BaseCoreApplication( void )
{

    Debug::Throw( "BaseCoreApplication::~BaseCoreApplication.\n" );
    emit saveConfiguration();
    XmlOptions::write();

    if( _hasApplicationManager() )
    {
        delete applicationManager_;
        applicationManager_ = 0;
    }

    ErrorHandler::get().exit();

}

//____________________________________________
bool BaseCoreApplication::initApplicationManager( void )
{

    Debug::Throw() << "BaseCoreApplication::initApplicationManager - arguments: " << arguments_.join( " " ) << endl;

    // check if already initialized
    if( _hasApplicationManager() ) return true;

    // assign application name to qapplication
    qApp->setApplicationName( applicationName() );

    // parse arguments
    const CommandLineParser parser( commandLineParser( arguments_ ) );
    if( parser.hasFlag( "--help" ) )
    {

        usage();
        return false;

    } else if( parser.hasFlag( "--version" ) ) {

        Debug::Throw(0) << "Qt: " << qVersion() << endl;
        Debug::Throw(0) <<  applicationName() << ": " << applicationVersion();
        const QString timeStamp( buildTimeStamp() );
        if( !timeStamp.isEmpty() ) Debug::Throw(0) << " (" << timeStamp << ")";
        Debug::Throw(0) << "" << endl;
        return false;

    } else if( parser.hasFlag( "--no-server" ) ) {

        realizeWidget();
        return true;

    }

    // create application manager
    applicationManager_ = new SERVER::ApplicationManager( this );
    applicationManager_->setApplicationName( applicationName() );

    // connections
    connect( applicationManager_, SIGNAL(commandRecieved(SERVER::ServerCommand)), SLOT(_processCommand(SERVER::ServerCommand)) );

    // initialization
    applicationManager_->initialize( arguments_ );
    return true;

}

//____________________________________________
bool BaseCoreApplication::realizeWidget( void )
{
    Debug::Throw( "BaseCoreApplication::realizeWidget.\n" );

    //! check if the method has already been called.
    if( realized_ ) return false;
    realized_ = true;
    return true;

}

//_______________________________________________
CommandLineParser BaseCoreApplication::commandLineParser( CommandLineArguments arguments, bool ignoreWarnings ) const
{

    CommandLineParser out( SERVER::ApplicationManager::commandLineParser() );

    out.registerFlag( "--help", QObject::tr( "print this help and exit" ) );
    out.registerFlag( "--version", QObject::tr( "print application version and exits" ) );

    // these are additional flags recognized by Qt.
    // this may be system dependent, and vary from one Qt version to the other,
    // but is not very important. They are listed here only to avoid warnings from the application.
    out.registerOption( "-style", "string", QObject::tr( "Qt widget style" ) );
    out.registerOption( "-graphicssystem", QObject::tr( "string" ), QObject::tr( "Qt drawing backend (raster|opengl)" ) );

    if( !arguments.isEmpty() )
    { out.parse( arguments, ignoreWarnings ); }

    return out;

}

//_______________________________________________
void BaseCoreApplication::_updateConfiguration( void )
{
    Debug::Throw( "BaseCoreApplication::_updateConfiguration.\n" );

    // debug
    Debug::setLevel( XmlOptions::get().get<int>( "DEBUG_LEVEL" ) );

    // emit signal to propagate changes to other widgets
    Debug::Throw( "BaseCoreApplication::_updateConfiguration - done.\n" );

}

//________________________________________________
bool BaseCoreApplication::_processCommand( SERVER::ServerCommand command )
{

    Debug::Throw() << "BaseCoreApplication::_processCommand: " << command.commandName() << endl;
    switch( command.command() )
    {

        case SERVER::ServerCommand::Accepted:
        realizeWidget();
        return true;

        case SERVER::ServerCommand::Abort:
        case SERVER::ServerCommand::Denied:
        qApp->quit();
        return true;

        default:
        break;

    }

    return false;

}
