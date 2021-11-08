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

#include <algorithm>

//__________________________________________________________________
PathHistory::PathHistory( QObject* parent ):
    QObject( parent ),
    Counter( QStringLiteral("PathHistory") )
{}

//__________________________________________________________________
FileRecord::List PathHistory::previousPathList() const
{
    FileRecord::List out;
    std::copy( pathList_.begin(), pathList_.begin()+index_, std::back_inserter( out ) );
    return out;
}

//__________________________________________________________________
FileRecord::List PathHistory::nextPathList() const
{
    FileRecord::List out;
    std::copy( pathList_.begin() + index_ +1, pathList_.end(), std::back_inserter( out ) );

    return out;
}

//__________________________________________________________________
void PathHistory::setPathList( const FileRecord::List& pathList )
{
    Debug::Throw( QStringLiteral("PathHistory::setPathList.\n") );

    pathList_ = pathList;
    index_ = pathList.size()-1;
    emit contentsChanged();
}

//__________________________________________________________________
void PathHistory::add( const FileRecord& path )
{
    Debug::Throw() << "PathHistory::add - " << path << Qt::endl;

    // make sure path is valid
    if( path.file().isEmpty() ) return;

    // make sure path is different from current
    if( !pathList_.empty() && pathList_[index_].file() == path.file() )
    {
        pathList_[index_] = path;
        return;
    }

    // remove everything that is after index_ from pathList_
    if( pathList_.size() > index_+1 )
    { pathList_.erase( pathList_.begin() + index_ + 1, pathList_.end() ); }

    // insert new path
    pathList_.append( path );
    index_ = pathList_.size()-1;
    emit contentsChanged();

    return;

}

//__________________________________________
void PathHistory::clear()
{
    Debug::Throw( QStringLiteral("PathHistory::clear.\n") );
    pathList_.clear();
    index_ = 0;
    emit contentsChanged();
}

//__________________________________________________________________
File PathHistory::selectPath( int index )
{
    Debug::Throw( QStringLiteral("PathHistory::selectPath.\n") );
    index_ = qMin( index, pathList_.size()-1 );
    return pathList_[index_].file();
}

//_______________________________________________
void PathHistory::_setMaxSize( int value )
{

    Debug::Throw( QStringLiteral("PathHistory::_setMaxSize.\n") );
    maxSize_ = value;
    return;

}

//___________________________________________________________________
FileRecord::List PathHistory::_truncatedList( FileRecord::List records ) const
{
    if( maxSize_ > 0 && records.size() > maxSize_ )
    { records.erase( records.begin(), records.begin() + (records.size()-maxSize_) ); }

    return records;
}
