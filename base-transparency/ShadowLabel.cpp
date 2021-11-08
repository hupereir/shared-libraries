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

#include <QPaintEvent>

#include "ShadowLabel.h"
#include "Debug.h"
#include "Effects.h"

namespace Transparency
{
  //__________________________________________________________________________________
  void ShadowLabel::paintEvent( QPaintEvent* event )
  {

    if( text().isEmpty() ) return;

    QPixmap pixmap( size() );
    pixmap.fill( Qt::transparent );

    {
      QPainter painter( &pixmap );
      painter.setClipRect( event->rect() );
      painter.setPen( palette().color( QPalette::WindowText ) );
      painter.setFont( font() );
      painter.drawText( contentsRect(), alignment(), text() );
      painter.end();
    }

    QPainter painter( this );
    if( _shadowOffset() > 0 && _shadowColor().isValid() )
    {
      QImage image( pixmap.toImage() );
      Transparency::Effects::shadowBlur(image, _shadowOffset(), _shadowColor() );
      painter.drawImage( rect().topLeft(), image);
    }

    painter.drawPixmap( rect().topLeft(), pixmap );
    painter.end();

  }
}
