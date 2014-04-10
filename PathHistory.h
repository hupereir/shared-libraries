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
#include "FileRecord.h"

#include <QList>

class PathHistory:  public QObject, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    PathHistory( QObject* parent ):
        QObject( parent ),
        Counter( "PathHistory" ),
        index_(0)
    {}

    //!@name accessors
    //@{

    //! path list
    const FileRecord::List& pathList( void ) const
    { return pathList_; }

    //! true if previous path in history is valid
    bool previousAvailable( void ) const
    { return index_ > 0; }

    //! true if next path in history is valid
    bool nextAvailable( void ) const
    { return index_ + 1 < pathList_.size(); }

    //@}

    //!@name modifiers
    //@{

    //! set path list
    /*!
    All existing files are removed.
    Index is set at the end of the list
    */
    void setPathList( const FileRecord::List& );

    //! add path to history
    /*!
    Everything that is originally after the current index_ is removed;
    New path is then added after the current index.
    And the current index is moved to it.
    */
    void add( const FileRecord& );
    void add( const File& file )
    { add( FileRecord( file ) ); }

    //! clear
    void clear( void )
    {
        pathList_.clear();
        index_ = 0;
    }

    //! retrieve next path in history
    File next( void )
    { return pathList_[++index_].file(); }

    //! retrieve previous path in history
    File previous( void )
    { return pathList_[--index_].file(); }

    //@}

    Q_SIGNALS:

    //! emmited when contents is changed
    void contentsChanged( void );

    private:

    //! path list
    FileRecord::List pathList_;

    //! current index in list
    int index_;

};

#endif
