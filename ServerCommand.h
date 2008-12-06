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

#include <QTextStream>
#include <iostream>
#include <list>
#include <map>
#include <string>

#include "ApplicationId.h"
#include "ArgList.h"  
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
      UNLOCK
    };
      
    //! constructor
    ServerCommand( const QString& buffer = "" );
  
    //! constructor
    ServerCommand( 
      const ApplicationId& id,
      const CommandType& command = NONE ):
      Counter( "ServerCommand" ),
      timestamp_( TimeStamp::now() ),
      id_( id ),
      command_( command )
    { Debug::Throw( "ServerCommand::ServerCommand.\n" ); }
  
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
    bool isChild( const ServerCommand& command ) const
    { return timeStamp() >= command.timeStamp() && id() == command.id(); }
    
    //! convert to a string
    operator QString( void ) const;    
    
    //! time stamp
    const TimeStamp& timeStamp( void ) const
    { return timestamp_; }
    
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
      return command_names_[ command() ];
    }
    
    //! argument
    void setArguments( const ArgList& args )
    { args_ = args; }
    
    //! arguments
    const ArgList& args( void ) const
    { return args_; }

    //! text conversions pair list
    void initializeConversions( void ) const;
    
    private:
    
    //! map command types to names
    void _initializeCommandNames( void ) const;
        
    //! time stamp
    TimeStamp timestamp_;
    
    //! application id
    ApplicationId id_;
    
    //! user name
    QString user_;
    
    //! command
    CommandType command_;
    
    //! arguments
    ArgList args_;
    
    //! separator
    static const QString separator_;    
        
    //! create command from stream
    friend QTextStream &operator >> ( QTextStream &, ServerCommand& );   

    //! dump command to stream
    friend QTextStream & operator<< ( QTextStream &, const ServerCommand& );
  
    //! command names
    typedef std::map<CommandType, QString > CommandMap;
    
    //! command names
    static CommandMap command_names_;
    
    //! text conversion pair type
    typedef std::map<QString, QString> ConversionMap;
      
    //! text conversion pair list
    static ConversionMap conversions_;
      
  };
};

#endif
