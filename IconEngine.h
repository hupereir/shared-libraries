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

/*!
\file IconEngine.h
\brief customized Icon factory to provide better looking disabled icons
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Counter.h"
#include "Debug.h"
#include "PixmapEngine.h"

#include <QtGui/QIcon>
#include <QtCore/QHash>
#include <QtCore/QPair>

#include <cassert>

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

    //! create icon
    static QIcon get( const QPixmap& pixmap )
    { return get()._get( pixmap ); }

    //! create icon
    static QIcon get( const QIcon& icon )
    { return get()._get( icon ); }

    //! map files and QIcon
    typedef QHash< QString, QIcon > Cache;

    //! map files and QIcon
    class Pair: public QPair<QString, QIcon >, public Counter
    {

        public:

        //! constructor
        Pair( void ):
            Counter( "IconEngine::Pair" )
        {}

        //! constructor
        Pair( const QPair<QString, QIcon >& pair ):
            QPair<QString, QIcon >( pair ),
            Counter( "IconEngine::Pair" )
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

    private:

    //!@name non static methods are hidden
    //@{

    //! constructor
    IconEngine( void );

    //! create icon
    /*! the file is stored into a cache to avoid all pixmaps manipulations */
    const QIcon& _get( const QString&, bool from_cache = true );

    //! create icon
    QIcon _get( const QPixmap& );

    //! create icon
    QIcon _get( const QIcon& );

    //@}

    //! map files and QIcon
    Cache cache_;

};

#endif
