
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

#include "HtmlTextNode.h"
#include "Debug.h"

#include <QStringList>

//_________________________________________
HtmlTextNode::HtmlTextNode( const QString& text, QDomElement& parent, QDomDocument& document )
{

    Debug::Throw( "HtmlTextNode::HtmlTextNode.\n" );
    QStringList lines( text.split( '\n' ) );
    if( lines.empty() ) return;

    // first append the first line
    QStringList::iterator iter = lines.begin();
    parent.appendChild( document.createTextNode( *iter ) );
    ++iter;

    // append remaining lines, after end of line tag
    for( ;iter != lines.end(); ++iter )
    {
        parent.appendChild( document.createElement( "br" ) );
        parent.appendChild( document.createTextNode( *iter ) );
    }

    return;
}
