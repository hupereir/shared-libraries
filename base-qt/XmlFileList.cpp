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

#include "XmlFileList.h"

#include "Debug.h"
#include "Operators.h"
#include "XmlDocument.h"
#include "XmlFileRecord.h"

#include <QApplication>
#include <QFile>

//_______________________________________________
XmlFileList::XmlFileList( QObject* parent ):
    FileList( parent ),
    tagName_( Base::Xml::FileList )
{

    Debug::Throw( "XmlFileList::XmlFileList.\n" );
    connect( qApp, SIGNAL(aboutToQuit()), SLOT(write()) );

}

//_______________________________________________
void XmlFileList::setTagName( const QString& value )
{

    // do nothing if tagName is unchanged
    if( value == tagName_ ) return;

    // update tag name and read
    tagName_ = value;
    clear();
    read();

}

//_______________________________________________
bool XmlFileList::setDBFile( const File& file )
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

//_______________________________________________
bool XmlFileList::read( File file )
{
    Debug::Throw( "XmlFileList::read.\n" );

    if( file.isEmpty() ) file = dbFile_;
    if( file.isEmpty() || !file.exists() ) return false;

    // parse the file
    XmlDocument document;
    QFile qfile( file );
    if ( !document.setContent( &qfile ) )
    {

        Debug::Throw() << document.error() << endl;
        return false;

    } else if( _read( document ) ) {

        emit contentsChanged();
        return true;

    } else return false;

}

//_______________________________________________
bool XmlFileList::_read( const XmlDocument& document )
{

    // look for relevant element
    const auto topNodes = document.elementsByTagName( tagName_ );
    if( topNodes.isEmpty() ) return false;
    for( auto node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        const auto element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == Base::Xml::Record )
        {

            XmlFileRecord record( element );
            if( !record.file().isEmpty() ) _add( record, true, false );

        }
    }

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
    auto records( _truncatedList( _records() ) );

    // read old list and compare to current
    XmlFileList oldList;
    if( oldList._read( document ) && !_differs( oldList._records(), records ) )
    { return true; }

    // create main element and insert records
    auto top = document.createElement( tagName_ );
    for( const auto& record:records )
    {

        if( !record.file().isEmpty() )
        { top.appendChild( XmlFileRecord( record ).domElement( document.get() ) ); }

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

//______________________________________
bool XmlFileList::_differs( const FileRecord::List& first, const FileRecord::List& second ) const
{

    // check sizes
    if( first.size() != second.size() ) return true;

    // check records one by one
    for( int i = 0; i < first.size(); ++i )
    {

        // records
        const auto& firstRecord = first[i];
        const auto& secondRecord = second[i];

        if( !( firstRecord == secondRecord ) ) return true;
        else if( firstRecord.time() != secondRecord.time() ) return true;
        else if( firstRecord.flags() != secondRecord.flags() ) return true;
        else if( !( firstRecord.properties() == secondRecord.properties() ) ) return true;

    }

    return false;

}
