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
   \file    File.cc
   \brief   file manipulation utility
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <sstream> 
#include <fstream>

#include <dirent.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "File.h"
#include "Debug.h"
#include "Util.h"

//! default debug level for this file 

using namespace std;

//_____________________________________________________________________
bool File::isAbsolute( void ) const
{ return( ( (*this)[0] =='/' || (*this)[0] =='~' ) ); }

//_____________________________________________________________________
bool File::create( void ) const
{
  ofstream out( this->c_str() );
  if( !out ) return false;
  return true;
}

//_____________________________________________________________________
time_t File::lastModified( void ) const
{ 
  if( !exist() ) return -1;
  
  // get stat structure associated to file
  struct stat statBuf;
  if( stat( expand().c_str(), &statBuf) ) return -1;
  return statBuf.st_mtime;
}

//_____________________________________________________________________
time_t File::lastAccessed( void ) const
{ 
  if( !exist() ) return -1;
  
  // get stat structure associated to file
  struct stat statBuf;
  if( stat( expand().c_str(), &statBuf) ) return -1;
  return statBuf.st_atime;
}


//_____________________________________________________________________
uid_t File::userId( void ) const
{ 
  if( !exist() ) return 0;
  
  // get stat structure associated to file
  struct stat statBuf;
  if( stat( expand().c_str(), &statBuf) ) return 0;
  return statBuf.st_uid;
}

//_____________________________________________________________________
gid_t File::groupId( void ) const
{ 
  if( !exist() ) return 0;
  
  // get stat structure associated to file
  struct stat statBuf;
  if( stat( expand().c_str(), &statBuf) ) return 0;
  return statBuf.st_gid;
}

//_____________________________________________________________________ 
mode_t File::permissions( void ) const
{
  struct stat status;
  if( lstat( expand().c_str(), &status ) !=0 ) return 0;
  else return status.st_mode;
}

//_____________________________________________________________________ 
std::string File::permissionsString( const mode_t& mode )
{
  
  ostringstream what;
  
  // link, directory or regular file
  if( S_ISLNK( mode ) ) what << "l";
  else if( S_ISDIR( mode ) ) what << "d";
  else what << "-";
  
  // user permissions
  what << ( ( mode & S_IRUSR ) ? "r":"-" );   
  what << ( ( mode & S_IWUSR ) ? "w":"-" );    
  what << ( ( mode & S_IXUSR ) ? "x":"-" );    
          
  // group permissions
  what << ( ( mode & S_IRGRP ) ? "r":"-" );   
  what << ( ( mode & S_IWGRP ) ? "w":"-" );    
  what << ( ( mode & S_IXGRP ) ? "x":"-" );    
          
  // other permissions
  what << ( ( mode & S_IROTH ) ? "r":"-" );   
  what << ( ( mode & S_IWOTH ) ? "w":"-" );    
  what << ( ( mode & S_IXOTH ) ? "x":"-" );    
  
  return what.str();
  
}
//_____________________________________________________________________
unsigned int File::fileSize( void ) const
{ 
  if( !exist() ) return 0;
  
  // get stat structure associated to file
  struct stat statBuf;
  if( stat( expand().c_str(), &statBuf) < 0 ) return 0;
  return statBuf.st_size;
  
}
 
//_____________________________________________________________________
string File::sizeString( const unsigned int& size_int ) 
{ 
  double size = (double) size_int;
  if( size < 0 ) return "-";
  
  static char* unit[] = { 
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
bool File::exist( void ) const
{ 
  File expanded( expand() );
  return( expanded.size() && !access(expanded.c_str(), R_OK ) ); 
}

//_____________________________________________________________________
bool File::isWritable( void ) const
{ 
  
  // empty files are always writable
  if( empty() ) return true;
  
  File expanded( expand() );
  return( expanded.size() && !access(expanded.c_str(), W_OK ) ); 
}

//_____________________________________________________________________
bool File::isDirectory( void ) const
{ 

  if( empty() ) return false;
  
  struct stat status;
  return ( stat( expand().c_str(), &status )==0 && S_ISDIR(status.st_mode) );
}

//_____________________________________________________________________
bool File::isLink( void ) const
{ 
  if( empty() ) return false;
  struct stat status;
  return ( lstat( expand().c_str(), &status )==0 && S_ISLNK(status.st_mode) );
}
//_____________________________________________________________________
File File::version( void ) const
{ 
  
  int version=0;
  string expand( this->expand() );
  File out( expand );
  while( out.exist() ) {
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
  if( !exist() ) return EMPTY_STRING;
  
  string expand( this->expand() );
  string backup( expand+"~" );
  ostringstream o; o << "cp \"" << expand << "\" \"" << backup << "\"" ;
  Util::run( o.str() );  
  return backup;
}
  
//_____________________________________________________________________
bool File::diff(const File& file ) const
{
  
  // no file exist
  if( !( exist() || file.exist() ) ) return false;

  // one of the file does not exist and the other does
  if( !( exist() && file.exist() ) ) return true;
  
  ostringstream o; 
  o << "diff -q \"" << expand() << "\" \"" << file.expand() << "\" > /dev/null";  
  return system( o.str().c_str() );

}

//_____________________________________________________________________
File File::readLink( void ) const
{ 

  // check existance and if file is a link
  if( !( exist() && isLink() ) ) return *this;
  
  // expand filename
  File found( expand() );  
  
  // keep track of files found while reading link
  set<File> files;
  files.insert( found );
  
  while( found.isLink() )
  {
    
    // try read link (safe way)
    char* found_buffer( 0 );
    int n_char( 0 );
    int buffer_size=100;
    while( 1 )
    {
      char* buffer = new char[buffer_size];
      n_char = readlink( found.c_str(), buffer, buffer_size );
      if( n_char < 0 ) {
        delete[] buffer;
        break;
      }
      
      if( n_char < buffer_size ) {
        found_buffer = buffer;
        break;
      } 
      
      delete [] buffer;
      buffer_size *= 2;
    
    }
  
    // problem reading link. Return current file
    if( !found_buffer ) return found;
  
    // make buffer a string and truncate
    File new_found( string( found_buffer ).substr( 0, n_char ) );
    delete [] found_buffer;
    
    // update path
    if( !new_found.isAbsolute() )
    { new_found = new_found.addPath( found.path() ).expand(); }
    
    // update "found"
    found = new_found;
    
    // check if found is in list (meaning recursive links)
    if( files.find( found ) != files.end() )
    {
      cout << "File::ReadLink - found recursive link :" << found << endl;
      break;
    }
    
    files.insert( found );
    
  }
  
  // clean and return
  return found;
    
}
  
//_____________________________________________________________________
bool File::remove( void ) const
{
  
  // check if file exist and remove
  // if it does not exist, do nothing and returns true (file was removed already)
  if( exist() ) return (::remove( this->c_str() ) == 0 );
  else return true;
  
}

//____________________________________________  
File File::addPath( const string& path ) const
{  
    
  // returns 0 if either path nor file are given
  if( !( size() || path.size() ) ) return EMPTY_STRING;
  
  // return path if path is given but not the file
  if( empty() ) return path;
  
  // if file is absolute, keep unchanged
  if( isAbsolute() ) return *this;
  
  // returns file if it is relative but path is not given
  if( !path.size() ) return string("./")+(*this);

  // file is relative and path is given
  if( path[path.size()-1] == '/' ) return path+(*this);
  
  return path+"/" + (*this);
}         
 
//_____________________________________________________________________
File File::expand( void ) const
{
  if( empty() ) return EMPTY_STRING;
   
  // make a copy of filename
  string tmpname( _expandTilde( *this ) );
  string name, path;
  _parseFilename( tmpname, name, path );
  return path+name;
}

//_____________________________________________________________________
File File::path( void ) const
{
  if( empty() ) return EMPTY_STRING;

  string name, path;
  _parseFilename( _expandTilde(*this), name, path );
  return path;
}

//_____________________________________________________________________
File File::localName( void ) const
{
  if( empty() ) return EMPTY_STRING;

  string name, path;
  _parseFilename( _expandTilde(*this), name, path );
  return name;
}

//_____________________________________________________________________
File File::extension( void ) const
{  
  // check file name
  if( empty() ) return EMPTY_STRING;
  
  // loop over characters
  size_t dotpos = rfind(".");
  size_t slashpos = rfind( "/" );
  if( slashpos == string::npos ) 
  return ( dotpos == string::npos ) ? EMPTY_STRING:substr( dotpos+1, size()-dotpos-1 );
  return ( dotpos == string::npos || dotpos < slashpos ) ? EMPTY_STRING:substr( dotpos+1, size()-dotpos-1 );
}

//_____________________________________________________________________
File File::truncatedName( void ) const
{

  // check file name
  if( empty() ) return EMPTY_STRING;
  
  // loop over characters
  size_t dotpos = rfind(".");
  size_t slashpos = rfind( "/" );

  if( dotpos == string::npos ) return *this;
  if( slashpos == string::npos ) return (dotpos)? substr(0,dotpos):EMPTY_STRING;
  if( slashpos < dotpos ) return substr(0,dotpos);
  return *this;

}  

//_____________________________________________________________________
list<File> File::listFiles( const bool& recursive, const bool& follow_links ) const
{
  File full_name( expand() );
  list<File> out;
  if( !full_name.isDirectory() || (full_name.isLink() && !follow_links ) ) return out;
  
  // open directory;
  DIR *dir( opendir( full_name.c_str() ) );
  while( struct dirent *entry = readdir( dir ) )
  {
    
    File found( entry->d_name );
    if( found == "." || found == ".." ) continue;
    found = found.addPath( *this );
    out.push_back( found );
    
    // list subdirectory if recursive
    if( recursive && found.isDirectory() )
    {
      // in case directory is a link
      // make sure it is not already in the list
      // to avoid recursivity
      if( found.isLink() && find_if( out.begin(), out.end(), SameLinkFTor( found ) ) != out.end() ) continue;
        
      // list subdirectory
      list<File> tmp = found.listFiles( true, follow_links ); 
      out.insert( out.end(), tmp.begin(), tmp.end() );
    }
    
  }
  
  closedir( dir );
  
  return out;
  
}
  
//_____________________________________________________________________
File File::find( const File& file, bool case_sensitive ) const
{
  
  if( !( exist() && isDirectory() ) ) return EMPTY_STRING;
  list<File> files( listFiles() );
  list<File> directories;
  for( list<File>::iterator iter = files.begin(); iter != files.end(); iter++ )
  {
    // check if file match
    if( File( *iter ).localName().isEqual( file, case_sensitive ) )
    return *iter;
    
    // check if file is directory        
    if( File(*iter).isDirectory() )
    directories.push_back( *iter );  
  }
  
  // loop over directories; search recursively
  for( list<File>::iterator iter = directories.begin(); iter!=directories.end(); iter++ )
  {
    File found( iter->find( file, case_sensitive ) );
    if( found != EMPTY_STRING ) return found;
  }
  
  return EMPTY_STRING;
  
}

//_____________________________________________________________________
bool File::_parseFilename( const string& fullname, string& filename, string& pathname)
{
  
  size_t slashpos = fullname.rfind('/');
  if( slashpos == string::npos ) {
    filename = fullname;
    pathname = Util::workingDirectory()+"/";
    return true;
  }
  
  if( slashpos == fullname.size()-1 ) {
    filename = EMPTY_STRING;
    pathname = fullname;
    return true;
  }
  
  filename = fullname.substr( slashpos+1, fullname.size()-slashpos-1 ); 
  pathname = fullname.substr( 0, slashpos+1 );
  
  // check for . or ..
  if( filename == ".." || filename == "." ) 
  { 
    pathname += filename;
    filename = EMPTY_STRING;
  }
  
  pathname = _normalizePathname( pathname );
  return true;

}

//_____________________________________________________________________
string File::_expandTilde(const string& pathname)
{

  if( !pathname.size() ) return EMPTY_STRING;
  if( pathname[0] != '~' ) return pathname;
  
  if( !getenv("HOME") ) return pathname;
  string out( getenv("HOME") );
  if(pathname.size()>1) out+= pathname.substr( 1, pathname.size()-1);
  return out;
}
    
//_____________________________________________________________________
string File::_normalizePathname( const string &pathname)
{
  if( !pathname.size() ) return EMPTY_STRING;
  
  string out;

  // if this is a relative pathname, prepend current directory 
  if (pathname[0] != '/') {
    char* cwd = new char[MAXPATHLEN];
    if( !getcwd( cwd, MAXPATHLEN ) ) {
      delete cwd;
      return pathname;
    }
    
    out = string( cwd )+"/"+pathname;
    delete[] cwd;
  } else out = pathname;
  
  if( out[out.size()-1] != '/' ) out+="/";
  return _compressPathname(out);
}

//_____________________________________________________________________
string File::_compressPathname(const string& pathname)
{
  
  if( !pathname.size() ) return EMPTY_STRING;
   
  size_t current = 0;
  size_t next = 0;
  list< string > sections;
  while( (next = _nextSlash( pathname, current )) != string::npos ) {
    sections.push_back( pathname.substr( current, next-current+1 ) );
    current = next+1;
  }
  
  if( current < pathname.size() ) sections.push_back( pathname.substr( current, pathname.size()-current ) );
    
  while( 1 ) {
    bool changed = false;
    list< string > compressed;
    for( list< string >::iterator iter = sections.begin(); iter != sections.end(); iter++ )
    if( *iter == "/" && compressed.size() && compressed.back()[compressed.back().size()-1] == '/' ) changed = true;
    else if( *iter == "../" &&  compressed.size() ) { changed = true; compressed.pop_back(); }
    else if( *iter == "./" ) changed = true;
    else compressed.push_back( *iter );
    sections = compressed;
    if( !changed ) break;
  }
  
  string out( "" );
  for( list< string >::iterator iter = sections.begin(); iter != sections.end(); iter++ )
  out+=*iter;

  return out;
}

//_____________________________________________________________________
size_t File::_nextSlash( const string& pathname, size_t start )
{
  if( !pathname.size() ) return string::npos;
  if( start >= pathname.size() ) return string::npos;
  size_t out = pathname.substr( start, pathname.size()-start ).find( "/" );
  if( out != string::npos ) out += start;
  return out;
}
 
//_____________________________________________________________________
string File::_userName( const uid_t& uid )
{
  const std::string& filename( "/etc/passwd" );
  ifstream in( filename.c_str() );
  if( in )
  {
    char line[512];
    while( in.getline( line, 512, '\n' ) )
    {
      if( !(line&&strlen( line ) ) ) continue;
      istringstream line_stream( Str( line ).replace( ":", " " ) );
      string user;
      string status;
      uid_t uid_local;
      line_stream >> user >> status >> uid_local;
      if( line_stream.rdstate() & ios::failbit ) continue;
      if( uid_local == uid ) return user;
    }
  }
  
  return Str().assign<uid_t>( uid );
}
 
//_____________________________________________________________________
string File::_groupName( const gid_t& gid )
{
  const std::string& filename( "/etc/group" );
  ifstream in( filename.c_str() );
  if( in )
  {
    char line[512];
    while( in.getline( line, 512, '\n' ) )
    {
      if( !(line&&strlen( line ) ) ) continue;
      istringstream line_stream( Str( line ).replace( ":", " " ) );
      string group;
      string status;
      gid_t gid_local;
      line_stream >> group >> status >> gid_local;
      if( line_stream.rdstate() & ios::failbit ) continue;
      if( gid_local == gid ) return group;
    }
  }
  
  return Str().assign<uid_t>( gid );
}
