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
  \file XmlTextFormatBlock.cpp
  \brief tracks text format in given text range
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "Str.h"
#include "XmlTextFormatBlock.h"

using namespace std;
using namespace FORMAT;

//____________________________________
const QString XmlTextFormatBlock::XML_TAG = "TextFormat";
const QString XmlTextFormatBlock::XML_FORMAT = "format";
const QString XmlTextFormatBlock::XML_COLOR = "color";

const QString XmlTextFormatBlock::XML_BEGIN = "begin";
const QString XmlTextFormatBlock::XML_END = "end";

// obsolete tag names
const QString XmlTextFormatBlock::XML_BEGIN_PAR = "begin_par";
const QString XmlTextFormatBlock::XML_BEGIN_INDEX = "begin_index";
const QString XmlTextFormatBlock::XML_END_PAR = "end_par";
const QString XmlTextFormatBlock::XML_END_INDEX = "end_index";

//____________________________________
XmlTextFormatBlock::XmlTextFormatBlock( const QDomElement& element )
{

  // parse attributes
  QDomNamedNodeMap attributes( element.attributes() );
  for( unsigned int i=0; i<attributes.length(); i++ )
  {
    QDomAttr attribute( attributes.item( i ).toAttr() );
    if( attribute.isNull() ) continue;
    QString name( attribute.name() );
    QString value( attribute.value() );

    // nominal tags
    if( name == XML_BEGIN ) begin() = value.toInt();
    else if( name == XML_END ) end() = value.toInt();

    // obsolete paragraph tags
    else if( name == XML_BEGIN_PAR ) _parBegin() = value.toInt();
    else if( name == XML_END_PAR ) _parEnd() = value.toInt();

    // obsolete index tags
    else if( name == XML_BEGIN_INDEX ) begin() = value.toInt();
    else if( name == XML_END_INDEX ) end() = value.toInt();

    // format
    else if( name == XML_FORMAT ) format() =value.toInt();
    else if( name == XML_COLOR ) color() = value;

    else Debug::Throw(0) << "XmlTextFormatBlock::XmlTextFormatBlock - unrecognized text format attribute: \"" << name << "\"\n";
  }

}

//__________________________________________________________
QDomElement XmlTextFormatBlock::domElement( QDomDocument& parent ) const
{
  QDomElement out( parent.createElement( XML_TAG ) );
  out.setAttribute( XML_BEGIN, Str().assign<int>(begin()) );
  out.setAttribute( XML_END, Str().assign<int>(end()) );
  out.setAttribute( XML_FORMAT, Str().assign<unsigned int>(format()) );
  out.setAttribute( XML_COLOR, color() );
  return out;
}
