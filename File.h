// $Id$ 

#ifndef File_h
#define File_h

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
   \file File.h
   \brief   file manipulation utility
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <time.h>
#include <string>
#include <list>

#include "Counter.h"
#include "Str.h"

//! file manipulation utility
class File: public Str
{
  
  public:
  
  //! constructor
  File( void ):
    Str( EMPTY_STRING )
  {}
  
  //! constructor
  File( const std::string& value ):
    Str( value )
  {}
  
  //! returns true if file has absolute pathname
  bool isAbsolute( void ) const;
  
  //! try create
  bool create( void ) const;
    
  //! time of file last modification
  virtual time_t lastModified( void ) const;

  //! time of file last access
  virtual time_t lastAccessed( void ) const;
  
  //! user id
  virtual uid_t userId( void ) const;
  
  //! group id
  virtual gid_t groupId( void ) const;
  
  //! user name
  virtual std::string userName( void ) const
  { return _userName( userId() ); }

  //! group name
  virtual std::string groupName( void ) const
  { return _groupName( groupId() ); }
  
  //! permission string
  virtual mode_t permissions( void ) const;

  //! permission string
  virtual std::string permissionsString( void ) const
  { return permissionsString( permissions() ); }
  
  //! permission string
  static std::string permissionsString( const mode_t& );
  
  //! file size
  virtual unsigned int fileSize( void ) const;
  
  //! file size (string version)
  virtual std::string sizeString( void ) const
  { return sizeString( size() ); }
  
  //! file size (string version)
  static std::string sizeString( const unsigned int& size );
    
  //! tells if a file exists
  virtual bool exist( void ) const;
  
  //! tells if a file exists and can be written into
  virtual bool isWritable( void ) const;
  
  //! tells if a file exists and is a directory
  virtual bool isDirectory( void ) const;
  
  //! tells if a file is a symbolic link
  virtual bool isLink( void ) const;
  
  //! returns a versioned filename
  /*! (add _i at the end with i the first integer for which file is not found) */
  virtual File version( void ) const;
  
  //! make a backup copy (terminated by ~) of a file, returns backup file
  virtual File backup( void ) const;
  
  //! returns true if two file differs
  virtual bool diff( const File& file ) const;
  
  //! return link destination file
  virtual File readLink( void ) const;
  
  //! removes file from disk
  /*!
    returns true if the file does not exists 
    or was successfully removed
  */
  virtual bool remove( void ) const;
  
  //! adds path to a file
  /*! note: the file is taken raw. No truncation/expension performed.*/
  virtual File addPath( const std::string& path ) const;
  
  //! expand a file name replacing .. or ~ to full path
  virtual File expand( void ) const;
  
  //! get path associated to full name
  virtual File path( void ) const;
  
  //! remove path from full name
  virtual File localName( void ) const;
  
  //! get file extension
  virtual File extension( void ) const;
  
  //! get truncated file (no extension)
  virtual File truncatedName( void ) const;
  
  //! return first file with matching short name, or empty string if not found
  virtual File find( const File& file, bool case_sensitive = true ) const;
  
  //! return list of files in a directory
  virtual std::list< File > listFiles( const bool& recursive = false, const bool& follow_links = true ) const;
  
  //! used to find files pointing to the same link
  class SameLinkFTor
  {
    
    public:
    
    //! constructor
    SameLinkFTor( const File& file ):
      file_( file.readLink() )
    {}
    
    //! predicate
    bool operator() ( const File& file ) const
    { return file.readLink() == file_; }
    
    private:
    
    //! predicted file
    std::string file_;
    
  };
  
  private:
  
  /*! \fn static size_t _NextSlash( const std::string& str, size_t pos)
    \brief returns position of next slash in a string starting from given one
    \param str the string
    \param pos the starting position
  */
  static size_t _nextSlash( const std::string& str, size_t pos); 
  
  //! cut a full file name into local name and path
  static bool _parseFilename( const std::string& fullname, std::string& filename, std::string& pathname);
  
  //! replace ~ by full path in a pathname
  static std::string _expandTilde( const std::string& pathname);
  
  //! make pathname std: starting from absolute path, with no '..' of '.' of '//' in there
  static std::string _normalizePathname( const std::string& pathname);
  
  //! remove useless peaces of string from pathname
  static std::string _compressPathname( const std::string& pathname);
  
  //! retrieve username from user id
  static std::string _userName( const uid_t& uid );
  
  //! retrieve group name from group id
  static std::string _groupName( const gid_t& gid );
  
};

#endif
