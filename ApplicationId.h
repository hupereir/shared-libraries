#ifndef ApplicationId_h
#define ApplicationId_h

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
   \file    ApplicationId.h
   \brief   application unique Identifier
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <iostream>
#include <string>

#include "Counter.h"

namespace SERVER
{
  

  /*! \brief
    application identifier. 
    Stores application name and user
    to uniquely identify an application.
  */
  class ApplicationId:public Counter
  {
    public:
    
    //! constructor
    ApplicationId( 
      const std::string& name = "", 
      const std::string& user = "",
      const std::string& display = "" ):
      Counter( "ApplicationId" ),
      name_( name ),
      user_( user + std::string("@")+display )
    {};
      
    //! name
    const std::string& name( void ) const
    { return name_; }
    
    //! name 
    void setName( const std::string& value )
    { name_ = value; }

    //! short name
    std::string userName( void ) const
    {
      unsigned int position( user_.find( "@" ) );
      return (position == std::string::npos ) ? user_ : user_.substr(0, position);
    }

    //! short name
    std::string display( void ) const
    {
      unsigned int position( user_.find( "@" ) );
      return (position == std::string::npos ) ? "" : user_.substr(position+1, user_.size()-position-1);
    }
    
    //! user
    const std::string& user( void ) const
    { return user_; }

    //! user
    void setUser( const std::string& value )
    { user_ = value; }
      
    //! equal to operator
    bool operator == (const ApplicationId& app ) const
    { 
      return ( 
        name() == app.name() &&
        user() == app.user() );
    }
  
    //! lower than to operator
    bool operator < (const ApplicationId& app ) const
    { 
      if ( name() != app.name() ) return name() < app.name();
      else if( user() != app.user() ) return user() < app.user();
      return false;
    }
  
    //! returns true if user and name makes sense
    bool isValid( void ) const
    { return !(name().empty() || user().empty() ); }
    
    private:
    
    //! application name
    std::string name_;
    
    //! application user
    std::string user_;
    
    //! streamer
    friend std::ostream& operator << ( std::ostream& out, const ApplicationId& id )
    { 
      out << id.name() << " (" << id.user() << ")";
      return out;
    }
   
  };
};

#endif
