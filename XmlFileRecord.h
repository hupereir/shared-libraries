#ifndef XmlFileRecord_h
#define XmlFileRecord_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "FileRecord.h"

#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QtCore/QString>

namespace FILERECORD
{
    namespace XML
    {
        static const QString FILE_LIST = "FileList";
        static const QString RECORD = "record";
        static const QString FILE = "file";
        static const QString TIME = "time";
        static const QString FLAGS = "flags";
        static const QString VALID= "valid";
        static const QString PROPERTY= "property";
        static const QString NAME= "name";
        static const QString VALUE= "value";
    }
}

//! xml interface to FileRecord
class XmlFileRecord: public FileRecord
{

    public:

    //! constructor
    XmlFileRecord( const FileRecord& file ):
        FileRecord( file )
    {}

    //! constructor
    XmlFileRecord( const QDomElement& dom );

    //! write to dom
    QDomElement domElement( QDomDocument& parent ) const;

};

#endif
