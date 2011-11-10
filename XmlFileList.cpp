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

/*!
\file XmlFileList.cpp
\brief Xml interface to FileList
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QApplication>
#include <QFile>

#include "Debug.h"
#include "Singleton.h"
#include "XmlFileList.h"
#include "XmlFileRecord.h"
#include "XmlOptions.h"
#include "XmlError.h"



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
bool XmlFileList::_setDBFile( const QString& file )
{
    Debug::Throw() << "XmlFileList::_setDBFile - file: " << file << endl;

    // check file
    if( dbFile_ == file && !_records().empty() ) return false;

    // store file and read
    dbFile_ = file;
    _read();

    return true;

}

//_______________________________________________
bool XmlFileList::_read( void )
{
    Debug::Throw( "XmlFileList::_read.\n" );
    if( dbFile_.isEmpty() || !QFileInfo(dbFile_).exists() ) return false;

    // parse the file
    QFile in( dbFile_ );
    if ( !in.open( QIODevice::ReadOnly ) )
    {
        Debug::Throw( "XmlFileList::_read - cannot open file.\n" );
        return false;
    }

    // dom document
    QDomDocument document;
    XmlError error( dbFile_ );
    if ( !document.setContent( &in, &error.error(), &error.line(), &error.column() ) ) {
        in.close();
        Debug::Throw() << error << endl;
        return _deprecatedRead();
    }

    QDomElement doc_element = document.documentElement();
    QDomNode node = doc_element.firstChild();
    for(QDomNode node = doc_element.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == XmlFileRecord::XML_RECORD )
        {

            XmlFileRecord record( element );
            if( !record.file().isEmpty() ) _add( record, true, false );
            else Debug::Throw(0, "XmlFileList::_read - attend to add empty record. Discarded.\n" );

        } else Debug::Throw() << "XmlFileList::_read - unrecognized tag " << element.tagName() << endl;
    }

    emit contentsChanged();

    return true;
}

//_______________________________________________
bool XmlFileList::_write( void )
{
    Debug::Throw( "XmlFileList::_write.\n" );
    if( dbFile_.isEmpty() )
    {
        Debug::Throw( "XmlFileList::_write - no file.\n" );
        return false;
    }

    // output file
    QFile out( dbFile_ );
    if( !out.open( QIODevice::WriteOnly ) ) return false;

    // get records truncated list
    FileRecord::List records( _truncatedList( _records() ) );

    // create document
    QDomDocument document;

    // create main element
    QDomElement top = document.appendChild( document.createElement( XmlFileRecord::XML_FILE_LIST ) ).toElement();

    // loop over records
    for( FileRecord::List::const_iterator iter = records.begin(); iter != records.end(); ++iter )
    {

        Debug::Throw() << "XmlFileList::_write - " << *iter;
        if( iter->file().isEmpty() )
        {
            Debug::Throw(0, "XmlFileList::_write - attend to write empty record. Discarded.\n" );
            continue;
        }

        top.appendChild( XmlFileRecord( *iter ).domElement( document ) );
    }

    out.write( document.toByteArray() );
    out.close();

    return true;
}


//______________________________________
void XmlFileList::_updateConfiguration( void )
{
    Debug::Throw( "XmlFileList::_updateConfiguration.\n" );

    // DB file
    _setDBFile( XmlOptions::get().raw("DB_FILE") );
    _setMaxSize( XmlOptions::get().get<int>( "DB_SIZE" ) );
    return;

}

//______________________________________
void XmlFileList::_saveConfiguration( void )
{
    Debug::Throw( "XmlFileList::_saveConfiguration.\n" );
    _write();
}

//_______________________________________________
bool XmlFileList::_deprecatedRead( void )
{
    Debug::Throw( "FileList::_deprecatedRead.\n" );

    // check file
    if( dbFile_.isEmpty() || !QFileInfo(dbFile_).exists() ) return false;

    // open stream
    QFile in( dbFile_ );
    if( !in.open( QIODevice::ReadOnly ) ) return false;

    while( in.canReadLine() )
    {
        QString line( in.readLine( 1024 ) );
        _add( FileRecord( File( line ) ), true, false );
    }

    in.close();

    emit contentsChanged();

    return true;
}
