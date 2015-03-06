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

#include "PathHistory.h"

#include "Debug.h"

//__________________________________________________________________
PathHistory::PathHistory( QObject* parent ):
    QObject( parent ),
    Counter( "PathHistory" ),
    index_(0)
{ Debug::Throw( "PathHistory::PathHistory.\n" ); }

//__________________________________________________________________
FileRecord::List PathHistory::previousPathList( void ) const
{
    FileRecord::List out;
    for( int index = 0; index < index_; ++index )
    { out.append( pathList_[index] ); }

    return out;
}

//__________________________________________________________________
FileRecord::List PathHistory::nextPathList( void ) const
{
    FileRecord::List out;
    for( int index = index_+1; index < pathList_.size(); ++index )
    { out.append( pathList_[index] ); }

    return out;
}

//__________________________________________________________________
void PathHistory::setPathList( const FileRecord::List& pathList )
{
    Debug::Throw( "PathHistory::setPathList.\n" );

    pathList_ = pathList;
    index_ = pathList.size()-1;
    emit contentsChanged();
}

//__________________________________________________________________
void PathHistory::add( const FileRecord& path )
{
    Debug::Throw() << "PathHistory::add - " << path << endl;

    // make sure path is valid
    if( path.file().isEmpty() ) return;

    // make sure path is different from current
    if( !pathList_.empty() && pathList_[index_].file() == path.file() )
    {
        pathList_[index_] = path;
        return;
    }

    // remove everything that is after index_ from pathList_
    while( pathList_.size() > index_+1 ) pathList_.removeLast();

    // insert new path
    pathList_.append( path );
    index_ = pathList_.size()-1;
    emit contentsChanged();

    return;

}

//__________________________________________
void PathHistory::clear( void )
{
    Debug::Throw( "PathHistory::clear.\n" );
    pathList_.clear();
    index_ = 0;
    emit contentsChanged();
}

//__________________________________________________________________
File PathHistory::selectPath( int index )
{
    Debug::Throw( "PathHistory::selectPath.\n" );
    index_ = qMin( index, pathList_.size()-1 );
    return pathList_[index_].file();
}

//_______________________________________________
void PathHistory::_setMaxSize( int value )
{

    Debug::Throw( "PathHistory::_setMaxSize.\n" );
    maxSize_ = value;
    return;

}

//___________________________________________________________________
FileRecord::List PathHistory::_truncatedList( FileRecord::List records ) const
{
    if( maxSize_ > 0 )
    {
        while( records.size() > maxSize_ )
        { records.removeFirst(); }
    }

    return records;
}
