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
   \file    ApplicationId.cpp
   \brief   application unique Identifier
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "ApplicationId.h"
#include "Debug.h"
#include "ServerXmlDef.h"

using namespace std;
using namespace SERVER;

//____________________________________________________
ApplicationId::ApplicationId( const QString& name, const QString& user, const QString& display ):
  Counter( "ApplicationId" ),
  name_( name ),
  user_( user + QString("@")+display )
{ Debug::Throw( "ApplicationId::ApplicationId.\n" ); }
 
//____________________________________________________
ApplicationId::ApplicationId( const QDomElement& element ):
  Counter( "ApplicationId" )
{
  Debug::Throw( "ApplicationId::ApplicationId (dom).\n" );
  
  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    if( attribute.name() == SERVER_XML::USER ) setUser( attribute.value() );
    else if( attribute.name() == SERVER_XML::NAME ) setName( attribute.value() );
    else Debug::Throw() << "ApplicationId::ApplicationId - unrecognized attribute " << attribute.name() << endl;
  }
  
}

//____________________________________________________
QDomElement ApplicationId::domElement( QDomDocument& document ) const
{
 
  Debug::Throw( "ApplicationId::domElement" );
  QDomElement out( document.createElement( SERVER_XML::ID ) );
  out.setAttribute( SERVER_XML::NAME, name() );
  out.setAttribute( SERVER_XML::USER, user() );
  return out;  
  
}

//____________________________________________________
QString ApplicationId::userName( void ) const
{
  int position( user_.indexOf( "@" ) );
  return (position < 0 ) ? user_ : user_.left(position);
}

//______________________________________________________
QString ApplicationId::display( void ) const
{
  int position( user_.indexOf( "@" ) );
  return (position < 0 ) ? "" : user_.mid(position+1 );
}

