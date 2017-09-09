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

#include "FileThread.h"
#include "Debug.h"

#include <QMetaType>

#include <numeric>

//______________________________________________________
FileThread::FileThread( QObject* parent ):
    QThread( parent ),
    Counter( "FileThread" )
{ qRegisterMetaType<File::List>( "File::List" ); }

//______________________________________________________
void FileThread::run()
{

    totalSize_ = 0;
    files_.clear();
    filesRecursive_.clear();
    error_ = false;

    // process command
    switch( command_ )
    {

        case Command::List:
        case Command::ListRecursive:
        case Command::SizeRecursive:
        {
            _listFiles( file_ );
            break;
        }

        case Command::Copy:
        {
            if( !file_.copy( destination_ ) )
            {
                error_ = true;
                errorString_ = tr("Failed to copy %1 to %2").arg( file_ ).arg( destination_ );
            }
            break;
        }

        default: break;
    }

    // emit (recomputed) total size
    switch( command_ )
    {

        case Command::SizeRecursive:
        _computeTotalSize();
        emit sizeAvailable( totalSize_ );
        break;

        case Command::List:
        if( !files_.empty() )
        { emit filesAvailable( files_ ); }
        break;

        default: break;

    }

    return;

}

//______________________________________________________
void FileThread::_listFiles( const File& parent )
{

    File::ListFlags flags( File::ListFlag::None );

    // show hidden files flag
    if( (flags_&File::ListFlag::ShowHiddenFiles) || command_ == Command::SizeRecursive )
    { flags |= File::ListFlag::ShowHiddenFiles; }

    // follow link flags, always on for first path
    // it is decided again for listRecursive when storing subdirectories
    flags |= File::ListFlag::FollowLinks;

    // clear current list of files
    files_.clear();
    File::List directories;
    for( const auto& file:parent.listFiles( flags ) )
    {

        files_.append( file );
        if( command_ == Command::ListRecursive || command_ == Command::SizeRecursive )
        {

            filesRecursive_.append( file );
            if( file.isDirectory() && ( !file.isLink() || (command_ == Command::ListRecursive && (flags_&File::ListFlag::FollowLinks) ) ) )
            { directories.append( file ); }

        }

    }

    // emit files available
    if( command_ == Command::ListRecursive )
    { emit filesAvailable( files_ ); }

    // emit total size update
    if( command_ == Command::SizeRecursive && _updateTotalSize() )
    { emit sizeAvailable( totalSize_ ); }

    // list sub-directories if needed
    if( command_ == Command::ListRecursive || command_ == Command::SizeRecursive )
    {
        for( const auto& file:directories )
        { _listFiles( file ); }
    }

}

//______________________________________________________
bool FileThread::_updateTotalSize()
{

    qint64 size = std::accumulate( files_.begin(), files_.end(), 0,
        []( const qint64& size, const File& file )
        { return file.isLink() ? size : (size + file.fileSize()); } );

    if( size >= 0 )
    {

        totalSize_+= size;
        return true;

    } else return false;

}

//______________________________________________________
void FileThread::_computeTotalSize()
{
    totalSize_ = std::accumulate( files_.begin(), files_.end(), 0,
        []( const qint64& size, const File& file )
        { return file.isLink() ? size : (size + file.fileSize()); } );
}
