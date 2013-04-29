// $Id$

/***********************************************************************
*
* Copyright 2013 Hugo PEREIRA DA COSTA <hugo.pereira@free.fr>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#include "FileThread.h"
#include "Debug.h"

#include <QMetaType>

//______________________________________________________
FileThread::FileThread( QObject* parent ):
    QThread( parent ),
    Counter( "FileThread" ),
    flags_( File::None ),
    totalSize_( 0 ),
    error_( false )
{ qRegisterMetaType<File::List>( "File::List" ); }

//______________________________________________________
void FileThread::run( void )
{

    totalSize_ = 0;
    files_.clear();
    filesRecursive_.clear();
    error_ = false;

    // process command
    switch( command_ )
    {

        case List:
        case ListRecursive:
        case SizeRecursive:
        {
            _listFiles( file_ );
            break;
        }

        case Copy:
        {
            if( !file_.copy( destination_ ) )
            {
                error_ = true;
                errorString_ = QString( tr("Failed to copy %1 to %2") ).arg( file_ ).arg( destination_ );
            }
            break;
        }

        default: break;
    }

    // emit (recomputed) total size
    switch( command_ )
    {

        case SizeRecursive:
        _computeTotalSize();
        emit sizeAvailable( totalSize_ );
        break;

        case List:
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

    File::ListFlags flags( File::None );

    // show hidden files flag
    if( (flags_&File::ShowHiddenFiles) || command_ == SizeRecursive )
    { flags |= File::ShowHiddenFiles; }

    // follow link flags, always on for first path
    // it is decided again for listRecursive when storing subdirectories
    flags |= File::FollowLinks;

    // clear current list of files
    files_.clear();

    const File::List files( parent.listFiles( flags ) );
    File::List directories;
    foreach( const File& file, files )
    {

        files_ << file;
        if( command_ == ListRecursive || command_ == SizeRecursive )
        {

            filesRecursive_ << file;
            if( file.isDirectory() && ( !file.isLink() || (command_ == ListRecursive && (flags_&File::FollowLinks) ) ) )
            { directories << file; }

        }

    }

    // emit files available
    if( command_ == ListRecursive )
    { emit filesAvailable( files_ ); }

    // emit total size update
    if( command_ == SizeRecursive && _updateTotalSize() )
    { emit sizeAvailable( totalSize_ ); }

    // list sub-directories if needed
    if( command_ == ListRecursive || command_ == SizeRecursive )
    {
        foreach( const File& file, directories )
        { _listFiles( file ); }
    }

}

//______________________________________________________
bool FileThread::_updateTotalSize( void )
{

    qint64 size(0);
    foreach( const File& file, files_ )
    { if( !file.isLink() ) size += file.fileSize(); }

    if( size >= 0 )
    {

        totalSize_+= size;
        return true;

    } else return false;

}

//______________________________________________________
void FileThread::_computeTotalSize( void )
{

    totalSize_ = 0;
    foreach( const File& file, filesRecursive_ )
    {  if( !file.isLink() ) totalSize_ += file.fileSize(); }
    return;

}
