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
class CustomPixmap: public QPixmap, public Counter
{

    public:

    enum Flag
    {
        Transparent = 0x1,
        HighDpi = 0x2,
        AllFlags = 0x3
    };

    Q_DECLARE_FLAGS( Flags, Flag )

    //* constructor
    CustomPixmap( const QSize&, Flags flags = 0 );

    //* constructor
    CustomPixmap( const QPixmap& pixmap ):
        QPixmap( pixmap ),
        Counter( "CustomPixmap" )
    {}

    //* constructor
    CustomPixmap( const QImage& image ):
        Counter( "CustomPixmap" )
    { *this = fromImage( image ); }

    //* constructor
    CustomPixmap( void ):
        Counter( "CustomPixmap" )
    {}

    //* constructor
    CustomPixmap( const QString& );

    //* find first file matching name in list of path
    virtual CustomPixmap find( const QString& );

    //* rotation
    enum Rotation
    {
        None,
        Clockwise,
        CounterClockwise
    };

    //* rotation
    CustomPixmap rotate( const Rotation& value );

    //* returns a transparent pixmap
    virtual CustomPixmap transparent( qreal ) const;

    //* returns a desaturated pixmap
    virtual CustomPixmap desaturate( void ) const;

    //* corner enumeration for merging pixmap
    enum Corner
    {
        TOP_LEFT,
        TOP_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_RIGHT,
        CENTER
    };

    //* merge pixmap, using the specified corner as an anchor
    virtual CustomPixmap merge( const CustomPixmap&, Corner corner = TOP_LEFT ) const;

    //* return highlighted pixmap
    virtual CustomPixmap highlighted( qreal opacity ) const;

    //* return highlighted (active) pixmap, build from the current
    virtual CustomPixmap active( void ) const
    { return highlighted( 0.2 ); }

    //* device pixel ratio
    qreal devicePixelRatio( void ) const;

    //* device pixel ratio
    void setDevicePixelRatio( qreal );

};


Q_DECLARE_OPERATORS_FOR_FLAGS( CustomPixmap::Flags )

#endif
