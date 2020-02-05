#ifndef CustomPixmap_h
#define CustomPixmap_h

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

#include "Counter.h"

#include <QPixmap>

//* customized QPixmap to look for icon of given name in list of directory
class CustomPixmap: public QPixmap, private Base::Counter<CustomPixmap>
{

    public:

    enum Flag
    {
        Transparent = 0x1
    };

    Q_DECLARE_FLAGS( Flags, Flag )

    //* constructor
    explicit CustomPixmap( const QSize&, Flags flags = 0 );

    //* constructor
    explicit CustomPixmap( const QPixmap& pixmap ):
        QPixmap( pixmap ),
        Counter( QStringLiteral("CustomPixmap") )
    {}

    //* constructor
    explicit CustomPixmap( QPixmap&& pixmap ):
        QPixmap( std::move(pixmap) ),
        Counter( QStringLiteral("CustomPixmap") )
    {}

    //* constructor
    explicit CustomPixmap( const QImage& image ):
        QPixmap( fromImage(image) ),
        Counter( QStringLiteral("CustomPixmap") )
    {}

    //* constructor
    explicit CustomPixmap():
        Counter( QStringLiteral("CustomPixmap") )
    {}

    //* constructor
    explicit CustomPixmap( const QString& );

    //* find first file matching name in list of path
    CustomPixmap& find( const QString& );

    //* rotation
    enum class Rotation
    {
        None,
        Clockwise,
        CounterClockwise
    };

    //* rotation
    CustomPixmap rotated( const Rotation& value ) const;

    //* returns a transparent pixmap
    CustomPixmap transparent( qreal ) const;

    //* returns a desaturated pixmap
    CustomPixmap desaturated() const;

    //* colorize
    CustomPixmap colorized( const QColor& ) const;

    //* corner enumeration for merging pixmap
    enum class Corner
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
        Center
    };

    //* merge pixmap, using the specified corner as an anchor
    CustomPixmap merged( const QPixmap&, Corner = Corner::TopLeft ) const;

    //* return highlighted pixmap
    CustomPixmap highlighted( qreal opacity ) const;

    //* return highlighted (active) pixmap, build from the current
    CustomPixmap active() const
    { return highlighted( 0.2 ); }

    //* device pixel ratio
    qreal devicePixelRatio() const;

    //* device pixel ratio
    void setDevicePixelRatio( qreal );

};


Q_DECLARE_OPERATORS_FOR_FLAGS( CustomPixmap::Flags )

#endif
