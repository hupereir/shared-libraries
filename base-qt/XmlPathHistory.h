#ifndef XmlPathHistory_h
#define XmlPathHistory_h

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

#include "File.h"
#include "PathHistory.h"
#include "base_qt_export.h"

#include <QString>

// forward declaration
class XmlDocument;

//* Xml interface to PathHistory
class BASE_QT_EXPORT XmlPathHistory: public PathHistory
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constuctor
    explicit XmlPathHistory( QObject* parent = nullptr );

    // set tag name
    void setTagName( const QString& );

    //* Read fileList from file
    bool read( File = File() );

    //* write fileList to file
    bool write( File = File() );

    protected:

    //* set db file
    bool _setDBFile( const File& );

    //* read
    bool _read( const XmlDocument& );

    private:

    //* update configuration
    void _updateConfiguration();

    //* save configuration
    void _saveConfiguration();

    //* compare two record lists
    bool _differs( const FileRecord::List&, const FileRecord::List& ) const;

    //* tag name
    QString tagName_;

    //* enable history save
    bool saveHistory_ = false;

    //* file from/to wich the files are saved
    File dbFile_;

};
#endif
