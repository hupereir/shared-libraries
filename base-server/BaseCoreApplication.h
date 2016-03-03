#ifndef BaseCoreApplication_h
#define BaseCoreApplication_h

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

#include "ApplicationManager.h"
#include "CommandLineArguments.h"
#include "Counter.h"

#include <QObject>

//* Main Window singleton object
class BaseCoreApplication: public QObject
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseCoreApplication( QObject* parent, CommandLineArguments arguments = CommandLineArguments() );

    //* destructor
    virtual ~BaseCoreApplication( void );

    //* initialize application manager
    virtual bool initApplicationManager( void );

    //* create all widgets
    virtual bool realizeWidget( void );

    //*@name application information
    //@{

    //* usage
    virtual void usage( void ) const = 0;

    //* command line parser
    virtual CommandLineParser commandLineParser( CommandLineArguments arguments = CommandLineArguments(), bool ignoreWarnings = true ) const;

    //* application name
    virtual QString applicationName( void ) const = 0;

    // application version
    virtual QString applicationVersion( void ) const = 0;

    //@}

    public Q_SLOTS:

    //* send server command
    virtual void sendServerCommand( Server::ServerCommand::CommandType );

    //* send server command
    virtual void sendServerCommand( Server::ServerCommand );

    Q_SIGNALS:

    //* emitted when configuration needs to be saved
    void saveConfiguration( void );

    //* emitted when configuration is changed
    void configurationChanged( void );

    protected Q_SLOTS:

    //* process command from server
    /** returns true if command has been accepted */
    virtual bool _processCommand( Server::ServerCommand );

    //*

    private Q_SLOTS:

    //* configuration
    void _updateConfiguration( void );

    protected:

    //* formated usage
    void _usage( QString applicationName, QString options = tr("[options]") ) const;

    //* argument list
    void _setArguments( CommandLineArguments arguments )
    { arguments_ = arguments; }

    //* argument list
    CommandLineArguments& _arguments( void )
    { return arguments_; }

    //* argument list
    const CommandLineArguments& _arguments( void ) const
    { return arguments_; }

    //* realized
    bool _realized( void ) const
    { return realized_; }

    //* application manager
    bool _hasApplicationManager( void ) const
    { return (bool) applicationManager_; }

    //* application manager
    Server::ApplicationManager& _applicationManager( void ) const
    { return *applicationManager_; }

    private:

    //* pointer to application manager
    Server::ApplicationManager* applicationManager_ = nullptr;

    //* command line arguments
    CommandLineArguments arguments_;

    //* true when Realized Widget has been called.
    bool realized_ = false;

};

#endif
