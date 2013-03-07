// $Id$
#ifndef XmlTextFormatBlock_h
#define XmlTextFormatBlock_h

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

#include "TextFormatBlock.h"
#include <QDomElement>
#include <QDomDocument>

namespace FORMAT
{
    namespace XML
    {

        static const QString TAG = "TextFormat";
//         static const QString FORMAT = "format";
//         static const QString COLOR = "color";
//         static const QString HREF = "href";
//         static const QString BEGIN = "begin";
//         static const QString END = "end";

    }

    //! text format with Xml abilities
    class XmlTextFormatBlock:public TextFormatBlock
    {

        public:

        //! constructor
        XmlTextFormatBlock( const QDomElement& element );

        //! constructor
        XmlTextFormatBlock( const TextFormatBlock& format ):
            TextFormatBlock( format )
        {}

        //! get dom element
        QDomElement domElement( QDomDocument& parent ) const;

    };

};

#endif
