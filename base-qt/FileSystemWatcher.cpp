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

#include "FileSystemWatcher.h"

//___________________________________________________
void FileSystemWatcher::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {

        Debug::Throw("FileSystemWatcher::timerEvent.\n" );

        timer_.stop();
        for( const auto& directory:directories_ )
        { emit directoryChangedDelayed( directory ); }

        directories_.clear();

    } else return QFileSystemWatcher::timerEvent( event );

}

//___________________________________________________
void FileSystemWatcher::_addModifiedDirectory( const QString& directory )
{

    if( directories_.contains( directory ) ) return;

    // append to directory list
    Debug::Throw() << "FileSystemWatcher::_addModifiedDirectory - " << directory << endl;
    directories_ << directory;

    // restart timer
    if( !timer_.isActive() ) timer_.start( delay_, this );

}
