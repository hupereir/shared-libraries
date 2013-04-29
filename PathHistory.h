#ifndef PathHistory_h
#define PathHistory_h

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Counter.h"
#include "File.h"

#include <QList>

class PathHistory: public Counter
{
    public:

    //! constructor
    PathHistory( void ):
        Counter( "PathHistory" ),
        index_(0)
    {   }

    //! true if next path in history is valid
    bool nextAvailable( void ) const
    { return index_ + 1 < pathList_.size(); }

    //! retrieve next path in history
    File next( void )
    { return pathList_[++index_]; }

    //! true if previous path in history is valid
    bool previousAvailable( void ) const
    { return index_ > 0; }

    //! retrieve previous path in history
    File previous( void )
    { return pathList_[--index_]; }

    //! add path to history
    /*!
    Everything that is originally after the current index_ is removed;
    New path is then added after the current index.
    And the current index is moved to it.
    */
    void add( File );

    private:

    //! path list
    QList<File> pathList_;

    //! current index in list
    int index_;

};

#endif
