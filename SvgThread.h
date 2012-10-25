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

#include "SvgRenderer.h"
#include "Svg.h"

#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

//! used to post a new grid when ready
namespace SVG
{

    //! independent thread used to automatically save file
    class SvgThread: public QThread
    {

        Q_OBJECT

        public:

        //! constructor
        SvgThread( QObject* );

        //! svg file
        void setSvgFile( const QString& file )
        {
            QMutexLocker lock( &mutex_ );
            svg_.load( file );
        }

        //! offset
        void setSvgOffset( const double& value )
        {
            QMutexLocker lock( &mutex_ );
            svgOffset_ = value;
        }

        //! set file
        void setSvgIdList( const SvgId::List& svgIds )
        {
            QMutexLocker lock( &mutex_ );
            svgIds_ = svgIds;
        }

        signals:

        //! image cache available
        void imageCacheAvailable( const SVG::ImageCache& );

        protected:

        //! Check files validity. Post a SvgEvent when finished
        virtual void run( void );

        private:

        //! mutex
        QMutex mutex_;

        //! svg renderer
        SvgRenderer svg_;

        //! svg offset
        double svgOffset_;

        //! requested sizes
        SvgId::List svgIds_;

        //! image cache
        ImageCache cache_;

    };

}

#endif
