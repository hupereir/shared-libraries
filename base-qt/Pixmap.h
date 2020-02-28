#ifndef Pixmap_h
#define Pixmap_h

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

#include "base_qt_export.h"
#include "Counter.h"

#include <QPixmap>

//* customized QPixmap to look for icon of given name in list of directory
class BASE_QT_EXPORT Pixmap: public QPixmap, private Base::Counter<Pixmap>
{

    public:

    enum Flag
    {
        Transparent = 0x1
    };

    Q_DECLARE_FLAGS( Flags, Flag )

    //* constructor
    explicit Pixmap( QSize , Flags flags = 0 );

    //* constructor
    explicit Pixmap( const QPixmap& pixmap ):
        QPixmap( pixmap ),
        Counter( QStringLiteral("Pixmap") )
    {}

    //* constructor
    explicit Pixmap( QPixmap&& pixmap ):
        QPixmap( std::move(pixmap) ),
        Counter( QStringLiteral("Pixmap") )
    {}

    //* constructor
    explicit Pixmap( const QImage& image ):
        QPixmap( fromImage(image) ),
        Counter( QStringLiteral("Pixmap") )
    {}

    //* constructor
    explicit Pixmap():
        Counter( QStringLiteral("Pixmap") )
    {}

    //* constructor
    explicit Pixmap( const QString& );

    //* find first file matching name in list of path
    Pixmap& find( const QString& );

    //* rotation
    enum class Rotation
    {
        None,
        Clockwise,
        CounterClockwise
    };

    //* rotation
    Pixmap rotated( Pixmap::Rotation value ) const;

    //* returns a transparent pixmap
    Pixmap transparent( qreal ) const;

    //* returns a desaturated pixmap
    Pixmap desaturated() const;

    //* colorize
    Pixmap colorized( const QColor& ) const;

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
    Pixmap merged( const QPixmap&, Corner = Corner::TopLeft ) const;

    //* return highlighted pixmap
    Pixmap highlighted( qreal opacity ) const;

    //* return highlighted (active) pixmap, build from the current
    Pixmap active() const
    { return highlighted( 0.2 ); }

};


Q_DECLARE_OPERATORS_FOR_FLAGS( Pixmap::Flags )

#endif
