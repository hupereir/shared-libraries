#ifndef BaseFileIconProvider_h
#define BaseFileIconProvider_h

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

#include "BaseFileInfo.h"
#include "Counter.h"
#include "CustomPixmap.h"
#include "File.h"

#include <QHash>
#include <QIcon>
#include <QObject>
#include <QPixmap>

//* returns icon for a given FileInfo
class BaseFileIconProvider: public QObject, public Counter
{

    public:

    //* constructor
    BaseFileIconProvider( QObject* = nullptr );

    //* icon matching given file info
    virtual const QIcon& icon( const BaseFileInfo& fileInfo )
    { return icon( fileInfo, fileInfo.type() ); }

    //* icon matching given file info
    virtual const QIcon& icon( const BaseFileInfo&, int );

    //* clear
    virtual void clear( void )
    { icons_.clear(); }

    //* key, used for hash
    class Key
    {
        public:

        //* constructor
        Key( void )
        {}

        //* constructor
        Key( const QString& file, int type ):
            file_( file ),
            type_( type )
        {}

        //* equal-to operator
        bool operator == (const Key& other ) const
        { return file_ == other.file_ && type_ == other.type_; }

        QString file_;
        int type_ = 0;

    };

    protected:

    //* icon cache
    using IconCache = QHash<Key, QIcon>;
    IconCache& _icons( void )
    { return icons_; }

    private:

    //* icon map
    IconCache icons_;

    //* invalid icon
    QIcon invalid_;

};

//* hash
inline uint qHash( const BaseFileIconProvider::Key& key )
{ return qHash( key.file_ )|key.type_; }

#endif
