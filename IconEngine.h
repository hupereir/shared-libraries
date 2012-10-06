#ifndef IconEngine_h
#define IconEngine_h

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "Counter.h"
#include "Debug.h"
#include "IconCache.h"

#include <QtCore/QStringList>

//! customized Icon factory to provide better looking disabled icons
class IconEngine: public Counter
{

    public:

    //! retrieve singleton
    static IconEngine& get( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    static const QIcon& get( const QString& file )
    { return get()._get( file ); }

    //! return cache
    static const BASE::IconCache& cache( void )
    { return get().cache_; }

    //! reload all icons set in cache from new path list
    bool reload( void );

    private:

    //!@name non static methods are hidden
    //@{

    //! constructor
    IconEngine( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    const BASE::IconCacheItem& _get( const QString&, bool fromCache = true );

    //@}

    //! pixmap path
    QStringList pixmapPath_;

    //! map files and QIcon
    BASE::IconCache cache_;

};

#endif
