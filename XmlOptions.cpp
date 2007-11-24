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
  \file XmlOptions.cpp
  \brief Option file parser based on xml
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QDomDocument>
#include <QDomElement>
#include <QFile>

#include "XmlOption.h"
#include "XmlOptions.h"

using namespace std;

//____________________________________________________________________
File XmlOptions::file_;
Options XmlOptions::singleton_;
XmlError XmlOptions::error_;

//____________________________________________________________________
bool XmlOptions::read( File file )
{
  
   Debug::Throw() << "XmlOptions::read - file=\"" << file << "\"\n";

  // check filename is valid
  if( !file.size() ) file = file_;
  if( !file.size() ) return false;

  // store requested file
  file_ = file;

  // parse the file
  QFile qtfile( file.c_str() );
  if ( !qtfile.open( QIODevice::ReadOnly ) )
  {
    Debug::Throw( "XmlOptions::read - cannot open file.\n" );
    return false;
  }

  // dom document
  QDomDocument document;
  error_ = XmlError( file );
  if ( !document.setContent( &qtfile, &error_.error(), &error_.line(), &error_.column() ) ) 
  {
    qtfile.close();
    return false;
  }

  QDomElement doc_element = document.documentElement();
  QDomNode node = doc_element.firstChild();
  for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
  {
    QDomElement element = node.toElement();
    if( element.isNull() ) continue;

    string tag_name( qPrintable( element.tagName() ) );

    // special options
    if( tag_name == OPTIONS::SPECIAL_OPTION )
    {
      Debug::Throw( "XmlOptions::read - special options" );

      // retrieve Value attribute
      string value( qPrintable( element.attribute( OPTIONS::VALUE.c_str() ) ) );
      if( value.size() ) get().keep( value );
    } else get().add( XmlOption( element ), true );

  }


  return true;

}

//________________________________________________
bool XmlOptions::write( File file )
{
  
  Debug::Throw( "XmlOptions::write.\n" );

  // check filename is valid
  if( file.empty() ) file = file_;
  if( file.empty() ) return false;
  
  // output file
  QFile out( file.c_str() );
  if( !out.open( QIODevice::WriteOnly ) ) return false;

  // create document
  QDomDocument document;

  // create main element
  QDomElement top = document.appendChild( document.createElement( OPTIONS::OPTIONS.c_str() ) ).toElement();

  // write list of special option names
  for( Options::SpecialOptionMap::const_iterator iter = get().specialOptions().begin(); iter != get().specialOptions().end(); iter++ )
  {
    // check size of options
    if( iter->second.empty() ) continue;
    
    // dump option name
    QDomElement element = document.createElement( OPTIONS::SPECIAL_OPTION.c_str() );
    element.setAttribute( OPTIONS::VALUE.c_str(), iter->first.c_str() );
    top.appendChild( element );
  }

  // write special options
  for( Options::SpecialOptionMap::const_iterator iter = get().specialOptions().begin(); iter != get().specialOptions().end(); iter++ )
  {
    Options::OptionList option_list( iter->second );
    for( Options::OptionList::iterator list_iter = option_list.begin(); list_iter != option_list.end(); list_iter++ )
    if( list_iter->isRecordable() && list_iter->set() && list_iter->raw().size() )
    top.appendChild( XmlOption( *list_iter ).domElement( document ) );
  }

  // write standard options
  for( Options::OptionMap::const_iterator iter = get().map().begin(); iter != get().map().end(); iter++ )
  if( iter->second.isRecordable() && iter->second.set() && iter->second.raw().size() )
  top.appendChild( XmlOption( iter->second ).domElement( document ) );

  out.write( document.toByteArray() );
  out.close();

  return true;

}
