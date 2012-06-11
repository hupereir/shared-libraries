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

#include "Debug.h"
#include "FileList.h"
#include "File.h"
#include "Str.h"

#include <algorithm>

//_______________________________________________
bool FileList::contains( const File& file ) const
{ return std::find_if( _records().begin(), _records().end(), FileRecord::SameFileFTor( file ) ) != _records().end(); }

//_______________________________________________
void FileList::remove( const File& file )
{
    Debug::Throw() << "FileList::remove - " << file << endl;
    _records().erase(std::remove_if( _records().begin(), _records().end(), FileRecord::SameFileFTor( file ) ), _records().end() );
    return;
}

//___________________________________________________
void FileList::set( const FileRecord::List& records )
{

    Debug::Throw( "FileList::set.\n" );
    _records() = records;
    emit contentsChanged();

}

//___________________________________________________
File::List FileList::files( void ) const
{
    Debug::Throw( "FileList::files.\n" );

    FileRecord::List records( _truncatedList( _records() ) );
    File::List out;
    foreach( const FileRecord& record, records )
    { out.push_back( record.file() ); }

    return out;
}

//_______________________________________________
FileRecord FileList::lastValidFile( void )
{

    Debug::Throw( "FileList::lastValidFile.\n" );

    // sort list
    std::sort( _records().begin(), _records().end(), FileRecord::FirstOpenFTor() );
    FileRecord::ListIterator iter( _records() );
    iter.toBack();
    while( iter.hasPrevious() )
    {
        const FileRecord& record( iter.previous() );
        if( (!check_) || record.isValid() ) return record;
    }

    return FileRecord( File("") );

}

//_______________________________________________
void FileList::checkValidFiles( void )
{
    Debug::Throw( "FileList::checkValidFiles.\n" );
    if( !check() ) return;
    if( thread_.isRunning() ) return;
    thread_.setRecords( _records() );
    thread_.start();
}

//_______________________________________________
void FileList::customEvent( QEvent* event )
{

    if( event->type() != ValidFileEvent::eventType() ) return QObject::customEvent( event );

    ValidFileEvent* valid_file_event( static_cast<ValidFileEvent*>(event) );
    if( !valid_file_event ) return QObject::customEvent( event );

    // set file records validity
    FileRecord::List& current_records( _records() );
    const FileRecord::List& records( valid_file_event->records() );
    for( FileRecord::List::iterator iter = current_records.begin(); iter != current_records.end(); ++iter )
    {
        FileRecord::List::const_iterator found = std::find_if(
            records.begin(),
            records.end(),
            FileRecord::SameFileFTor( iter->file() ) );
        if( found == records.end() ) continue;
        iter->setValid( found->isValid() );
    }

    _setCleanEnabled( valid_file_event->hasInvalidRecords() );

    emit validFilesChecked();
    return QObject::customEvent( event );

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
        std::remove_if( _records().begin(), _records().end(), FileRecord::InvalidFTor() ),
        _records().end() );

    return;
}

//___________________________________________________
void FileList::clear( void )
{
    Debug::Throw( "FileList::clear" );
    _records().clear();
    return;
}
//_______________________________________________
void FileList::_setMaxSize( const int& value )
{

    Debug::Throw( "FileList::_setMaxSize.\n" );
    maxSize_ = value;

    return;

}

//_______________________________________________
FileRecord& FileList::_add(
    const FileRecord& record,
    const bool& update_timestamp,
    const bool& emit_signal )
{

    // do not add empty files
    assert( !record.file().isEmpty() );

    FileRecord::List::iterator iter = std::find_if( _records().begin(), _records().end(), FileRecord::SameFileFTor( record.file() ) );
    if( iter != _records().end() )
    {

        if( update_timestamp && iter->time() != record.time() )
        {
            iter->setTime( std::max( iter->time(), record.time() ) );
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
    if( maxSize_ > 0 && int(records.size()) > maxSize_ )
    {

        std::sort( records.begin(), records.end(), FileRecord::FirstOpenFTor() );
        if( check_ )
        {

            while( int(records.size()) > maxSize_ )
            {
                FileRecord::List::iterator iter( std::find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) );
                if( iter != records.end() ) records.erase( iter );
                else break;
            }

        }

        // remove oldest files
        while( int(records.size()) > maxSize_ )
        { records.erase( records.begin() ); }

    }

    return records;
}
