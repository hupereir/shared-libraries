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
* Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*******************************************************************************/

#include "XmlMigration.h"

#include "Debug.h"
#include "XmlDocument.h"
#include "XmlOptions.h"

#include <QtCore/QFile>

//_________________________________________________________________________
bool XmlMigration::run( void )
{

    Debug::Throw( "XmlMigration::run.\n" );

    // check xml name
    if( xmlName_.isEmpty() )
    {
        Debug::Throw(0) << "XmlMigration::run - invalid option" << endl;
        return false;
    }

    // check whether there is already a node with requested name inside RC file
    if( _findXmlName() )
    {
        Debug::Throw(0) << "XmlMigration::run - found tag " << xmlName_ << " in resource file. Aborted." << endl;
        return false;
    }

    // define old file
    File oldFile;
    if( !oldOptionName_.isEmpty() && XmlOptions::get().contains( oldOptionName_ ) )
    { oldFile = File( XmlOptions::get().raw( oldOptionName_ ) ); }

    // check if old file exists
    if( !oldFile.exists() ) oldFile = oldFile_;
    if( !oldFile.exists() )
    {
        Debug::Throw(0) << "XmlMigration::run - could not find old file " << oldFile << ". Aborted." << endl;
        return false;
    }

    // read old element
    QDomElement oldElement;
    {
        QFile qfile( oldFile );
        XmlDocument document;
        if( !document.setContent( &qfile ) )
        {
            Debug::Throw(0) << "XmlMigration::run - could not read old file " << oldFile << ". Aborted." << endl;
            return false;
        }

        oldElement = document.documentElement();
    }

    Debug::Throw(0) << "XmlMigration::run - found element named: " << oldElement.tagName() << endl;

    // rename
    oldElement.setTagName( xmlName_ );

    // read rc file
    File rcFile( XmlOptions::get().raw( "RC_FILE" ) );
    XmlDocument document;
    {
        QFile qtfile( rcFile );
        document.setContent( &qtfile );
    }

    // assign renamed old element to rc file and write
    document.replaceChild( oldElement );
    {
        QFile qfile( rcFile );
        if( !qfile.open( QIODevice::WriteOnly ) )
        {
            Debug::Throw(0) << "XmlMigration::run - unable to write to " << rcFile << ". Aborted." << endl;
            return false;
        }
        qfile.write( document.toByteArray() );
    }

    // delete old file
    if( !oldFile.remove() )
    {
        Debug::Throw(0) << "XmlMigration::run - unable to remove old file " << oldFile << "." << endl;
        return false;
    }

    Debug::Throw(0) << "XmlMigration::run - succesfully performed migration." << endl;
    return true;

}

//_________________________________________________________________________
bool XmlMigration::_findXmlName( void ) const
{
    Debug::Throw( "XmlMigration::_findXmlName.\n" );
    QFile qfile( XmlOptions::get().raw( "RC_FILE" ) );
    XmlDocument document;
    if( !document.setContent( &qfile ) ) return false;
    else return !document.elementsByTagName( xmlName_).isEmpty();
}




