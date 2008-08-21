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

#include <QApplication>
#include <QFile>

#include "Debug.h"
#include "XmlFileList.h"
#include "XmlFileRecord.h"
#include "XmlOptions.h"
#include "XmlUtil.h"
#include "XmlError.h"

using namespace std;

//_______________________________________________ 
static const string XML_FILE_LIST = "file_list";

//_______________________________________________ 
XmlFileList::XmlFileList( QObject* parent ):
  FileList( parent )
{ 
  
  Debug::Throw( "XmlFileList::XmlFileList.\n" );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _saveConfiguration() ) );
  _updateConfiguration();

}
  
//_______________________________________________
bool XmlFileList::_setDBFile( const File& file )
{
  Debug::Throw() << "XmlFileList::_setDBFile - file: " << file << endl;

  // check file
  if( db_file_ == file && !_records().empty() ) return false;
    
  // store file and read
  db_file_ = file;
  _read();
  
  return true;
  
}

//_______________________________________________
bool XmlFileList::_read( void )
{
  Debug::Throw( "XmlFileList::_read.\n" );
  if( db_file_.empty() || !db_file_.exists() ) return false;

  // parse the file
  QFile in( db_file_.c_str() );
  if ( !in.open( QIODevice::ReadOnly ) )
  {
    Debug::Throw( "XmlFileList::_read - cannot open file.\n" );
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
    if( element.tagName() == XmlFileRecord::XML_RECORD ) _add( XmlFileRecord( element ), true, false );
    else Debug::Throw() << "XmlFileList::_read - unrecognized tag " << qPrintable( element.tagName() ) << endl;
  }

  emit contentsChanged();
  
  return true;
}

//_______________________________________________
bool XmlFileList::_write( void )
{
  Debug::Throw( "XmlFileList::_write.\n" );
  if( db_file_.empty() ) 
  {
    Debug::Throw( "XmlFileList::_write - no file.\n" );    
    return false;
  }
  
  // output file
  QFile out( db_file_.c_str() );
  if( !out.open( QIODevice::WriteOnly ) ) return false;

  // get records truncated list
  FileRecord::List records( _truncatedList( _records() ) );
    
  // create document
  QDomDocument document;

  // create main element
  QDomElement top = document.appendChild( document.createElement( XML_FILE_LIST.c_str() ) ).toElement();

  // loop over records
  for( FileRecord::List::const_iterator iter = records.begin(); iter != records.end(); iter++ )
  { 
    Debug::Throw() << "XmlFileList::_write - " << *iter;
    top.appendChild( XmlFileRecord( *iter ).domElement( document ) ); 
  }

  out.write( document.toByteArray() );
  out.close();
    
  return true;
}


//______________________________________
void XmlFileList::_updateConfiguration( void )
{
  Debug::Throw( "XmlFileList::_updateConfiguration.\n" );
  
  // DB file
  _setDBFile( XmlOptions::get().raw("DB_FILE") );
  _setMaxSize( XmlOptions::get().get<int>( "DB_SIZE" ) );
  return;
  
}

//______________________________________
void XmlFileList::_saveConfiguration( void )
{
  Debug::Throw( "XmlFileList::_saveConfiguration.\n" );
  _write();
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
    _add( FileRecord( File( qPrintable( line ) ) ), true, false ); 
  }
  
  in.close();
  
  emit contentsChanged();
  
  return true;
}
