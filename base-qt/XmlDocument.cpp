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

#include "XmlDocument.h"

#include "Debug.h"
#include "XmlError.h"

//___________________________________________________________________
XmlDocument::XmlDocument():
    Counter( QStringLiteral("XmlDocument") ),
    topNodeTagName_( QStringLiteral("Resources") )
{}

//___________________________________________________________________
void XmlDocument::replaceChild( QDomElement& element )
{

    if( document_.documentElement().tagName() != topNodeTagName_ )
    {
        document_.clear();
        document_.appendChild( document_.createElement( topNodeTagName_ ) );
        document_.documentElement().appendChild( element );
        return;
    }

    // find previous options element
    auto&& children( document_.elementsByTagName( element.tagName() ) );
    if( !children.isEmpty() )
    {
        document_.documentElement().replaceChild( element, children.at(0) );
    } else {
        document_.documentElement().appendChild( element );
    }

    return;

}
