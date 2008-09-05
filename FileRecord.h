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
    flags_( 0 ),
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
  FileRecord& setFile( const std::string& file )
  { 
    file_ = file; 
    return *this;
  }

  //! time stamp
  const int& time( void ) const
  { return time_; }

  //! time stamp
  FileRecord& setTime( const TimeStamp& time )
  { 
    time_ = time;
    return *this;
  }
  
  //!@name flags
  //@{
  
  //! flags
  FileRecord& setFlags( unsigned int value )
  { 
    flags_ = value; 
    return *this;
  }
  
  //! flags
  FileRecord& setFlag( unsigned int flag, const bool& value = true )
  {
    if( value ) { flags_ |= flag; }
    else { flags_ &= (~flag); } 
    return *this;
  }
  
  //! flags
  const unsigned int& flags( void ) const
  { return flags_; }

  //! flags
  bool hasFlag( const unsigned int& flag ) const
  { return flags_ & flag; }
  
  //! used to retrieve file records that match a given flag
  class HasFlagFTor
  {
    
    public:
    
    //! constructor
    HasFlagFTor( const unsigned int& flag ):
      flag_( flag )
      {}
      
    //! predicate
    bool operator() ( const FileRecord& record ) const
    { return record.hasFlag( flag_ ); }
      
    private:
    
    // predicted flag
    unsigned int flag_;
    
  };
  
  //@}
  
  //! validity
  const bool& isValid( void ) const
  { return valid_; }
  
  //! validity
  FileRecord& setValid( const bool& valid )
  { 
    valid_ = valid; 
    return *this;
  }

  
  //!@name properties
  //@{
  
  //! map string to unsigned int property ID
  class PropertyId
  {
    
    public: 
    
    typedef unsigned short Id;
    
    //! get id matching name
    /*! insert in map if name is new */
    static Id get( std::string name );
   
    //! get name matching id
    /*! throw exception if not found */
    static std::string get( Id );
     
    private:
    
    //! counter
    static Id counter_;
    
    //! id map
    typedef std::map< std::string, Id > IdMap;
    
    //! id map
    static IdMap id_map_;
    
    //! id map
    typedef std::vector< std::string > NameMap;
    
    //! name map
    static NameMap name_map_;
    
  };

  //! add property
  FileRecord& addProperty( std::string tag, std::string value )
  { return addProperty( PropertyId::get( tag ), value ); }
  
  //! add property
  FileRecord& addProperty( PropertyId::Id, std::string );

  //! add property
  template <typename T>
  FileRecord& addProperty( std::string tag, const T& value )
  { return addProperty<T>( tag, value ); }

  //! add property
  template <typename T>
  FileRecord& addProperty( PropertyId::Id id, const T& value )
  { 
    properties_[ id ] = Str().assign<T>( value ); 
    return *this;
  }
  
  //! true if property is available
  bool hasProperty( std::string tag ) const
  { return hasProperty( PropertyId::get( tag ) ); }
    
  //! true if property is available
  bool hasProperty( PropertyId::Id id ) const
  { return properties_.find( id ) != properties_.end(); }

  //! additional property map
  typedef std::map< PropertyId::Id, std::string > PropertyMap;
  
  //! property map
  const PropertyMap& properties( void ) const
  { return properties_; }

  //! retrieve property
  std::string property( std::string tag ) const
  { return property( PropertyId::get( tag ) ); }

  //! retrieve property
  std::string property( PropertyId::Id id ) const
  {
    PropertyMap::const_iterator iter(  properties_.find( id ) );
    return ( iter == properties_.end() ) ? "":iter->second;
  }
  
  //! convert string to any type using string streamer
  template <typename T>
  T property( const std::string& tag ) const
  { return property<T>( PropertyId::get( tag ) ); }
  
  //! convert string to any type using string streamer
  template <typename T>
  T property( PropertyId::Id id ) const
  {
    PropertyMap::const_iterator iter(  properties_.find( id ) );
    assert( iter != properties_.end() );
    return Str( iter->second ).get<T>();
  }

  //@}
  
  //! used to sort records according to files
  class FileFTor
  {
    
    public:
    
    //! predicate
    bool operator() (const FileRecord& first, const FileRecord& second) const
    { return first.file() < second.file(); }
  
  };
  
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
    { return first.file() == second.file(); }
    
    private:
    
    //! filename
    File file_;
    
  };

  //! used to sort FileRecords using canonical filenames
  class CanonicalFileFTor
  {
    
    public:
        
    //! predicate
    bool operator() (const FileRecord& first, const FileRecord& second) const
    { return first.file().canonicalName() < second.file().canonicalName(); }
      

  };

  //! used to remove FileRecord with identical canonical filenames
  class SameCanonicalFileFTor
  {
    
    public:
          
    //! constructor
    SameCanonicalFileFTor( const File& file = File("") ):
      file_( file.canonicalName() )
    {}
          
    //! constructor
    SameCanonicalFileFTor( const FileRecord& record ):
      file_( record.file().canonicalName() )
    {}
    
    //! predicate
    bool operator() (const FileRecord& first, const FileRecord& second) const
    { return first.file().canonicalName() == second.file().canonicalName(); }
   
    //! predicate
    bool operator() (const FileRecord& record ) const
    { return record.file().canonicalName() == file_; }
    
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
  
  //! flags
  unsigned int flags_;
  
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
