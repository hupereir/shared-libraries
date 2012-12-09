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
* FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "XmlFileList.h"

#include "Debug.h"
#include "Singleton.h"
#include "XmlDocument.h"
#include "XmlFileRecord.h"
#include "XmlOptions.h"
#include "XmlError.h"

#include <QtGui/QApplication>
#include <QtCore/QFile>

//_______________________________________________
XmlFileList::XmlFileList( QObject* parent ):
    FileList( parent )
{

    Debug::Throw( "XmlFileList::XmlFileList.\n" );
    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _saveConfiguration() ) );
    _updateConfiguration();

}

//_______________________________________________
bool XmlFileList::read( File file )
{
    Debug::Throw( "XmlFileList::read.\n" );

    if( file.isEmpty() ) file = dbFile_;
    if( file.isEmpty() || !file.exists() ) return false;

    // parse the file
    XmlDocument document;
    {
        QFile qfile( file );
        XmlError error( file );
        if ( !document.setContent( &qfile, &error.error(), &error.line(), &error.column() ) )
        {
            Debug::Throw() << error << endl;
            return false;
        }
    }

    // look for relevant element
    QDomNodeList topNodes = document.elementsByTagName( FILERECORD::XML::FILE_LIST );
    if( topNodes.isEmpty() ) return false;
    for(QDomNode node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == FILERECORD::XML::RECORD )
        {

            XmlFileRecord record( element );
            if( !record.file().isEmpty() ) _add( record, true, false );

        }
    }

    emit contentsChanged();

    return true;
}

//_______________________________________________
bool XmlFileList::write( File file )
{
    Debug::Throw( "XmlFileList::write.\n" );
    if( file.isEmpty() ) file = dbFile_;
    if( file.isEmpty() ) return false;

    // create document and read
    XmlDocument document;
    {
        QFile qtfile( file );
        document.setContent( &qtfile );
    }

    // get records truncated list
    FileRecord::List records( _truncatedList( _records() ) );

    // create main element and insert records
    QDomElement top = document.createElement( FILERECORD::XML::FILE_LIST );
    foreach( const FileRecord& record, records )
    {

        Debug::Throw() << "XmlFileList::write - " << record;
        if( record.file().isEmpty() )
        {
            Debug::Throw(0, "XmlFileList::write - attend to write empty record. Discarded.\n" );
            continue;
        }

        top.appendChild( XmlFileRecord( record ).domElement( document ) );

    }

    // append top node to document and write
    document.replaceChild( top );
    {
        QFile qfile( file );
        if( !qfile.open( QIODevice::WriteOnly ) ) return false;
        qfile.write( document.toByteArray() );
    }

    return true;
}

//_______________________________________________
bool XmlFileList::_setDBFile( const File& file )
{
    Debug::Throw() << "XmlFileList::_setDBFile - file: " << file << endl;

    // check file
    if( dbFile_ == file && !_records().isEmpty() ) return false;

    // store file and read
    dbFile_ = file;

    // make sure file is hidden (windows only)
    if( dbFile_.localName().startsWith( '.' ) )
    { dbFile_.setHidden(); }

    read();

    return true;

}

//______________________________________
void XmlFileList::_updateConfiguration( void )
{
    Debug::Throw( "XmlFileList::_updateConfiguration.\n" );

    // DB file
    _setDBFile( File( XmlOptions::get().raw("RC_FILE") ) );
    _setMaxSize( XmlOptions::get().get<int>( "DB_SIZE" ) );
    return;

}

//______________________________________
void XmlFileList::_saveConfiguration( void )
{
    Debug::Throw( "XmlFileList::_saveConfiguration.\n" );
    write();
}
