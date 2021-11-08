#ifndef PathHistory_h
#define PathHistory_h

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
#include "base_qt_export.h"

#include <QList>

class BASE_QT_EXPORT PathHistory:  public QObject, private Base::Counter<PathHistory>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit PathHistory( QObject* );

    //*@name accessors
    //@{

    //* path list
    const FileRecord::List& pathList() const
    { return pathList_; }

    //* get previous path list
    FileRecord::List previousPathList() const;

    //* next path list
    FileRecord::List nextPathList() const;

    //* true if previous path in history is valid
    bool previousAvailable() const
    { return index_ > 0; }

    //* true if next path in history is valid
    bool nextAvailable() const
    { return index_ + 1 < pathList_.size(); }

    //@}

    //*@name modifiers
    //@{

    //* set path list
    /**
    All existing files are removed.
    Index is set at the end of the list
    */
    void setPathList( const FileRecord::List& );

    //* add path to history
    /**
    Everything that is originally after the current index_ is removed;
    New path is then added after the current index.
    And the current index is moved to it.
    */
    void add( const FileRecord& );
    void add( const File& file )
    { add( FileRecord( file ) ); }

    //* clear
    void clear();

    //* retrieve next path in history
    File next()
    { return pathList_[++index_].file(); }

    //* retrieve previous path in history
    File previous()
    { return pathList_[--index_].file(); }

    //* select path matching index in previous history
    File selectPath( int );

    //@}

    Q_SIGNALS:

    //* emitted when contents is changed
    void contentsChanged();

    protected:

    //* maximum Size
    void _setMaxSize( int );

    //* maximum size
    int _maxSize() const
    { return maxSize_; }

    //* truncate list if larger than maxSize_
    FileRecord::List _truncatedList() const
    { return _truncatedList( pathList_ ); }

    //* truncate list if larger than maxSize_
    FileRecord::List _truncatedList( FileRecord::List ) const;

    private:

    //* path list
    FileRecord::List pathList_;

    //* maximum size (zero means no limit)
    int maxSize_ = 0;

    //* current index in list
    int index_ = 0;

};

#endif
