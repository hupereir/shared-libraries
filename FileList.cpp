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
  records_->erase(remove_if( records_->begin(), records_->end(), FileRecord::SameFileFTor( file ) ), records_->end() );
  return;
}

//_______________________________________________
void FileList::setMaxSize( const int& value )
{
  Debug::Throw( "FileList::setMaxSize.\n" );
  max_size_ = value;
  _truncateList();

  return;

}
//___________________________________________________
list<File> FileList::files( void ) const
{
  Debug::Throw( "FileList::files.\n" );
  list<File> out;
  for( FileRecord::List::const_iterator iter = records_->begin(); iter != records_->end(); iter++ )
  { out.push_back( iter->file() ); }
  
  return out;
}
//_______________________________________________
FileRecord FileList::lastValidFile( void )
{

  Debug::Throw( "FileList::lastValidFile.\n" );
  
  // sort list
  records_->sort();
  
  if( Debug::level() >= 1 )
  {
    for( FileRecord::List::reverse_iterator iter = records_->rbegin(); iter != records_->rend(); iter++ )
    { cout << "FileList::lastValidFile - file: " << iter->file() << " time: " << iter->file() << endl; }
  }
  
  for( FileRecord::List::reverse_iterator iter = records_->rbegin(); iter != records_->rend(); iter++ ) 
  { 
    if( (!check_) || iter->isValid() ) 
    {
      current_ = *iter; 
      return *iter;
    }
  }
  return current_ = FileRecord( File("") );

}

//___________________________________________________
void FileList::_clean( void )
{
  Debug::Throw( "FileList::_clean" );
  if( check_ ) {
    records_->erase(
      remove_if( records_->begin(), records_->end(), FileRecord::InvalidFTor() ),
      records_->end() );
  } else records_->clear();
  return;
}

//___________________________________________________ 
unsigned int FileList::_invalidFiles( void ) const
{
  Debug::Throw( "FileList::_invalidFiles.\n" );
  return (check_) ? count_if( records_->begin(), records_->end(), FileRecord::InvalidFTor() ):size();
}

//_______________________________________________
FileRecord& FileList::_add( const FileRecord& record, const bool& update_timestamp )
{

  FileRecord::List::iterator iter = find_if( records_->begin(), records_->end(), FileRecord::SameFileFTor( record.file() ) );
  if( iter != records_->end() ) 
  {
    
    if( update_timestamp ) iter->setTime( max( iter->time(), record.time() ) );
    
    current_ = *iter;
    return *iter;
    
  } else {

    records_->push_back( record );    
    _truncateList();    
    current_ = records_->back();
    return records_->back();
    
  }
    
}

//___________________________________________________________________
void FileList::_truncateList( void )
{
  // shrink list
  if( max_size_ > 0 && int(records_->size()) > max_size_ )
  {
    
    records_->sort();
    
    // first try remove invalid files
    if( check_ )
    {
      while( int(records_->size()) > max_size_ )
      {
        FileRecord::List::iterator iter( find_if( records_->begin(), records_->end(), FileRecord::InvalidFTor() ) );
        if( iter != records_->end() ) records_->erase( iter );
        else break;
      }
    }
    
    // remove oldest files
    while( int(records_->size()) > max_size_ )
    { records_->pop_front(); }
    
  }
  
  return;
}
