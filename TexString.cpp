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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "TexString.h"
#include "File.h"

namespace SPELLCHECK
{

    //________________________________________________________
    QString TexString::toLatexAccents( void ) const
    {
        QString out( *this );
        foreach( const Conversion& conversion, conversions() )
        { if( !conversion.first.isEmpty() ) out = out.replace( conversion.first, conversion.second ); }
        return out;
    }

    //________________________________________________________
    QString TexString::toTextAccents( void ) const
    {
        QString out( *this );
        foreach( const Conversion& conversion, conversions() )
        { if( !conversion.second.isEmpty() ) out = out.replace( conversion.second, conversion.first ); }
        return out;
    }

    //________________________________________________________
    const TexString::ConversionList& TexString::conversions( void )
    {
        static ConversionList conversions_;

        if( conversions_.empty() )
        {
            conversions_ << Conversion( "�", "\\`a" );
            conversions_ << Conversion( "�", "\\^a" );
            conversions_ << Conversion( "�", "\\\"a" );
            conversions_ << Conversion( "�", "\\'e" );
            conversions_ << Conversion( "�", "\\`e" );
            conversions_ << Conversion( "�", "\\^e" );
            conversions_ << Conversion( "�", "\\\"e" );
            conversions_ << Conversion( "�", "\\^\\i " );
            conversions_ << Conversion( "�", "\\\"\\i " );
            conversions_ << Conversion( "�", "\\^o" );
            conversions_ << Conversion( "�", "\\\"o" );
            conversions_ << Conversion( "�", "\\`u" );
            conversions_ << Conversion( "�", "\\^u" );
            conversions_ << Conversion( "�", "\\c c" );
            // (wtf) conversions_ << Conversion( "?", "-" );
        }

        return conversions_;

    }

}
