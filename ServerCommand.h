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

#ifndef ServerCommand_h
#define ServerCommand_h

#include <iostream>
#include <list>
#include <map>
#include <string>

#include "ApplicationId.h"
#include "ArgList.h"  
#include "Counter.h"  
#include "Debug.h"  

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
  
    
    //! request accepted command
    static const std::string ACCEPTED;
    
    //! alive command. Is send by a client when it recieved a Raise command 
    static const std::string ALIVE;
  
    //! request denied command
    static const std::string DENIED;
  
    //! force existing client to die
    static const std::string ABORT;
    
    //! request denied command
    static const std::string IDENTIFY;
     
    //! request command
    static const std::string IDENTIFY_SERVER;
    
    //! client killed command
    static const std::string KILLED;
     
    //! raise command
    static const std::string RAISE;
    
    //! request command
    static const std::string REQUEST;
    
    //! unlock command, clear list of registered applications
    static const std::string UNLOCK;
      
    //! constructor
    ServerCommand( const std::string& buffer = "" );
  
    //! constructor
    ServerCommand( 
      const ApplicationId& id,
      const std::string& command = "" ):
      Counter( "ServerCommand" ),
      id_( id ),
      command_( command )
    { Debug::Throw( "ServerCommand::ServerCommand.\n" ); }
  
  
    //! convert to a string
    operator std::string( void ) const;
    
    //! application id
    const ApplicationId& id( void ) const
    { return id_; }
    
    //! application id
    void setId( const ApplicationId& id )
    { id_ = id; }
    
    //! command
    std::string command( void ) const
    { return command_; }
    
    //! command
    void setCommand( const std::string& command )
    { command_ = command; }
    
    //! argument
    void setArguments( const ArgList& args )
    { args_ = args; }
    
    //! arguments
    const ArgList& args( void ) const
    { return args_; }
    
    private:
    
    //! application id
    ApplicationId id_;
    
    //! user name
    std::string user_;
    
    //! command
    std::string command_;
    
    //! arguments
    ArgList args_;
    
    //! separator
    static const std::string separator_;
  
    //! create command from stream
    friend std::istream &operator >> (std::istream &in, ServerCommand &);   
    
    //! dump command to stream
    friend std::ostream &operator << (std::ostream &out,const ServerCommand &);   
  
    //! text conversion pair type
    typedef std::map<std::string, std::string> ConversionMap;
    
    //! text conversions pair list
    static ConversionMap _initConversions( void );
  
    //! text conversion pair list
    static ConversionMap conversions_;
      
  };
};

#endif
