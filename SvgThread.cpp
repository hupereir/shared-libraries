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

namespace SVG
{

    //______________________________________________________
    SvgThread::SvgThread( QObject* parent ):
        QThread( parent )
    {
        // register FileRecord::List as meta type so that it can be used in SIGNAL
        qRegisterMetaType<SVG::ImageCache>( "SVG::ImageCache" );
    }

    //______________________________________________________
    void SvgThread::run( void )
    {

        if( !svg_.isValid() ) return;
        if( svgIds_.empty() ) return;

        svg_.updateConfiguration();
        cache_.clear();

        for( SvgId::List::const_iterator iter = svgIds_.begin(); iter != svgIds_.end(); ++iter )
        {

            QImage image( iter->size(), QImage::Format_ARGB32_Premultiplied );
            image.fill( Qt::transparent );
            svg_.render( image, iter->id() );
            cache_.insert( *iter, image );

        }

        emit imageCacheAvailable( cache_ );

        return;

    }

};
