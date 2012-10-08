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

/*!
\file XmlString.cpp
\brief   Some Xml utilities
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Debug.h"
#include "XmlString.h"
#include "Str.h"
#include "iostream"

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
    _conversions() << Conversion( "à", "XML_AGRAVE" );
    _conversions() << Conversion( "â", "XML_AHAT" );
    _conversions() << Conversion( "é", "XML_ECUTE" );
    _conversions() << Conversion( "è", "XML_EGRAVE" );
    _conversions() << Conversion( "ê", "XML_EHAT" );
    _conversions() << Conversion( "î", "XML_IHAT" );
    _conversions() << Conversion( "ô", "XML_OHAT" );
    _conversions() << Conversion( "ù", "XML_UGRAVE" );
    _conversions() << Conversion( "ç", "XML_CCED" );
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
