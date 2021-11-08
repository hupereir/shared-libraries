#ifndef XmlFileRecord_h
#define XmlFileRecord_h

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

#include "FileRecord.h"
#include "base_qt_export.h"

#include <QDomElement>
#include <QDomDocument>
#include <QString>

namespace Base
{
    namespace Xml
    {
        static const QString FileList = QStringLiteral("FileList");
        static const QString SessionFileList = QStringLiteral("SessionFileList");
        static const QString LastSessionFileList = QStringLiteral("LastSessionFileList");
        static const QString PathHistory = QStringLiteral("PathHistory");
        static const QString Record = QStringLiteral("record");
    }
}

//* xml interface to FileRecord
class BASE_QT_EXPORT XmlFileRecord: public FileRecord
{

    public:

    //* constructor
    explicit XmlFileRecord( const FileRecord& file ):
        FileRecord( file )
    {}

    //* constructor
    explicit XmlFileRecord( const QDomElement& );

    //* write to dom
    QDomElement domElement( QDomDocument& ) const;

    using List = QList<XmlFileRecord>;
    class BASE_QT_EXPORT Helper
    {
        public:

        //* constructor
        static List list( const QDomElement& );

        //* write to dom
        static QDomElement domElement( const List&, QDomDocument& );

    };

};

#endif
