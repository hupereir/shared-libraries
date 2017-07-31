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

#include "ImageData.h"

namespace Notifications
{

    //____________________________________________________________
    ImageData::ImageData( QImage image )
    {
        image = image.convertToFormat( QImage::Format_ARGB32 );
        width = image.width();
        height = image.height();
        rowStride = image.bytesPerLine();
        hasAlpha = image.hasAlphaChannel();
        channels = hasAlpha ? 4:3;
        bitsPerSample = image.depth()/channels;
        data.append( reinterpret_cast<char*>(image.rgbSwapped().bits()), rowStride*height );
    }

}
