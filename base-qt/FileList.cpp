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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Debug.h"
#include "FileList.h"
#include "File.h"

#include <algorithm>

//_______________________________________________
FileList::FileList( QObject* parent ):
    QObject( parent ),
    Counter( "FileList" ),
    thread_( this )
{
    // thread connection
    connect( &thread_, SIGNAL(recordsAvailable(const FileRecord::List&,bool)), SLOT(_processRecords(const FileRecord::List&,bool)) );
}

//_______________________________________________
bool FileList::contains( const File& file ) const
{ return std::find_if( records_.begin(), records_.end(), FileRecord::SameFileFTor( file ) ) != records_.end(); }

//_______________________________________________
void FileList::remove( const File& file )
{
    Debug::Throw() << "FileList::remove - " << file << endl;
    records_.erase(std::remove_if( records_.begin(), records_.end(), FileRecord::SameFileFTor( file ) ), records_.end() );
    return;
}

//___________________________________________________
void FileList::set( const FileRecord::List& records )
{

    Debug::Throw( "FileList::set.\n" );
    records_ = records;
    emit contentsChanged();

}

//___________________________________________________
File::List FileList::files() const
{
    Debug::Throw( "FileList::files.\n" );

    FileRecord::List records( _truncatedList( records_ ) );
    File::List out;
    for( const auto& record:records )
    { out << record.file(); }

    return out;
}

//_______________________________________________
FileRecord FileList::lastValidFile()
{

    Debug::Throw( "FileList::lastValidFile.\n" );

    // sort list
    std::sort( records_.begin(), records_.end(), FileRecord::FirstOpenFTor() );
    FileRecord::ListIterator iter( records_ );
    iter.toBack();
    while( iter.hasPrevious() )
    {
        const FileRecord& record( iter.previous() );
        if( (!check_) || record.isValid() ) return record;
    }

    return FileRecord( File("") );

}

//_______________________________________________
void FileList::checkValidFiles()
{
    Debug::Throw( "FileList::checkValidFiles.\n" );
    if( !check() ) return;
    if( thread_.isRunning() ) return;
    thread_.setRecords( records_ );
    thread_.start();
}

//_______________________________________________
void FileList::_processRecords( const FileRecord::List& records, bool hasInvalidRecords)
{

    // set file records validity
    for( auto& record:records_ )
    {
        FileRecord::List::const_iterator found = std::find_if( records.begin(), records.end(), FileRecord::SameFileFTor( record.file() ) );
        if( found == records.end() ) continue;
        record.setValid( found->isValid() );
    }

    _setCleanEnabled( hasInvalidRecords );

    emit validFilesChecked();

}

//___________________________________________________
void FileList::clean()
{
    Debug::Throw( "FileList::clean" );

    if( !check() )
    {
        records_.clear();
        return;
    }

    // remove invalid files
    records_.erase(
        std::remove_if( records_.begin(), records_.end(), FileRecord::InvalidFTor() ),
        records_.end() );

    return;
}

//___________________________________________________
void FileList::clear()
{
    Debug::Throw( "FileList::clear" );
    records_.clear();
    return;
}

//_______________________________________________
void FileList::setMaxSize( int value )
{

    Debug::Throw( "FileList::setMaxSize.\n" );
    maxSize_ = value;
    return;

}

//_______________________________________________
FileRecord& FileList::_add(
    const FileRecord& record,
    bool updateTimeStamp,
    bool emitSignal )
{

    // do not add empty files
    Q_ASSERT( !record.file().isEmpty() );

    auto iter = std::find_if( records_.begin(), records_.end(), FileRecord::SameFileFTor( record.file() ) );
    if( iter != records_.end() )
    {

        Debug::Throw() << "FileList::_add - updating: " << record.file() << endl;
        if( updateTimeStamp && iter->time() != record.time() )
        {
            iter->setTime( TimeStamp(qMax( iter->time(), record.time() ) ) );
            if( emitSignal ) emit contentsChanged();
        }

        return *iter;

    } else {

        Debug::Throw() << "FileList::_add - adding: " << record.file() << endl;
        records_ << record;

        if( emitSignal ) emit contentsChanged();
        return records_.back();

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
                auto iter( std::find_if( records.begin(), records.end(), FileRecord::InvalidFTor() ) );
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
