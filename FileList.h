#ifndef FileList_h
#define FileList_h

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
   \file FileList.h
   \brief handles list of files saved into resource file for later reopening
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <list>     
#include <string>   

#include "Counter.h"
#include "Debug.h"
#include "Exception.h"
#include "File.h"
#include "FileRecord.h"
#include "TimeStamp.h"

//! handles list of files saved into resource file for later reopening
class FileList: public Counter
{

  public:

  //! constructor
  FileList( void ):
    Counter( "FileList" ),
    max_size_( -1 ),
    check_( true ),
    records_( 0 )
  { _findList( File("") ); }

  //! destructor
  virtual ~FileList( void )
  { Debug::Throw( "FileList::~FileList.\n" ); }

  //! add file.
  virtual void add( const File& file )
  { _add( FileRecord( file ) ); }

  //! remove file from database
  virtual void remove( const File& file );

  //! get filerecord associated to a name
  /*! creates new fileRecord if not found */
  virtual FileRecord& get( const File& file )
  { return _add( FileRecord( file ) ); }
    
  //! maximum Size
  virtual void setMaxSize( const int& value );
  
  //! gets file list size
  virtual int size( void ) const
  {
    Exception::checkPointer( records_, DESCRIPTION( "invalid list" ) );
    return records_->size();
  }
  
  //! get all files
  std::list< File > files( void ) const;
  
  //! get last valid file
  virtual File lastValidFile( void );

  //! check_ flag
  virtual void setCheck( const bool& value )
  { check_ = value; }
  
  protected:

  //! maximum size
  virtual const int& _maxSize( void ) const
  { return max_size_; }

  //! retrieve list of files associated to given file
  virtual void _findList( const File file )
  { records_ = &record_lists_[file]; }

  //! list of files records
  virtual const FileRecord::List& _records( void ) const
  {
    Exception::checkPointer( records_, DESCRIPTION( "invalid list" ) );
    return *records_;
  }
  
  //! add record to current list
  virtual FileRecord& _add( const FileRecord& record );

  //! check flag
  virtual const bool& _check( void ) const
  { return check_; }

  //! returns number of invalid files
  virtual unsigned int _invalidFiles( void ) const;
  
  //! clean files. Remove either invalid or all files, depending on check_
  virtual void _clean( void );

  //! truncate list if larger than max_size_
  virtual void _truncateList( void );

  private:
  
  //! maximum size (negative means no limit)
  int max_size_;

  //! if true, check file validity
  bool check_;

  //! current list of files
  FileRecord::List* records_;

  //! shortcut to list of files map
  typedef std::map< File, FileRecord::List > FileRecordMap;

  //! static map between file lists and associated files
  static FileRecordMap record_lists_;

};
#endif
