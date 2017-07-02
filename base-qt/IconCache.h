#ifndef IconCache_h
#define IconCache_h

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

#include <QString>
#include <QStringList>
#include <QHash>
#include <QPair>
#include <QIcon>

namespace Base
{
    //* cache item
    class IconCacheItem: public QIcon, private Counter<IconCacheItem>
    {

        public:

        //* constructor
        explicit IconCacheItem():
            Counter( "Base::IconCacheItem" )
        {}

        //* copy constructor
        explicit IconCacheItem( const QIcon& other ):
            QIcon( other ),
            Counter( "Base::IconCacheItem" )
        {}

        //* copy constructor
        explicit IconCacheItem( QIcon&& other ):
            QIcon( std::move( other ) ),
            Counter( "Base::IconCacheItem" )
        {}

        //* flags
        enum Flag
        {
            None = 0,
            FromCache = 1<<0,
            FromTheme = 1<<1,
            FromResource = 1<<2,
            Any = FromCache|FromTheme|FromResource
        };

        Q_DECLARE_FLAGS( Flags, Flag );

        //*@name accessors
        //@{

        //* flags
        Flags flags() const
        { return flags_; }

        //* files
        const QStringList& files() const
        { return files_; }

        //@}

        //*@name modifiers
        //@{

        //* flags
        void setFlags( Flags value )
        { flags_ = value; }

        //* clear files
        void clearFiles()
        { files_.clear(); }

        //* add file
        void addFile( const QString& file )
        { files_ << file; }

        //@}

        private:

        //* flags
        Flags flags_ = Flag::None;

        //* associated files
        QStringList files_;

    };

    //* cache
    class IconCache: public QHash<QString, IconCacheItem>
    {

        public:

        //* constructor
        explicit IconCache()
        {}

        //* pair
        class Pair: public QPair<QString, IconCacheItem >
        {
            public:

            //* universal constructor
            template< typename... Args >
            explicit Pair( Args&&... args ):
                QPair<QString, IconCacheItem>( std::forward<Args>(args)... )
                {}

            //* equal to operator
            bool operator == ( const Pair& other ) const
            { return first == other.first; }

            //* less than operator
            bool operator < ( const Pair& other ) const
            { return first < other.first; }

        };

    };

    Q_DECLARE_OPERATORS_FOR_FLAGS( IconCacheItem::Flags )

}

#endif
