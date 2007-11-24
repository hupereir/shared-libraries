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
  \file ErrorHandler.cpp
  \brief Customized Qt error handler. Messages in the disabled_messages_ list
  are skipped.
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "ErrorHandler.h"
#include "Debug.h"

using namespace std;

//_____________________________________________________________
list<string> ErrorHandler::disabled_messages_;
unsigned int ErrorHandler::n_disabled_( 0 );

//_____________________________________________________________
void ErrorHandler::Throw( QtMsgType type, const char* message )
{
  Debug::Throw() << "ErrorHandler::Throw - " << message << endl;
  
  string local_message( message );
  
  // check if message is to be disabled
  bool disabled( false );
  for( list<string>::iterator it=disabled_messages_.begin(); it != disabled_messages_.end(); it++ )
  {
    if( local_message.find( *it ) != string::npos ) {
      disabled = true;
      break;
    }
  }  
    
  // check message type
  switch ( type ) {
    
    case QtDebugMsg: break;
    
    case QtWarningMsg:
    if( !disabled ) cerr << "ErrorHandler::Throw - warning: " << message << endl;
    break;
    
    case QtFatalMsg:
    cerr << "ErrorHandler::Throw - fatal: " << message << endl;
    disabled = false;
    abort();
    break;
      
    default:
    if( !disabled )
    cerr << "ErrorHandler::Throw - unknown: " << message << endl;
    break;
      
  }
  
  if( disabled ) n_disabled_++;
  return;
}
