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
#include "Operators.h"

#include <algorithm>

//_______________________________________________
FileList::FileList( QObject* parent ):
    QObject( parent ),
    Counter( QStringLiteral("FileList") ),
    thread_( new ValidFileThread( this ) )
{
    // thread connection
    connect( thread_.get(), &ValidFileThread::recordsAvailable, this, &FileList::_processRecords );
}

//_______________________________________________
bool FileList::contains( const File& file ) const
{ return std::any_of( records_.begin(), records_.end(), FileRecord::SameFileFTorUnary( file ) ); }

//_______________________________________________
void FileList::remove( const File& file )
{
    Debug::Throw() << "FileList::remove - " << file << Qt::endl;
    records_.erase(std::remove_if( records_.begin(), records_.end(), FileRecord::SameFileFTorUnary( file ) ), records_.end() );
    return;
}

//___________________________________________________
void FileList::set( const FileRecord::List& records )
{

    Debug::Throw( QStringLiteral("FileList::set.\n") );
    records_ = records;
    emit contentsChanged();

}

//___________________________________________________
File::List FileList::files() const
{
    Debug::Throw( QStringLiteral("FileList::files.\n") );

    File::List out;
    const auto records( _truncatedList( records_ ) );
    std::transform( records.begin(), records.end(), std::back_inserter( out ), []( const FileRecord& record ) { return record.file(); } );
    return out;
}

//_______________________________________________
FileRecord FileList::lastValidFile()
{

    Debug::Throw( QStringLiteral("FileList::lastValidFile.\n") );

    std::sort( records_.begin(), records_.end(), FileRecord::FirstOpenFTor() );
    const auto iter = std::find_if( records_.rbegin(), records_.rend(), [this]( const FileRecord& record ) { return (!check_) || record.isValid(); } );
    return iter == records_.rend() ? FileRecord():*iter;
}

//_______________________________________________
void FileList::checkValidFiles()
{
    Debug::Throw( QStringLiteral("FileList::checkValidFiles.\n") );
    if( !check_ ) return;
    if( thread_->isRunning() ) return;
    thread_->setRecords( records_ );
    thread_->start();
}

//_______________________________________________
void FileList::_processRecords( const FileRecord::List& records, bool hasInvalidRecords)
{

    // set file records validity
    for( auto& record:records_ )
    { record.setValid( std::any_of( records.begin(), records.end(), FileRecord::SameFileFTorUnary( record.file() ) ) ); }

    _setCleanEnabled( hasInvalidRecords );

    emit validFilesChecked();

}

//___________________________________________________
void FileList::clean()
{
    Debug::Throw( QStringLiteral("FileList::clean") );

    if( check_ )
    {

        // remove invalid files
        records_.erase(
            std::remove_if( records_.begin(), records_.end(), FileRecord::InvalidFTor() ),
            records_.end() );

    } else {

        records_.clear();

    }

    return;
}

//___________________________________________________
void FileList::clear()
{
    Debug::Throw( QStringLiteral("FileList::clear") );
    records_.clear();
    return;
}

//_______________________________________________
void FileList::setMaxSize( int value )
{

    Debug::Throw( QStringLiteral("FileList::setMaxSize.\n") );
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
    auto iter = std::find_if( records_.begin(), records_.end(), FileRecord::SameFileFTorUnary( record.file() ) );
    if( iter != records_.end() )
    {

        Debug::Throw() << "FileList::_add - updating: " << record.file() << Qt::endl;
        if( updateTimeStamp && iter->time() != record.time() )
        {
            iter->setTime( TimeStamp(qMax( iter->time(), record.time() ) ) );
            if( emitSignal ) emit contentsChanged();
        }

        return *iter;

    } else {

        Debug::Throw() << "FileList::_add - adding: " << record.file() << Qt::endl;
        records_.append( record );

        if( emitSignal ) emit contentsChanged();
        return records_.back();

    }

}

//___________________________________________________________________
FileRecord::List FileList::_truncatedList( const FileRecord::List& constRecords ) const
{

    // copy
    auto records = constRecords;

    // remove invalid records
    if( check_ )
    {

        records.erase(
            std::remove_if( records.begin(), records.end(), FileRecord::InvalidFTor() ),
            records.end() );

    }

    // shrink list
    if( maxSize_ > 0 && records.size() > maxSize_ )
    { records.erase( records.begin(), records.begin() + (records.size() - maxSize_ )); }

    return records;
}
