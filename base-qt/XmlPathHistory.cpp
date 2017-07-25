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

#include "XmlPathHistory.h"

#include "CppUtil.h"
#include "Debug.h"
#include "Singleton.h"
#include "XmlDocument.h"
#include "XmlFileRecord.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QFile>

//_______________________________________________
XmlPathHistory::XmlPathHistory( QObject* parent ):
    PathHistory( parent )
{

    Debug::Throw( "XmlPathHistory::XmlPathHistory.\n" );
    connect( Base::Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );
    connect( qApp, SIGNAL(aboutToQuit()), SLOT(_saveConfiguration()) );
    _updateConfiguration();

}

//_______________________________________________
void XmlPathHistory::setTagName( const QString& value )
{

    // do nothing if tagName is unchanged
    if( value == tagName_ ) return;

    // update tag name and read
    tagName_ = value;
    clear();
    read();

}

//_______________________________________________
bool XmlPathHistory::read( File file )
{
    Debug::Throw( "XmlPathHistory::read.\n" );

    // do nothing for invalid tagname
    if( tagName_.isEmpty() )
    {
        Debug::Throw() << "XmlPathHistory::read - tag name is empty" << endl;
        return false;
    }

    // do nothing if saveHistory is false
    if( !saveHistory_ ) return false;

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

        return true;

    } else return false;

}

//_______________________________________________
bool XmlPathHistory::_read( const XmlDocument& document )
{

    // look for relevant element
    QDomNodeList topNodes = document.elementsByTagName( tagName_ );
    if( topNodes.isEmpty() ) return false;

    FileRecord::List pathList;
    for(QDomNode node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == Base::Xml::Record )
        {

            XmlFileRecord record( element );
            if( !record.file().isEmpty() ) pathList.append( record );

        }
    }

    // assign
    setPathList( pathList );

    return true;

}

//_______________________________________________
bool XmlPathHistory::write( File file )
{
    Debug::Throw( "XmlPathHistory::write.\n" );

    // do nothing for invalid tag name
    if( tagName_.isEmpty() )
    {
        Debug::Throw() << "XmlPathHistory::write - tag name is empty" << endl;
        return false;
    }

    // check file
    if( file.isEmpty() ) file = dbFile_;
    if( file.isEmpty() ) return false;

    // create document and read
    XmlDocument document;
    {
        QFile qtfile( file );
        document.setContent( &qtfile );
    }

    // get path list
    FileRecord::List pathList;
    if( saveHistory_ ) pathList = _truncatedList();

    // read old list and compare to current
    XmlPathHistory oldList;
    oldList.tagName_ = tagName_;
    if( oldList._read( document ) && !_differs( oldList.pathList(), pathList ) )
    { return true; }

    // create main element and insert
    QDomElement top = document.createElement( tagName_ );
    for( const auto& path:pathList )
    {

        if( !path.file().isEmpty() )
        { top.appendChild( XmlFileRecord( path ).domElement( document.get() ) ); }

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
bool XmlPathHistory::_setDBFile( const File& file )
{
    Debug::Throw() << "XmlPathHistory::_setDBFile - file: " << file << endl;

    // check file
    if( dbFile_ == file && !pathList().isEmpty() ) return false;

    // store file and read
    dbFile_ = file;

    // make sure file is hidden (windows only)
    if( dbFile_.localName().startsWith( '.' ) )
    { dbFile_.setHidden(); }

    read();

    return true;

}

//______________________________________
void XmlPathHistory::_updateConfiguration()
{
    Debug::Throw( "XmlPathHistory::_updateConfiguration.\n" );

    // max size
    if( XmlOptions::get().contains( "HISTORY_SIZE" ) )
    { _setMaxSize( XmlOptions::get().get<int>( "HISTORY_SIZE" ) ); }

    // save history
    if( XmlOptions::get().contains( "SAVE_HISTORY" ) )
    { saveHistory_ = XmlOptions::get().get<bool>( "SAVE_HISTORY" ); }

    // DB file
    _setDBFile( File( XmlOptions::get().raw( "RC_FILE" ) ) );

    return;

}

//______________________________________
void XmlPathHistory::_saveConfiguration()
{
    Debug::Throw( "XmlPathHistory::_saveConfiguration.\n" );
    write();
}

//______________________________________
bool XmlPathHistory::_differs( const FileRecord::List& first, const FileRecord::List& second ) const
{

    // check sizes
    if( first.size() != second.size() ) return true;

    // check records one by one
    for( int i = 0; i < first.size(); ++i )
    {

        // records
        const FileRecord& firstRecord = first[i];
        const FileRecord& secondRecord = second[i];

        if( !( firstRecord == secondRecord ) ) return true;
        else if( firstRecord.time() != secondRecord.time() ) return true;
        else if( firstRecord.flags() != secondRecord.flags() ) return true;
        else if( !( firstRecord.properties() == secondRecord.properties() ) ) return true;

    }

    return false;

}
