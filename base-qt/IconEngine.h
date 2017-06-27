#ifndef IconEngine_h
#define IconEngine_h

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
#include "Debug.h"
#include "File.h"
#include "IconCache.h"

//* customized Icon factory to provide better looking disabled icons
class IconEngine final: private Base::Counter<IconEngine>
{

    public:

    //* retrieve singleton
    static IconEngine& get();

    //* copy constructor
    explicit IconEngine( const IconEngine& ) = delete;

    //* assignment operator
    IconEngine& operator = (const IconEngine& ) = delete;

    //* create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    static const QIcon& get( const QString& file, Base::IconCacheItem::Flags flags = Base::IconCacheItem::Flag::Any )
    { return get()._get( file, flags ); }

    //* return cache
    static const Base::IconCache& cache()
    { return get().cache_; }

    //* reload all icons set in cache from new path list
    bool reload();

    //* clear
    void clear();

    private:

    //*@name non static methods are hidden
    //@{

    //* constructor
    explicit IconEngine();

    //* create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    const Base::IconCacheItem& _get( const QString&, Base::IconCacheItem::Flags = Base::IconCacheItem::Flag::Any );

    //@}

    //* pixmap path
    File::List pixmapPath_;

    //* map files and QIcon
    Base::IconCache cache_;

};

#endif
