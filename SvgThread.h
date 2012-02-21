#ifndef SvgThread_h
#define SvgThread_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*******************************************************************************/

#include "Debug.h"
#include "SvgRenderer.h"
#include "Svg.h"

#include <QtCore/QEvent>
#include <QtCore/QThread>

//! used to post a new grid when ready
namespace SVG
{

    class SvgEvent: public QEvent
    {

        public:

        //! constructor
        SvgEvent( const ImageCache& cache ):
            QEvent( eventType() ),
            cache_( cache )
        {}

        //! destructor
        ~SvgEvent( void )
        {}

        //! static event type
        static QEvent::Type eventType( void );

        //! records
        const ImageCache& cache() const
        { return cache_; }

        private:

        //! Svg cache
        ImageCache cache_;

    };

    //! independent thread used to automatically save file
    class SvgThread: public QThread
    {

        public:

        //! constructor
        SvgThread( QObject* reciever ):
            reciever_( reciever ),
            svgOffset_( 0 )
        {}

        //! svg file
        void setSvgFile( const QString& file )
        { svg_.load( file ); }

        //! offset
        void setSvgOffset( const double& value )
        { svgOffset_ = value; }

        //! set file
        void setSvgIdList( const SvgId::List& svg_ids )
        { svgIds_ = svg_ids; }

        //! Check files validity. Post a SvgEvent when finished
        void run( void );

        private:

        //! reciever object for posted events
        QObject* reciever_;

        //! svg renderer
        SvgRenderer svg_;

        //! svg offset
        double svgOffset_;

        //! requested sizes
        SvgId::List svgIds_;

    };

}

#endif
