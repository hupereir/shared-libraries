#ifndef XmlMigration_h
#define XmlMigration_h

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

#include "Counter.h"
#include "File.h"

#include <QString>

class XmlMigration: public Counter
{

    public:

    //* constructor
    XmlMigration( File oldFileName = File(), QString optionName = QString(), QString xmlName = QString() ):
        Counter( "XmlMigration" ),
        oldFile_( oldFileName ),
        oldOptionName_( optionName ),
        xmlName_( xmlName )
    {}

    //* old file name
    void setOldFileName( const File& value )
    { oldFile_ = value; }

    //* set old option name
    void setOldOptionName ( const QString& value )
    { oldOptionName_ = value; }

    //* set new Xml name
    void setNewXmlName( const QString& value )
    { xmlName_ = value; }

    //* do the migration
    bool run( void );

    protected:

    //* find XmlName in rcFile
    bool _findXmlName( void ) const;

    private:

    //* old filename
    File oldFile_;

    //* old option name
    QString oldOptionName_;

    //* new Xml name
    QString xmlName_;

};

#endif
