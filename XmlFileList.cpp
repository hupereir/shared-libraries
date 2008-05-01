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
* FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
   \file XmlFileList.cpp
   \brief Xml interface to FileList
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QFile>

#include "Debug.h"
#include "XmlFileList.h"
#include "XmlFileRecord.h"
#include "XmlUtil.h"
#include "XmlError.h"
using namespace std;

//_______________________________________________ 
static const string XML_FILE_LIST = "file_list";

//_______________________________________________
void XmlFileList::setDBFile( const File& file )
{
  Debug::Throw() << "XmlFileList::SetDBFile - file: " << file << endl;

  // store file
  db_file_ = file;
  
  // make a copy of the current list
  FileRecord::List local_list( _records() );
  
  // retrieve list matching db file
  _findList( file );
  
  // read if empty
  if( _records().empty() ) read();
  
  // merge with current files
  for( FileRecord::List::iterator iter = local_list.begin(); iter != local_list.end(); iter++ )
  { _add( *iter ); }
  
  return;
  
}

//_______________________________________________
bool XmlFileList::read( void )
{
  Debug::Throw( "XmlFileList::read.\n" );
  if( db_file_.empty() || !db_file_.exists() ) return false;

  // parse the file
  QFile in( db_file_.c_str() );
  if ( !in.open( QIODevice::ReadOnly ) )
  {
    Debug::Throw( "Options::Read - cannot open file.\n" );
    return false;
  }

  // dom document
  QDomDocument document;
  XmlError error( db_file_ );
  if ( !document.setContent( &in, &error.error(), &error.line(), &error.column() ) ) {
    in.close();
    Debug::Throw() << error << endl;
    return _deprecatedRead();
  }

  QDomElement doc_element = document.documentElement();
  QDomNode node = doc_element.firstChild();
  for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
  {
    QDomElement element = node.toElement();
    if( element.isNull() ) continue;

    // special options
    if( element.tagName() == XmlFileRecord::XML_RECORD ) _add( XmlFileRecord( element ) );
    else Debug::Throw() << "XmlFileList::read - unrecognized tag " << qPrintable( element.tagName() ) << endl;
  }

  return true;
}

//_______________________________________________
bool XmlFileList::write( void )
{
  Debug::Throw( "XmlFileList::Write.\n" );
  if( db_file_.empty() ) 
  {
    Debug::Throw( "XmlFileList::Write - no file.\n" );    
    return false;
  }
  
  // output file
  QFile out( db_file_.c_str() );
  if( !out.open( QIODevice::WriteOnly ) ) return false;

  // truncate list
  if( _maxSize() > 0 && int( _records().size() ) > _maxSize() )
  { _truncateList(); }
    
  // create document
  QDomDocument document;

  // create main element
  QDomElement top = document.appendChild( document.createElement( XML_FILE_LIST.c_str() ) ).toElement();

  // loop over records
  for( FileRecord::List::const_iterator iter = _records().begin(); iter != _records().end(); iter++ )
  { 
    Debug::Throw() << "XmlFileList::Write - " << *iter;
    top.appendChild( XmlFileRecord( *iter ).domElement( document ) ); 
  }

  out.write( document.toByteArray() );
  out.close();
  
  return true;
}

//_______________________________________________
bool XmlFileList::_deprecatedRead( void )
{
  Debug::Throw( "FileList::_deprecatedRead.\n" );
  
  // check file
  if( db_file_.empty() || !db_file_.exists() ) return false; 
  
  // open stream
  QFile in( db_file_.c_str() );
  if( !in.open( QIODevice::ReadOnly ) ) return false;
      
  while( in.canReadLine() ) 
  { 
    QString line( in.readLine( 1024 ) );
    add( File(qPrintable( line ) ) ); 
  }
  
  in.close();
  return true;
}
