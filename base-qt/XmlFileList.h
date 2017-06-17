#ifndef XmlFileList_h
#define XmlFileList_h

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

#include "FileList.h"
#include "File.h"

#include <QString>

// forward declaration
class XmlDocument;

//* Xml interface to FileList
class XmlFileList: public FileList
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constuctor
    explicit XmlFileList( QObject* parent = nullptr );

    //*@name modifiers
    //@{

    // set tag name
    void setTagName( const QString& );

    //* set db file
    bool setDBFile( const File& );

    //@}

    public Q_SLOTS:

    //* Read fileList from file
    bool read( File = File() );

    //* write fileList to file
    bool write( File = File() );

    protected:

    //* read
    bool _read( const XmlDocument& );

    private:

    //* compare two record lists
    bool _differs( const FileRecord::List&, const FileRecord::List& ) const;

    //* tag name
    QString tagName_;

    //* file from/to wich the files are saved
    File dbFile_;

};
#endif
