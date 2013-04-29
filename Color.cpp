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

namespace BASE
{

    //__________________________________________________________
    QColor Color::merge( const QColor& other, qreal intensity ) const
    {
        if( !isValid() ) return other;
        if( !other.isValid() ) return *this;
        if( other == *this ) return *this;

        double red = intensity*Color::red() + (1.0-intensity )*other.red();
        double green = intensity*Color::green() + (1.0-intensity )*other.green();
        double blue = intensity*Color::blue() + (1.0-intensity )*other.blue();
        double alpha = intensity*Color::alpha() + (1.0-intensity )*other.alpha();

        return QColor( int( red ), int( green ), int( blue ), int( alpha ) );
    }

    //__________________________________________________________
    QColor Color::addAlpha( qreal intensity ) const
    {
        QColor out( *this );
        out.setAlpha( intensity*alpha() );
        return out;
    }

    //__________________________________________________________
    bool Color::operator < (const Color& other ) const
    {
        if( red() != other.red() ) return red() < other.red();
        else if( green() != other.green() ) return green() < other.green();
        else if( blue() != other.blue() ) return blue() < other.blue();
        else if( alpha() != other.alpha() ) return alpha() < other.alpha();
        else return false;
    }

}

//__________________________________________________________
QTextStream& operator << (QTextStream& out, const BASE::Color& color )
{
    out << color.red() << "," << color.green() << "," << color.blue() << "," << color.alpha();
    return out;
}

//__________________________________________________________
QTextStream& operator >> (QTextStream& in, BASE::Color& color )
{
    QString colorString;
    in >> colorString;
    const QStringList stringList( colorString.split( "," ) );
    if( stringList.size() >= 3 )
    {
        color.setRed( stringList[0].toInt() );
        color.setGreen( stringList[1].toInt() );
        color.setBlue( stringList[2].toInt() );

        if( stringList.size() >= 4 )  color.setAlpha( stringList[3].toInt() );
        else color.setAlpha( 255 );

    } else {

        color.setNamedColor( colorString );

    }

    return in;
}
