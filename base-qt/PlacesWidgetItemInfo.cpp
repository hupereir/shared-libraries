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

#include "PlacesWidgetItemInfo.h"

#include "Debug.h"

namespace Xml
{ static const QString Flags = "flags"; };

//___________________________________________________________________
PlacesWidgetItemInfo::PlacesWidgetItemInfo( const QDomElement& element ):
BaseFileInfo( element )
{
    Debug::Throw( "PlacesWidgetItemInfo::PlacesWidgetItemInfo (dom).\n" );

    // parse attributes
    auto attributes( element.attributes() );
    for( int i=0; i<attributes.count(); i++ )
    {
        auto attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == Xml::Flags ) setFlags( (Flags) attribute.value().toInt() );
    }

}

//________________________________________________________________
QDomElement PlacesWidgetItemInfo::domElement( QDomDocument& document ) const
{
    Debug::Throw( "BaseFileInfo::DomElement.\n" );
    auto out( BaseFileInfo::domElement( document ) );
    if( flags_ ) out.setAttribute( Xml::Flags, flags_ );
    return out;
}

//___________________________________________________________________
PlacesWidgetItemInfo::List PlacesWidgetItemInfo::Helper::list( const QDomElement& element )
{

    PlacesWidgetItemInfo::List out;

    // read records
    for( auto node = element.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        auto element = node.toElement();
        if( element.isNull() ) continue;

        // children
        if( element.tagName() == Xml::FileInfo )
        {
            PlacesWidgetItemInfo fileInfo( element );
            if( fileInfo.file().isEmpty() ) fileInfo.setFlag( PlacesWidgetItemInfo::Separator, true );
            out.append( fileInfo );
        }
    }

    return out;

}

//___________________________________________________________________
QDomElement PlacesWidgetItemInfo::Helper::domElement( const PlacesWidgetItemInfo::List& list, QDomDocument& document )
{
    // create main element
    auto top = document.createElement( Xml::FileInfoList );
    for( const auto& fileInfo:list )
    { top.appendChild( fileInfo.domElement( document ) );  }
    return top;
}
