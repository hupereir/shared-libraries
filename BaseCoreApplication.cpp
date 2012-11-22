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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "ErrorHandler.h"
#include "BaseCoreApplication.h"
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
    connect( this, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

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
    Debug::Throw( "BaseCoreApplication::~BaseCoreApplication - done.\n" );

}

//____________________________________________
void BaseCoreApplication::initApplicationManager( void )
{
    Debug::Throw( "BaseCoreApplication::initApplicationManager.\n" );

    if( _hasApplicationManager() ) return;

    if( SERVER::ApplicationManager::commandLineParser( _arguments() ).hasFlag( "--no-server" ) )
    {
        realizeWidget();
        return;
    }

    // create application manager
    applicationManager_ = new SERVER::ApplicationManager( this );
    applicationManager_->setApplicationName( XmlOptions::get().raw( "APP_NAME" ) );

    // connections
    connect( applicationManager_, SIGNAL( commandRecieved( SERVER::ServerCommand ) ), SLOT( _processCommand( SERVER::ServerCommand ) ) );

    // initialization
    applicationManager_->initialize( arguments_ );
    Debug::Throw( "BaseCoreApplication::initApplicationManager - done.\n" );

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

    switch( command.command() )
    {

        case SERVER::ServerCommand::ACCEPTED:
        realizeWidget();
        return true;

        case SERVER::ServerCommand::ABORT:
        case SERVER::ServerCommand::DENIED:
        qApp->quit();
        return true;

        default:
        break;

    }

    return false;

}
