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

#ifndef ErrorHandler_h
#define ErrorHandler_h

/*!
  \file ErrorHandler.h
  \brief Customized Qt error handler. Messages in the disabled_messages_ list
  are skipped.
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <iostream>
#include <string>
#include <list>

/*!
  \class ErrorHandler
  \brief Customized Qt error handler. Messages in the disabled_messages_ list
  are skipped.
*/

class ErrorHandler
{
  
  public:
  
  //! check/display Qt error messages
  static void Throw( QtMsgType type, const char* message );
  
  //! adds a disabled message in the list
  static void DisableMessage( const std::string& message ) 
  { disabled_messages_.push_back( message ); }
  
  //! adds a disabled message in the list
  static void ClearDisabledMessages( void ) 
  { disabled_messages_.clear(); }
  
  //! adds a disabled message in the list
  static void Exit( void ) 
  { }
  
  private:
  
  //! all messages containing one of the string in the list are not displayed
  static std::list< std::string > disabled_messages_;
  
  //! number of recieved disabled message
  static unsigned int n_disabled_;
  
};

#endif
