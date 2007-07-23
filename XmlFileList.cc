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
   \file XmlFileList.cc
   \brief Xml interface to FileList
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QFile>
#include <fstream>
#include <sstream>

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
  if( db_file_.empty() || !db_file_.exist() ) return false;

  // parse the file
  QFile qtfile( db_file_.c_str() );
  if ( !qtfile.open( QIODevice::ReadOnly ) )
  {
    Debug::Throw( "Options::Read - cannot open file.\n" );
    return false;
  }

  // dom document
  QDomDocument document;
  XmlError error( db_file_ );
  if ( !document.setContent( &qtfile, &error.error(), &error.line(), &error.column() ) ) {
    qtfile.close();
    Debug::Throw() << error << endl;
    return _deprecatedRead();
  }

  QDomElement doc_element = document.documentElement();
  QDomNode node = doc_element.firstChild();
  for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
  {
    QDomElement element = node.toElement();
    if( element.isNull() ) continue;

    string tag_name( qPrintable( element.tagName() ) );

    // special options
    if( tag_name == XmlFileRecord::XML_RECORD ) _add( XmlFileRecord( element ) );
    else Debug::Throw() << "XmlFileList::read - unrecognized tag " << tag_name << endl;
  }

  return true;
}

//_______________________________________________
bool XmlFileList::write( void )
{
  Debug::Throw( "XmlFileList::Write.\n" );
  if( db_file_.empty() ) return false;

  // check stream
  ofstream out( db_file_.c_str() );
  if( !out ) return false;

  // truncate list
  if( _maxSize() > 0 && int( _records().size() ) > _maxSize() )
  { _truncateList(); }
    
  // create document
  QDomDocument document;

  // create main element
  QDomElement top = document.appendChild( document.createElement( XML_FILE_LIST.c_str() ) ).toElement();

  // loop over records
  for( FileRecord::List::const_iterator iter = _records().begin(); iter != _records().end(); iter++ )
  { top.appendChild( XmlFileRecord( *iter ).domElement( document ) ); }

  out << qPrintable( document.toString() );
  out.close();
  return true;
}

//_______________________________________________
bool XmlFileList::_deprecatedRead( void )
{
  Debug::Throw( "FileList::_deprecatedRead.\n" );
  
  // check file
  if( db_file_.empty() || !db_file_.exist() ) return false; 
  
  // open stream
  ifstream in( db_file_.c_str() );
  if( !in ) return false;
    
  char line[ 256 ];
  while( in.getline( line, 256, '\n' ) ) 
  { add( File(line) ); }
  in.close();
  return true;
}
