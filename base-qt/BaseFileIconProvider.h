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

//* returns icon for a given FileInfo
class BaseFileIconProvider: public QObject, private Base::Counter<BaseFileIconProvider>
{

    public:

    //* constructor
    explicit BaseFileIconProvider( QObject* = nullptr );

    //* icon matching given file info
    virtual const QIcon& icon( const BaseFileInfo& fileInfo )
    { return icon( fileInfo, fileInfo.type() ); }

    //* icon matching given file info
    virtual const QIcon& icon( const BaseFileInfo&, int );

    //* clear
    virtual void clear()
    { icons_.clear(); }

    //* key, used for hash
    class Key
    {
        public:

        //* constructor
        explicit Key()
        {}

        //* constructor
        explicit Key( const QString& file, int type ):
            file_( file ),
            type_( type )
        {}

        //*@name accessors
        //@{

        //* file
        const QString file() const { return file_; }

        //* type
        int type() const { return type_; }

        //@}

        private:

        QString file_;
        int type_ = 0;

    };

    //*@name pixmap manipulation icons
    //@{

    //* add link overlay icon
    static CustomPixmap linked( const CustomPixmap& );

    //* add hidden effect
    static CustomPixmap hidden( const CustomPixmap& );

    //* add clipped effect
    static CustomPixmap clipped( const CustomPixmap& );

    //@}

    protected:

    //* icon cache
    using IconCache = QHash<Key, QIcon>;
    IconCache& _icons()
    { return icons_; }

    private:

    //* icon map
    IconCache icons_;

    //* invalid icon
    QIcon invalid_;

};

//* equal-to operator
inline bool operator == (const BaseFileIconProvider::Key& first, const BaseFileIconProvider::Key& second )
{ return first.file() == second.file() && first.type() == second.type(); }

//* hash
inline uint qHash( const BaseFileIconProvider::Key& key )
{ return qHash( key.file() )|key.type(); }

#endif
