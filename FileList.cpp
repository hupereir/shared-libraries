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
   \file FileList.cpp
   \brief   handles list of files saved into resource file for later reopening
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <algorithm>
#include <fstream>
#include <sstream>

#include "Debug.h"
#include "FileList.h"
#include "File.h"
#include "Str.h"

using namespace std;

//_______________________________________________
bool FileList::has( const File& file ) const
{ return find_if( _records().begin(), _records().end(), FileRecord::SameFileFTor( file ) ) != _records().end(); }

//_______________________________________________
void FileList::remove( const File& file )
{
  Debug::Throw() << "FileList::remove - " << file << endl;
  _records().erase(remove_if( _records().begin(), _records().end(), FileRecord::SameFileFTor( file ) ), _records().end() );
  return;
}

//___________________________________________________
list<File> FileList::files( void ) const
{
  Debug::Throw( "FileList::files.\n" );
  
  FileRecord::List records( _truncatedList( _records() ) );
  
  list<File> out;
  for( FileRecord::List::const_iterator iter = records.begin(); iter != records.end(); iter++ )
  { out.push_back( iter->file() ); }
  
  return out;
}

//_______________________________________________
FileRecord FileList::lastValidFile( void )
{

  Debug::Throw( "FileList::lastValidFile.\n" );
  
  // sort list
  sort( _records().begin(), _records().end(), FileRecord::FirstOpenFTor() );  
  for( FileRecord::List::reverse_iterator iter = _records().rbegin(); iter != _records().rend(); iter++ ) 
  { if( (!check_) || iter->isValid() ) return *iter; }
  return FileRecord( File("") );

}

//_______________________________________________
void FileList::checkValidFiles( void )
{
  Debug::Throw( "FileList::checkValidFiles.\n" );
  if( !check() ) return;
  if( thread_.isRunning() ) return;
  thread_.setFiles( _records() );
  thread_.start();
}


//_______________________________________________ 
void FileList::customEvent( QEvent* event )
{
  
  if( event->type() != QEvent::User ) return;
  
  ValidFileEvent* valid_file_event( dynamic_cast<ValidFileEvent*>(event) );
  if( !valid_file_event ) return;
  
  // set file records validity
  FileRecord::List& current_records( _records() );
  const FileRecord::List& records( valid_file_event->files() ); 
  for( FileRecord::List::iterator iter = current_records.begin(); iter != current_records.end(); iter++ )
  {
    FileRecord::List::const_iterator found = find_if( 
      records.begin(),
      records.end(), 
      FileRecord::SameFileFTor( iter->file() ) );
    if( found == records.end() ) continue;
    iter->setValid( found->isValid() );
  }
  
  emit validFilesChecked();
  return;
  
}

//___________________________________________________
bool FileList::cleanEnabled( void ) const
{
  Debug::Throw( "FileList::cleanEnabled.\n" );
  if( !check() ) return !_records().empty();

  for( FileRecord::List::const_iterator iter = _records().begin(); iter != _records().end(); )
  {
    
    // check item validity
    if( !iter->isValid() ) return true;
    
    // check for duplicates
    FileRecord::SameCanonicalFileFTor ftor( iter->file() );
    if( find_if( ++iter, _records().end(), ftor ) != _records().end() )
    { return true; }
    
  }
  
  return false;
  
}

//___________________________________________________
void FileList::clean( void )
{
  Debug::Throw( "FileList::clean" );
  
  if( !check() ) 
  {
    _records().clear();
    return;
  }

  // remove invalid files
  _records().erase(
    remove_if( _records().begin(), _records().end(), FileRecord::InvalidFTor() ),
    _records().end() );
  
  // remove duplicated files
  sort( _records().begin(), _records().end(), FileRecord::CanonicalFileFTor() );
  _records().erase( unique( _records().begin(), _records().end(), FileRecord::SameCanonicalFileFTor() ), _records().end() );
  sort( _records().begin(), _records().end(), FileRecord::FirstOpenFTor() );
  
  return;
}

//_______________________________________________
void FileList::_setMaxSize( const int& value )
{

  Debug::Throw( "FileList::_setMaxSize.\n" );
  max_size_ = value;

  return;

}

//_______________________________________________
FileRecord& FileList::_add( 
  const FileRecord& record, 
  const bool& update_timestamp,
  const bool& emit_signal )
{
  
  FileRecord::List::iterator iter = find_if( _records().begin(), _records().end(), FileRecord::SameFileFTor( record.file() ) );
  if( iter != _records().end() ) 
  {
    
    if( update_timestamp && iter->time() != record.time() ) 
    {
      iter->setTime( max( iter->time(), record.time() ) );
      if( emit_signal ) emit contentsChanged();
    }
    
    return *iter;
    
  } else {
    
    Debug::Throw() << "FileList::_add - record: " << record.file() << endl;

    _records().push_back( record );    

    if( emit_signal ) emit contentsChanged();
    return _records().back();
    
  }
    
}

//___________________________________________________________________
FileRecord::List FileList::_truncatedList( FileRecord::List records ) const
{
  // shrink list
  if( max_size_ > 0 && int(records.size()) > max_size_ )
  {
    
    sort( records.begin(), records.end(), FileRecord::FirstOpenFTor() );
    if( check_ )
    {
      
      while( int(records.size()) > max_size_ )
      {
        FileRecord::List::iterator iter( find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) );
        if( iter != records.end() ) records.erase( iter );
        else break;
      }
      
    }
    
    // remove oldest files
    while( int(records.size()) > max_size_ )
    { records.erase( records.begin() ); }
    
  }
  
  return records;
}
