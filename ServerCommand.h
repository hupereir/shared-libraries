#ifndef ServerCommand_h
#define ServerCommand_h

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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License        
* for more details.                     
*                          
* You should have received a copy of the GNU General Public License along with 
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     
* Place, Suite 330, Boston, MA  02111-1307 USA                           
*                         
*                         
*******************************************************************************/

/*!
   \file    ServerCommand.h
   \brief   formated command for interprocess communication
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QDomElement>
#include <QDomDocument>
#include <QString>
#include <QTextStream>

#include <list>
#include <map>

#include "ApplicationId.h"
#include "CommandLineArguments.h"  
#include "Counter.h"  
#include "Debug.h"  
#include "TimeStamp.h"  

namespace SERVER
{
  
  //_________________________________________
  /*! \brief 
    command to be send over the server
    command must be formated APP_NAME::USER::COMMAND::ARG[0]::ARG[1] ...
  */
  class ServerCommand:public Counter
  {
  
    public:
  
    //! command type
    enum CommandType
    {
      NONE,
      ACCEPTED,
      ALIVE,
      DENIED,
      ABORT,
      IDENTIFY,
      IDENTIFY_SERVER,
      KILLED,
      RAISE,
      REQUEST,
      UNLOCK,
      GEOMETRY_REQUEST,
      GEOMETRY
    };
      
    //! list
    typedef std::list<ServerCommand> List;
      
    //! constructor
    ServerCommand( const ApplicationId& id = ApplicationId(), const CommandType& command = NONE );
  
    //! constructor
    ServerCommand( const QDomElement& );

    //! dom element
    QDomElement domElement( QDomDocument& ) const;
    
    //! equal-to operator 
    bool operator == ( const ServerCommand& command ) const
    { 
      return timeStamp() == command.timeStamp() &&
        id() == command.id() &&
        ServerCommand::command() == command.command();
    }
    
    //! less-than operator
    bool operator < ( const ServerCommand& command ) const
    {
      if( timeStamp() != command.timeStamp() ) return timeStamp() < command.timeStamp();
      if( id() != command.id() ) return id() < command.id();
      
      // to order command, place IDENTIFY_SERVER first
      if( ServerCommand::command() != command.command() ) return 
        ( ServerCommand::command() == IDENTIFY_SERVER || ServerCommand::command() < command.command() );
      return false;
    }
    
    //! true if this command is a child of argument
    /*! this is needed to have commands appear in Tree view */
    bool isChild( const ServerCommand& command ) const
    { return timeStamp() >= command.timeStamp() && id() == command.id(); }
    
    //! time stamp
    const TimeStamp& timeStamp( void ) const
    { return timestamp_; }
    
    //! client id
    const unsigned int& clientId( void ) const
    { return client_id_; }
    
    //! client id
    ServerCommand& setClientId( unsigned int value )
    { 
      client_id_ = value;
      return *this;
    }
    
    //! application id
    const ApplicationId& id( void ) const
    { return id_; }
    
    //! application id
    void setId( const ApplicationId& id )
    { id_ = id; }
    
    //! command
    CommandType command( void ) const
    { return command_; }
    
    //! command
    void setCommand( const CommandType& command )
    { command_ = command; }
    
    //! command name
    QString commandName( void ) const
    {
      _initializeCommandNames();
      return _commandNames()[ command() ];
    }
    
    //! argument
    void setArguments( const CommandLineArguments& arguments )
    { arguments_ = arguments; }
    
    //! arguments
    const CommandLineArguments& arguments( void ) const
    { return arguments_; }

    private:
    
    //! command names
    typedef std::map<CommandType, QString > CommandMap;
    
    //! command names
    static CommandMap& _commandNames( void );

    //! map command types to names
    void _initializeCommandNames( void ) const;
        
    //! time stamp
    TimeStamp timestamp_;
    
    //! client id
    unsigned int client_id_;
    
    //! application id
    ApplicationId id_;
    
    //! command
    CommandType command_;
    
    //! arguments
    CommandLineArguments arguments_;
        
  };
};

#endif
