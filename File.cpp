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
   \file    File.cpp
   \brief   file manipulation utility
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <algorithm>
#include <cmath>
#include <set>
#include <sstream> 
#include <fstream>

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>

#include "File.h"
#include "Debug.h"

using namespace std;

//_____________________________________________________________________
File File::root( void )
{ return File( qPrintable( QDir::rootPath() ) ); }

//_____________________________________________________________________
bool File::isAbsolute( void ) const
{ return QFileInfo( c_str() ).isAbsolute(); }

//_____________________________________________________________________
bool File::create( void ) const
{ return QFile( c_str() ).open( QIODevice::WriteOnly ); }

//_____________________________________________________________________
time_t File::lastModified( void ) const
{ 
  if( !exists() ) return -1;
  return QFileInfo( c_str() ).lastModified().toTime_t();
}

//_____________________________________________________________________
time_t File::lastAccessed( void ) const
{ 
  if( !exists() ) return -1;
  return QFileInfo( c_str() ).lastRead().toTime_t();
}


//_____________________________________________________________________
unsigned int File::userId( void ) const
{ 
  if( !exists() ) return 0;
  return QFileInfo( c_str() ).ownerId();
}

//_____________________________________________________________________
string File::userName( void ) const
{ 
  if( !exists() ) return string();
  QString out( QFileInfo( c_str() ).owner() );
  if( out.isNull() || out.isEmpty() ) out = QString().setNum( QFileInfo( c_str() ).ownerId() );
  return qPrintable( out );
}

//_____________________________________________________________________
unsigned int File::groupId( void ) const
{ 
  if( !exists() ) return 0;
  return QFileInfo( c_str() ).groupId();
}

//_____________________________________________________________________
string File::groupName( void ) const
{ 
  if( !exists() ) return string();
  return qPrintable( QFileInfo( c_str() ).group() );
}

//_____________________________________________________________________ 
QFile::Permissions File::permissions( void ) const
{ return QFileInfo( c_str() ).permissions(); }

//_____________________________________________________________________ 
std::string File::permissionsString( const QFile::Permissions& mode ) const
{
  
  ostringstream what;
  
  // link, directory or regular file
  if( QFileInfo( c_str() ).isDir() ) what << "l";
  else if( QFileInfo( c_str() ).isSymLink() ) what << "d";
  else what << "-";
  
  // user permissions
  what << ( ( mode & QFile::ReadUser ) ? "r":"-" );   
  what << ( ( mode & QFile::WriteUser ) ? "w":"-" );    
  what << ( ( mode & QFile::ExeUser ) ? "x":"-" );    
          
  // group permissions
  what << ( ( mode & QFile::ReadGroup  ) ? "r":"-" );   
  what << ( ( mode & QFile::WriteGroup ) ? "w":"-" );    
  what << ( ( mode & QFile::ExeGroup   ) ? "x":"-" );    
          
  // other permissions
  what << ( ( mode & QFile::ReadOther  ) ? "r":"-" );   
  what << ( ( mode & QFile::WriteOther ) ? "w":"-" );    
  what << ( ( mode & QFile::ExeOther   ) ? "x":"-" );    
  
  return what.str();
  
}
//_____________________________________________________________________
unsigned int File::fileSize( void ) const
{ 
  if( !exists() ) return 0;
  return QFileInfo( c_str() ).size();
}
 
//_____________________________________________________________________
string File::sizeString( const unsigned int& size_int ) 
{ 
  double size = (double) size_int;
  if( size < 0 ) return "-";
  
  static const char* unit[] = { 
    "",
    "k",
    "M",
    "G"
  };
  
  if( size == 0 ) return "0";
  
  int power = static_cast<int>( log( size )/log( static_cast<double>(1024) ) );
  char size_str[7];
  if( !power ) sprintf( size_str, "%7.0f", size ); 
  else {
  
    if( power > 3 ) power = 3;
    size /= pow( static_cast<double>(1024), static_cast<double>(power) );
    sprintf( size_str, "%7.1f", size );
  
  }
  
  int pos( 0 );
  while( size_str[pos] == ' ' ) pos++;
  
  return string( size_str+pos ) + unit[power];

}

//_____________________________________________________________________
bool File::exists( void ) const
{ return QFileInfo( c_str() ).exists(); }

//_____________________________________________________________________
bool File::isWritable( void ) const
{ return empty() || QFile( c_str() ).open( QIODevice::ReadWrite ); }

//_____________________________________________________________________
bool File::isDirectory( void ) const
{ return !empty() && QFileInfo( c_str() ).isDir(); }

//_____________________________________________________________________
bool File::isLink( void ) const
{ return !empty() && QFileInfo( c_str() ).isSymLink(); }

//_____________________________________________________________________
bool File::isHidden( void ) const
{ return !empty() && QFileInfo( c_str() ).isHidden(); }

//_____________________________________________________________________
File File::version( void ) const
{ 
  
  int version=0;
  string expand( File::expand() );
  File out( expand );
  while( out.exists() ) {
    ostringstream o; o << expand << "_" << version;
    out = o.str();
    version++;
  }
  
  return out; 
}

//_____________________________________________________________________
File File::backup( void ) const
{ 
  
  // check filename is valid and file exists
  if( !exists() ) return File();
  
  string expand( File::expand() );
  string backup( expand+"~" );
  
  // open this file
  QFile in( expand.c_str() );
  if( !in.open( QIODevice::ReadOnly ) ) return File();
  
  // open backup
  QFile out( backup.c_str() );
  if( !out.open( QIODevice::WriteOnly ) ) return File();
  
  out.write( in.readAll() );
  out.close();
  in.close();
  return backup;

}
  
//_____________________________________________________________________
bool File::diff(const File& file ) const
{
  
  // no file exists
  if( !( exists() || file.exists() ) ) return false;

  // one of the file does not exists and the other does
  if( !( exists() && file.exists() ) ) return true;
  
  QFile first( c_str() );
  QFile second( file.c_str() );
  bool first_open( first.open( QIODevice::ReadOnly ) );
  bool second_open( second.open( QIODevice::ReadOnly ) );

  // no file exists
  if( !( first_open || second_open ) ) return false;

  // one of the file does not exists and the other does
  if( !( first_open && second_open ) ) return true;
  
  return( first.readAll() != second.readAll() ); 
  
}

//_____________________________________________________________________
bool File::isEqual( const File& file ) const
{
  return expand() == file.expand();
}

//_____________________________________________________________________
File File::readLink( void ) const
{ return File( qPrintable( QFileInfo( c_str() ).canonicalFilePath() ) ); }
  
//_____________________________________________________________________
bool File::remove( void ) const
{
  
  Debug::Throw() << "File::remove - " << *this << endl;
  
  // check if file exists and remove
  // if it does not exists, do nothing and returns true (file was removed already)
  if( exists() ) return QFile( c_str() ).remove();
  else return true;
  
}
 
//_____________________________________________________________________
bool File::removeRecursive( void ) const
{
  
  Debug::Throw() << "File::removeRecursive - " << *this << endl;
  
  if( !isDirectory() ) return remove();
  
  // list content of directory
  QDir dir( c_str() );
  QStringList files( dir.entryList() );
  for( QStringList::iterator iter = files.begin(); iter != files.end(); iter++ )
  { 
    // skip "." and ".."
    if( (*iter) == "." || (*iter) == ".." ) continue;
    File file = File( qPrintable( *iter ) ).addPath( *this );
    if( file.isLink() || !file.isDirectory() )
    {
      if( !file.remove() ) return false;
    } else if( !file.removeRecursive() ) return false; 
  }
  dir.cdUp();
  dir.rmdir( c_str() );
  return true;
}
 
//_____________________________________________________________________
bool File::rename( File new_file ) const
{
  
  // check if file exists and rename
  // return false  otherwise
  if( exists() ) return QFile( c_str() ).rename( new_file.c_str() );
  else return false;
  
}

//____________________________________________  
File File::addPath( const string& path ) const
{  
    
  // returns 0 if either path nor file are given
  if( empty() && path.empty() ) return File();
  
  // return path if path is given but not the file
  if( empty() ) return path;
  
  // if file is absolute, keep unchanged
  if( isAbsolute() ) return *this;
  
  // returns file if it is relative but path is not given
  if( !path.size() ) return string("./")+(*this);

  QFileInfo info;
  info.setFile( QDir( path.c_str() ), c_str() );
  return File( qPrintable( info.absoluteFilePath() ) );

}         
 
//_____________________________________________________________________
File File::expand( void ) const
{ 
  if( empty() ) return File();
  File out( qPrintable( QFileInfo(c_str()).absoluteFilePath() ) );

  // remove trailing slash, except for root
  if( out.size() > 1 && out[out.size()-1] == '/' ) out.resize( out.size()-1 );
  return out;
  
}

//_____________________________________________________________________
File File::path( void ) const
{ return empty() ? File(): File(qPrintable( QFileInfo(c_str()).absolutePath() )); }

//_____________________________________________________________________
File File::localName( void ) const
{ return empty() ? File(): File(qPrintable( QFileInfo(c_str()).fileName() )); }

//_____________________________________________________________________
File File::canonicalName( void ) const
{ 
  if( empty() ) return File();
  QString canonical_name( QFileInfo(c_str()).canonicalFilePath() );
  return canonical_name.isEmpty() ? File():File( qPrintable( canonical_name ) );
}

//_____________________________________________________________________
File File::extension( void ) const
{  
  // check file name
  if( empty() ) return File();
  
  // loop over characters
  string local( localName() ); 
  size_t dotpos = local.rfind(".");
  return File( dotpos == string::npos ? "" : local.substr( dotpos+1, local.size()-dotpos-1 ) );
  
}

//_____________________________________________________________________
File File::truncatedName( void ) const
{

  // check file name
  if( empty() ) return File();
  
  // loop over characters
  size_t dotpos = rfind(".");
  size_t slashpos = rfind( "/" );

  if( dotpos == string::npos ) return *this;
  if( slashpos == string::npos ) return File(dotpos? substr(0,dotpos):"");
  if( slashpos < dotpos ) return substr(0,dotpos);
  
  return *this;

}

//_____________________________________________________________________
list<File> File::listFiles( const unsigned int& flags ) const
{
  
  // Debug::Throw(0) << "File::listFiles - this: " << c_str() << " - hidden: " << (flags&SHOW_HIDDEN) << endl;
  
  File full_name( expand() );
  list<File> out;
  if( !full_name.isDirectory() || (full_name.isLink() && !flags&FOLLOW_LINKS ) ) return out;
  
  // open directory
  QDir::Filters filter = QDir::AllEntries;
  if( flags & SHOW_HIDDEN ) filter |= QDir::Hidden;
  
  QDir dir( full_name.c_str() );
  QStringList files( dir.entryList( filter ) );
  for( QStringList::iterator iter = files.begin(); iter != files.end(); iter++ )
  {
    
    if( *iter == "." || *iter == ".." ) continue;
    
    QFileInfo file_info;
    file_info.setFile( QDir( c_str() ), *iter );
    File found( qPrintable( file_info.absoluteFilePath() ) );
    out.push_back( found );
    
    // Debug::Throw( 0 ) << "File::listFiles - " << found << endl;
    
    // list subdirectory if recursive
    if( flags & RECURSIVE && found.isDirectory() )
    {
      
      // in case directory is a link
      // make sure it is not already in the list
      // to avoid recursivity
      if( found.isLink() && std::find_if( out.begin(), out.end(), SameLinkFTor( found ) ) != out.end() ) continue;
        
      // list subdirectory
      list<File> tmp = found.listFiles( flags ); 
      out.insert( out.end(), tmp.begin(), tmp.end() );
    }
    
  }
    
  return out;
  
}
  
//_____________________________________________________________________
File File::find( const File& file, bool case_sensitive ) const
{
  
  Debug::Throw() << "File::find - this: " << c_str() << endl;
  if( !( exists() && isDirectory() ) ) return File();
  list<File> files( listFiles( RECURSIVE ) );
  list<File> directories;
  for( list<File>::iterator iter = files.begin(); iter != files.end(); iter++ )
  {
        
    // check if file match
    if( File( *iter ).localName().Str::isEqual( file, case_sensitive ) )
    return *iter;
    
    // check if file is directory        
    if( File(*iter).isDirectory() )
    directories.push_back( *iter );  
  }
  
  // loop over directories; search recursively
  for( list<File>::iterator iter = directories.begin(); iter!=directories.end(); iter++ )
  {
    File found( iter->find( file, case_sensitive ) );
    if( found != File() ) return found;
  }
  
  return File();
  
}
