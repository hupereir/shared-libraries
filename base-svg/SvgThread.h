#ifndef SvgThread_h
#define SvgThread_h

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

#include "base_svg_export.h"
#include "SvgRenderer.h"
#include "Svg.h"

#include <QMutex>
#include <QMutexLocker>
#include <QPalette>
#include <QThread>

//* used to post a new grid when ready
namespace Svg
{

    //* independent thread used to automatically save file
    class BASE_SVG_EXPORT SvgThread: public QThread
    {

        Q_OBJECT

        public:

        //* constructor
        explicit SvgThread( QObject* );

        //* create style sheet
        void createStyleSheet( QPalette palette )
        {
            QMutexLocker lock( &mutex_ );
            renderer_.createStyleSheet( palette );
        }

        //* svg file
        void setSvgFile( QString file )
        {
            QMutexLocker lock( &mutex_ );
            renderer_.load( file );
        }

        //* set file
        void setSvgIdList( const SvgId::List& svgIds )
        {
            QMutexLocker lock( &mutex_ );
            svgIds_ = svgIds;
        }

        Q_SIGNALS:

        //* image cache available
        void imageCacheAvailable( Svg::ImageCache );

        protected:

        //* Check files validity. Post a SvgEvent when finished
        void run() override;

        private:

        //* mutex
        QMutex mutex_;

        //* svg renderer
        SvgRenderer renderer_;

        //* requested sizes
        SvgId::List svgIds_;

        //* image cache
        ImageCache cache_;

    };

}

#endif
