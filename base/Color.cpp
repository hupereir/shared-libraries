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

#include "Color.h"

#include <QStringList>

namespace Base
{

    //__________________________________________________________
    Color& Color::merge( const QColor& other, qreal intensity )
    {
        if( !isValid() ) { *this = Color( other ); return *this; }
        if( !other.isValid() ) return *this;
        if( other == color_ ) return *this;

        color_.setRed( intensity*color_.red() + (1.0-intensity )*other.red() );
        color_.setGreen( intensity*color_.green() + (1.0-intensity )*other.green() );
        color_.setBlue( intensity*color_.blue() + (1.0-intensity )*other.blue() );
        color_.setAlpha( intensity*color_.alpha() + (1.0-intensity )*other.alpha() );
        return *this;

    }

    //__________________________________________________________
    Color& Color::addAlpha( qreal intensity )
    {
        color_.setAlpha( intensity*color_.alpha() );
        return *this;
    }

    //__________________________________________________________
    bool Color::operator < (const Color& other ) const
    {
        if( color_.red() != other.color_.red() ) return color_.red() < other.color_.red();
        else if( color_.green() != other.color_.green() ) return color_.green() < other.color_.green();
        else if( color_.blue() != other.color_.blue() ) return color_.blue() < other.color_.blue();
        else if( color_.alpha() != other.color_.alpha() ) return color_.alpha() < other.color_.alpha();
        else return false;
    }
}

//__________________________________________________________
QTextStream& operator << (QTextStream& out, const Base::Color& color )
{
    out << color.get().red() << "," << color.get().green() << "," << color.get().blue() << "," << color.get().alpha();
    return out;
}

//__________________________________________________________
QTextStream& operator >> (QTextStream& in, Base::Color& color )
{
    QString colorString;
    in >> colorString;
    const QStringList stringList( colorString.split( "," ) );
    if( stringList.size() >= 3 )
    {
        color.get().setRed( stringList[0].toInt() );
        color.get().setGreen( stringList[1].toInt() );
        color.get().setBlue( stringList[2].toInt() );

        if( stringList.size() >= 4 )  color.get().setAlpha( stringList[3].toInt() );
        else color.get().setAlpha( 255 );

    } else {

        color.get().setNamedColor( colorString );

    }

    return in;
}
