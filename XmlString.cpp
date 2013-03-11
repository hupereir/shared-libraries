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

#include "XmlString.h"

//__________________________________
XmlString::ConversionList& XmlString::_conversions( void )
{
    static ConversionList conversions;
    return conversions;
}

//__________________________________
void XmlString::_initConversions() const
{

    _conversions() << Conversion( "<", "XML_LT" );
    _conversions() << Conversion( ">", "XML_GT" );
    _conversions() << Conversion( "&", "XML_AND" );
    _conversions() << Conversion( "\"", "XML_QUOTE" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_AGRAVE" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_AHAT" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_ATREM" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_ECUTE" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_EGRAVE" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_EHAT" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_EYTRM" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_IHAT" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_ITREM" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_OHAT" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_OTREM" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_UGRAVE" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_UHAT" );
    _conversions() << Conversion( QString::fromLatin1( "�" ), "XML_CCED" );
    _conversions() << Conversion( "\t", "XML_TAB" );
    _conversions() << Conversion( "\n", "XML_ENDL" );

    // this conversion is needed for XML not to remove entries that consist of empty spaces only
    // it is used in xmlToText but not in textToXml
    _conversions() << Conversion( "", "XML_NONE" );

    return;

}

//__________________________________
QString XmlString::toXml( void ) const
{

    QString out(*this);
    if( !( _conversions().size() ) ) _initConversions();
    for( ConversionList::iterator iter = _conversions().begin(); iter != _conversions().end(); ++iter )
    { if( !iter->first.isEmpty() ) out = out.replace( iter->first, iter->second ); }

    return out;
}

//__________________________________
QString XmlString::toText( void ) const
{

    QString out(*this);
    if( !( _conversions().size() ) ) _initConversions();

    // HTML style conversions (escape characters)
    ConversionListIterator iter( _conversions() );
    iter.toBack();
    while( iter.hasPrevious() )
    {
        const Conversion& current( iter.previous() );
        if( !current.second.isEmpty() ) out = out.replace( current.second, current.first ); }

    return out;
}
