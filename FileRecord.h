#ifndef FileRecord_h
#define FileRecord_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
   \file FileRecord.h
   \brief handles previously opened file and tags
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <assert.h>
#include <string> 
#include <vector>  

#include "Counter.h"
#include "Debug.h"
#include "File.h"
#include "Str.h"
#include "TimeStamp.h"

//! handles previously opened file and tags 
class FileRecord: public Counter
{

  public:
  
  //! shortcut to list of records
  //typedef std::list<FileRecord> List;
  typedef std::vector<FileRecord> List;

  //! constructor
  FileRecord( const File& file = File(""), const TimeStamp& time = TimeStamp::now() ):
    Counter( "FileRecord" ),
    file_( file ),
    time_( time ),
    valid_( true )
  {}

  //! less than operator
  bool operator < (const FileRecord& record ) const;

  //! less than operator
  bool operator == (const FileRecord& record ) const;

  //! file
  const File& file( void ) const
  { return file_; }

  //! file
  void setFile( const std::string& file )
  { file_ = file; }

  //! time stamp
  const int& time( void ) const
  { return time_; }

  //! time stamp
  void setTime( const TimeStamp& time )
  { time_ = time; }
  
  //! validity
  const bool& isValid( void ) const
  { return valid_; }
  
  //! validity
  void setValid( const bool& valid )
  { valid_ = valid; }

  //! add information
  void addProperty( const std::string& tag, const std::string& value )
  { properties_[tag] = value; }
  
  //! true if information is available
  bool hasProperty( const std::string& tag ) const
  { return properties_.find( tag ) != properties_.end(); }
  
  //! additional information map
  typedef std::map< std::string, std::string > PropertyMap;
  
  //! property map
  const PropertyMap& properties( void ) const
  { return properties_; }

  //! retrieve information
  std::string property( const std::string& tag ) const
  {
    PropertyMap::const_iterator iter(  properties_.find( tag ) );
    return ( iter == properties_.end() ) ? "":iter->second;
  }
  
  //! convert string to any type using string streamer
  template <typename T>
  T property( const std::string& tag ) const
  {
    PropertyMap::const_iterator iter(  properties_.find( tag ) );
    assert( iter != properties_.end() );
    return Str( iter->second ).get<T>();
  }
  
  //! used to retrieve FileRecord with identical filenames
  class SameFileFTor
  {
  
    public:
  
    //! constructor
    SameFileFTor( const File& file = File("") ):
      file_( file )
    {}

    //! constructor
    SameFileFTor( const FileRecord& record ):
      file_( record.file() )
    {}
    
    //! predicate
    bool operator() (const FileRecord& record ) const
    { return record.file() == file_; }
    
    //! predicate
    bool operator() (const FileRecord& first, const FileRecord& second) const
    { return first.file() < second.file(); }
    
    private:
    
    //! filename
    File file_;
    
  };

  //! used to remove non existing files
  class InvalidFTor
  {
    public:

    //! predicate
    bool operator()( const FileRecord& record )
    { return !record.isValid(); }

  };

  //! used to remove non existing files
  class ValidFTor
  {
    public:

    //! predicate
    bool operator()( const FileRecord& record )
    { return record.isValid(); }

  };  
  
  //! used to retrieve FileRecord with identical filenames
  class FirstOpenFTor
  {
  
    public: 
    
    //! predicate
    bool operator() (const FileRecord& first, const FileRecord& second) const
    { return first.time() < second.time(); }
    
  };
  
  private:

  //! file
  File file_;
  
  //! additional properties
  PropertyMap properties_;
  
  //! time
  int time_;
  
  //! file validity (true if file exists)
  bool valid_;
  
  //! streamers
  friend std::ostream& operator << ( std::ostream& out, const FileRecord& record )
  {
    out << record.file() << std::endl;
    for( PropertyMap::const_iterator iter = record.properties().begin(); iter != record.properties().end(); iter++ )
    { out << "  " << iter->first << ": " << iter->second << std::endl; }
    return out;
  }

};

#endif
