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
void FileList::remove( const File& file )
{
  Debug::Throw() << "FileList::remove - " << file << endl;
  records().erase(remove_if( records().begin(), records().end(), FileRecord::SameFileFTor( file ) ), records().end() );
  return;
}

//___________________________________________________
list<File> FileList::files( void ) const
{
  Debug::Throw( "FileList::files.\n" );
  list<File> out;
  for( FileRecord::List::const_iterator iter = records().begin(); iter != records().end(); iter++ )
  { out.push_back( iter->file() ); }
  
  return out;
}
//_______________________________________________
FileRecord FileList::lastValidFile( void )
{

  Debug::Throw( "FileList::lastValidFile.\n" );
  
  // sort list
  sort( records().begin(), records().end(), FileRecord::FirstOpenFTor() );  
  for( FileRecord::List::reverse_iterator iter = records().rbegin(); iter != records().rend(); iter++ ) 
  { if( (!check_) || iter->isValid() ) return *iter; }
  return FileRecord( File("") );

}

//_______________________________________________
void FileList::checkValidFiles( void )
{
  Debug::Throw( "FileList::checkValidFiles.\n" );
  if( !check() ) return;
  if( thread_.isRunning() ) return;
  thread_.setFiles( records() );
  thread_.start();
}


//_______________________________________________ 
void FileList::customEvent( QEvent* event )
{
  
  if( event->type() != QEvent::User ) return;
  
  ValidFileEvent* valid_file_event( dynamic_cast<ValidFileEvent*>(event) );
  if( !valid_file_event ) return;
  
  // set file records validity
  FileRecord::List& current_records( records() );
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
unsigned int FileList::invalidFiles( void ) const
{
  Debug::Throw( "FileList::invalidFiles.\n" );
  return check() ? count_if( records().begin(), records().end(), FileRecord::InvalidFTor() ):size();
}

//___________________________________________________
void FileList::clean( void )
{
  Debug::Throw( "FileList::clean" );
  if( check() ) 
  {
    records().erase(
      remove_if( records().begin(), records().end(), FileRecord::InvalidFTor() ),
      records().end() );
  } else records().clear();
  return;
}

//_______________________________________________
void FileList::_setMaxSize( const int& value )
{

  Debug::Throw( "FileList::_setMaxSize.\n" );
  max_size_ = value;
  _truncateList();

  return;

}

//_______________________________________________
FileRecord& FileList::_add( 
  const FileRecord& record, 
  const bool& update_timestamp,
  const bool& emit_signal )
{

  FileRecord::List::iterator iter = find_if( records().begin(), records().end(), FileRecord::SameFileFTor( record.file() ) );
  if( iter != records().end() ) 
  {
    
    if( update_timestamp && iter->time() != record.time() ) 
    {
      iter->setTime( max( iter->time(), record.time() ) );
      if( emit_signal ) emit contentsChanged();
    }
    
    return *iter;
    
  } else {

    records().push_back( record );    
    _truncateList();    
    if( emit_signal ) emit contentsChanged();
    return records().back();
    
  }
    
}

//___________________________________________________________________
void FileList::_truncateList( void )
{
  // shrink list
  if( max_size_ > 0 && int(records().size()) > max_size_ )
  {
    
    sort( records().begin(), records().end(), FileRecord::FirstOpenFTor() );
    if( check_ )
    {
      while( int(records().size()) > max_size_ )
      {
        FileRecord::List::iterator iter( find_if( records().begin(), records().end(), FileRecord::InvalidFTor() ) );
        if( iter != records().end() ) records().erase( iter );
        else break;
      }
    }
    
    // remove oldest files
    while( int(records().size()) > max_size_ )
    { records().erase( records().begin() ); }
    
  }
  
  return;
}
