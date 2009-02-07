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
bool File::isAbsolute( void ) const
{ return QFileInfo( *this ).isAbsolute(); }

//_____________________________________________________________________
bool File::create( void ) const
{ return QFile( *this ).open( QIODevice::WriteOnly ); }

//_____________________________________________________________________
time_t File::created( void ) const
{ 
  if( !exists() ) return -1;
  return QFileInfo( *this ).created().toTime_t();
}

//_____________________________________________________________________
time_t File::lastModified( void ) const
{ 
  if( !exists() ) return -1;
  return QFileInfo( *this ).lastModified().toTime_t();
}

//_____________________________________________________________________
time_t File::lastAccessed( void ) const
{ 
  if( !exists() ) return -1;
  return QFileInfo( *this ).lastRead().toTime_t();
}


//_____________________________________________________________________
unsigned int File::userId( void ) const
{ 
  if( !exists() ) return 0;
  return QFileInfo( *this ).ownerId();
}

//_____________________________________________________________________
QString File::userName( void ) const
{ 
  if( !exists() ) return QString();
  QString out( QFileInfo( *this ).owner() );
  if( out.isNull() || out.isEmpty() ) out = QString().setNum( QFileInfo( *this ).ownerId() );
  return out;
}

//_____________________________________________________________________
unsigned int File::groupId( void ) const
{ 
  if( !exists() ) return 0;
  return QFileInfo( *this ).groupId();
}

//_____________________________________________________________________
QString File::groupName( void ) const
{ 
  if( !exists() ) return QString();
  return QFileInfo( *this ).group();
}

//_____________________________________________________________________ 
QFile::Permissions File::permissions( void ) const
{ return QFileInfo( *this ).permissions(); }

//_____________________________________________________________________ 
QString File::permissionsString( const QFile::Permissions& mode ) const
{
  QString out;
  QTextStream what( &out );
  
  // link, directory or regular file
  if( QFileInfo( *this ).isDir() ) what << "l";
  else if( QFileInfo( *this ).isSymLink() ) what << "d";
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
  
  return out;
  
}
//_____________________________________________________________________
unsigned int File::fileSize( void ) const
{ 
  if( !exists() ) return 0;
  return QFileInfo( *this ).size();
}
 
//_____________________________________________________________________
QString File::sizeString( const unsigned int& size_int ) 
{ 
  double size = (double) size_int;
  if( size < 0 ) return "-";
  
  static const QString unit[] = { 
    "",
    "k",
    "M",
    "G"
  };
  
  if( size == 0 ) return "0";
  
  int power = static_cast<int>( log( size )/log( static_cast<double>(1024) ) );
  QString out;
  
  if( !power ) out.sprintf( "%7.0f", size ); 
  else {
  
    if( power > 3 ) power = 3;
    size /= pow( static_cast<double>(1024), static_cast<double>(power) );
    out.sprintf( "%7.1f", size );
  
  }
  
  return out.trimmed() + unit[power];

}

//_____________________________________________________________________
bool File::exists( void ) const
{ return QFileInfo( *this ).exists(); }

//_____________________________________________________________________
bool File::isWritable( void ) const
{ return isEmpty() || QFile( *this ).open( QIODevice::ReadWrite ); }

//_____________________________________________________________________
bool File::isDirectory( void ) const
{ return !isEmpty() && QFileInfo( *this ).isDir(); }

//_____________________________________________________________________
bool File::isLink( void ) const
{ return !isEmpty() && QFileInfo( *this ).isSymLink(); }

//_____________________________________________________________________
bool File::isHidden( void ) const
{ return !isEmpty() && QFileInfo( *this ).isHidden(); }

//_____________________________________________________________________
File File::version( void ) const
{ 
  
  int version=0;
  QString expand( File::expand() );
  File out( expand );
  while( out.exists() ) 
  {
    out.clear();
    QTextStream(&out) << expand << "_" << version;
    version++;
  }
  
  return out; 
}

//_____________________________________________________________________
File File::backup( void ) const
{ 
  
  // check filename is valid and file exists
  if( !exists() ) return File();
  
  QString expand( File::expand() );
  QString backup( expand+"~" );
  
  // open this file
  QFile in( expand );
  if( !in.open( QIODevice::ReadOnly ) ) return File();
  
  // open backup
  QFile out( backup );
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
  
  QFile first( *this );
  QFile second( file );
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
{ return File( QFileInfo( *this ).canonicalFilePath() ); }
  
//_____________________________________________________________________
bool File::remove( void ) const
{
  
  Debug::Throw() << "File::remove - " << *this << endl;
  
  // check if file exists and remove
  // if it does not exists, do nothing and returns true (file was removed already)
  if( exists() ) return QFile( *this ).remove();
  else return true;
  
}
 
//_____________________________________________________________________
bool File::removeRecursive( void ) const
{
  
  Debug::Throw() << "File::removeRecursive - " << *this << endl;
  
  if( !isDirectory() ) return remove();
  
  // list content of directory
  QDir dir( *this );
  QStringList files( dir.entryList() );
  for( QStringList::iterator iter = files.begin(); iter != files.end(); iter++ )
  { 
    // skip "." and ".."
    if( (*iter) == "." || (*iter) == ".." ) continue;
    File file = File(*iter).addPath( *this );
    if( file.isLink() || !file.isDirectory() )
    {
      if( !file.remove() ) return false;
    } else if( !file.removeRecursive() ) return false; 
  }
  dir.cdUp();
  dir.rmdir( *this );
  return true;
}
 
//_____________________________________________________________________
bool File::rename( File new_file ) const
{
  
  // check if file exists and rename
  // return false  otherwise
  if( exists() ) return QFile( *this ).rename( new_file );
  else return false;
  
}

//____________________________________________  
File File::addPath( const QString& path, bool use_absolute ) const
{  
    
  // returns 0 if either path nor file are given
  if( isEmpty() && path.isEmpty() ) return File();
  
  // return path if path is given but not the file
  if( isEmpty() ) return path;
  
  // if file is absolute, keep unchanged
  if( isAbsolute() ) return *this;
  
  // returns file if it is relative but path is not given
  if( !path.size() ) return QString("./")+(*this);

  QFileInfo info;
  info.setFile( QDir( path ), *this );
  return use_absolute ? info.absoluteFilePath(): info.filePath();

}         
 
//_____________________________________________________________________
File File::expand( void ) const
{ 
  if( isEmpty() ) return File();
  File out( QFileInfo(*this).absoluteFilePath() );

  // remove trailing slash, except for root
  if( out.size() > 1 && out[out.size()-1] == '/' ) out.resize( out.size()-1 );
  
  return out;
  
}

//_____________________________________________________________________
File File::path( bool use_absolute ) const
{ 
  
  if( isEmpty() ) return File();
  if( use_absolute ) return QFileInfo(*this).absolutePath();
  else return QFileInfo(*this).path();
  
}

//_____________________________________________________________________
File File::localName( void ) const
{ return isEmpty() ? File(): File( QFileInfo(*this).fileName() ); }

//_____________________________________________________________________
File File::canonicalName( void ) const
{ 
  if( isEmpty() ) return File();
  QString canonical_name( QFileInfo(*this).canonicalFilePath() );
  return canonical_name.isEmpty() ? File():File( canonical_name );
}

//_____________________________________________________________________
File File::extension( void ) const
{  
  // check file name
  if( isEmpty() ) return File();
  
  // loop over characters
  QString local( localName() ); 
  int dotpos = local.lastIndexOf(".");
  return File( dotpos < 0 ? "" : local.mid( dotpos+1 ) );
  
}

//_____________________________________________________________________
File File::truncatedName( void ) const
{

  // check file name
  if( isEmpty() ) return File();
  
  // loop over characters
  int dotpos = lastIndexOf(".");
  int slashpos = lastIndexOf( "/" );

  if( dotpos < 0 ) return *this;
  if( slashpos < 0 ) return File( dotpos >= 0 ? left(dotpos):"");
  if( slashpos < dotpos ) return left(dotpos);
  
  return *this;

}

//_____________________________________________________________________
list<File> File::listFiles( const unsigned int& flags ) const
{
  
  // Debug::Throw(0) << "File::listFiles - this: " << *this << " - hidden: " << (flags&SHOW_HIDDEN) << endl;
  
  File full_name( expand() );
  list<File> out;
  if( !full_name.isDirectory() || (full_name.isLink() && !flags&FOLLOW_LINKS ) ) return out;
  
  // open directory
  QDir::Filters filter = QDir::AllEntries;
  if( flags & SHOW_HIDDEN ) filter |= QDir::Hidden;
  
  QDir dir( full_name );
  QStringList files( dir.entryList( filter ) );
  for( QStringList::iterator iter = files.begin(); iter != files.end(); iter++ )
  {
    
    if( *iter == "." || *iter == ".." ) continue;
    
    QFileInfo file_info;
    file_info.setFile( QDir( *this ), *iter );
    File found( file_info.absoluteFilePath() );
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
  
  Debug::Throw() << "File::find - this: " << *this << endl;
  if( !( exists() && isDirectory() ) ) return File();
  list<File> files( listFiles( RECURSIVE ) );
  list<File> directories;
  for( list<File>::iterator iter = files.begin(); iter != files.end(); iter++ )
  {
        
    // check if file match
    if( File( *iter ).localName().compare( file, case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ) == 0 )
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
