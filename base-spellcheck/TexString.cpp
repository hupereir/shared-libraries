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

#include "TexString.h"
#include "File.h"

namespace SpellCheck
{

    //________________________________________________________
    QString TexString::toLatexAccents() const
    {
        QString out( value_ );
        for( const auto& conversion:conversions() )
        { if( !conversion.first.isEmpty() ) out = out.replace( conversion.first, conversion.second ); }
        return out;
    }

    //________________________________________________________
    QString TexString::toTextAccents() const
    {
        QString out( value_ );
        for( const auto& conversion:conversions() )
        { if( !conversion.second.isEmpty() ) out = out.replace( conversion.second, conversion.first ); }
        return out;
    }

    //________________________________________________________
    const TexString::ConversionList& TexString::conversions()
    {
        static ConversionList conversions =
        {
             Conversion( "à", "\\`a" ),
             Conversion( "â", "\\^a" ),
             Conversion( "ä", "\\\"a" ),
             Conversion( "é", "\\'e" ),
             Conversion( "è", "\\`e" ),
             Conversion( "ê", "\\^e" ),
             Conversion( "ë", "\\\"e" ),
             Conversion( "î", "\\^\\i " ),
             Conversion( "ï", "\\\"\\i " ),
             Conversion( "ô", "\\^o" ),
             Conversion( "ö", "\\\"o" ),
             Conversion( "ù", "\\`u" ),
             Conversion( "û", "\\^u" ),
             Conversion( "ç", "\\c c" )
        };

        return conversions;

    }

}
