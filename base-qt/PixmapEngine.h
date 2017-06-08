#ifndef PixmapEngine_h
#define PixmapEngine_h

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
#include "PixmapCache.h"

//! customized Icon factory to provide better looking disabled icons
class PixmapEngine: private Base::Counter<PixmapEngine>
{

    public:

    //! retrieve singleton
    static PixmapEngine& get( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    static const QPixmap& get( const QString& file, bool fromCache = true )
    { return get()._get( file, fromCache ); }

    //! return cache
    static const Base::PixmapCache& cache( void )
    { return get().cache_; }

    //! reload all icons set in cache from new path list
    bool reload( void );

    private:

    //!@name non static methods are hidden
    //@{

    //! constructor
    explicit PixmapEngine( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    const QPixmap& _get( const QString& file, bool fromCache );

    //@}

    //! pixmap path
    File::List pixmapPath_;

    //! map files and QPixmap
    Base::PixmapCache cache_;

};

#endif
