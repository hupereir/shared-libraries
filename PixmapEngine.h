#ifndef PixmapEngine_h
#define PixmapEngine_h

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

#include <QtGui/QPixmap>
#include <QtCore/QMap>
#include <QtCore/QVector>

#include <cassert>

//! customized Icon factory to provide better looking disabled icons
class PixmapEngine: public Counter
{

    public:

    //! retrieve singleton
    static PixmapEngine& get( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    static const QPixmap& get( const QString& file, bool from_cache = true )
    { return get()._get( file, from_cache ); }

    //! map files and QPixmap
    typedef QMap< QString, QPixmap > Cache;

    //! map files and QPixmap
    class Pair: public QPair<QString, QPixmap >, public Counter
    {

        public:

        //! constructor
        Pair( void ):
            Counter( "PixmapEngine::Pair" )
        {}

        //! constructor
        Pair( const QString& key, const QPixmap& value ):
            QPair<QString, QPixmap>( key, value ),
            Counter( "PixmapEngine::Pair" )
        {}

        //! constructor
        Pair( const QPair<QString, QPixmap >& pair ):
            QPair<QString, QPixmap >( pair ),
            Counter( "PixmapEngine::Pair" )
        {}

        //! equal to operator
        bool operator == ( const Pair& pair ) const
        { return first == pair.first; }

        //! less than operator
        bool operator < ( const Pair& pair ) const
        { return first < pair.first; }

    };

    //! return cache
    static const Cache& cache( void )
    { return get().cache_; }

    //! reload all icons set in cache from new path list
    bool reload( void );

    protected:

    //! pixmap path
    void _setPixmapPath( const QVector< QString >& pathList )
    { pixmapPath_ = pathList; }

    //! pixmap path
    const QVector< QString >& _pixmapPath( void ) const
    { return pixmapPath_; }

    private:

    //!@name non static methods are hidden
    //@{

    //! constructor
    PixmapEngine( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    const QPixmap& _get( const QString& file, bool from_cache );

    //@}

    //! pixmap path
    QVector< QString > pixmapPath_;

    //! map files and QPixmap
    Cache cache_;

};

#endif
