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

#include "SvgThread.h"
#include "File.h"

#include <QMetaType>
#include <algorithm>

namespace Svg
{

    //______________________________________________________
    SvgThread::SvgThread( QObject* parent ):
        QThread( parent )
    {
        // register FileRecord::List as meta type so that it can be used in SIGNAL
        qRegisterMetaType<Svg::ImageCache>( "Svg::ImageCache" );
    }

    //______________________________________________________
    void SvgThread::run( void )
    {

        if( !renderer_.isValid() ) return;
        if( svgIds_.empty() ) return;

        renderer_.updateConfiguration();
        cache_.clear();

        for( const auto& svg:svgIds_ )
        {

            QImage image( svg.size(), QImage::Format_ARGB32_Premultiplied );
            image.fill( Qt::transparent );
            renderer_.render( image, svg.id() );
            cache_.insert( svg, image );

        }

        emit imageCacheAvailable( cache_ );

        return;

    }

};
