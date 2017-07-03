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
        static ConversionList conversions_;

        if( conversions_.empty() )
        {
            conversions_ << Conversion( "à", "\\`a" );
            conversions_ << Conversion( "â", "\\^a" );
            conversions_ << Conversion( "ä", "\\\"a" );
            conversions_ << Conversion( "é", "\\'e" );
            conversions_ << Conversion( "è", "\\`e" );
            conversions_ << Conversion( "ê", "\\^e" );
            conversions_ << Conversion( "ë", "\\\"e" );
            conversions_ << Conversion( "î", "\\^\\i " );
            conversions_ << Conversion( "ï", "\\\"\\i " );
            conversions_ << Conversion( "ô", "\\^o" );
            conversions_ << Conversion( "ö", "\\\"o" );
            conversions_ << Conversion( "ù", "\\`u" );
            conversions_ << Conversion( "û", "\\^u" );
            conversions_ << Conversion( "ç", "\\c c" );
            // (wtf) conversions_ << Conversion( "?", "-" );
        }

        return conversions_;

    }

}
