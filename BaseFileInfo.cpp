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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "BaseFileInfo.h"
#include "Debug.h"
#include "TimeStamp.h"
#include "XmlString.h"

#include <cassert>

//! Some Xml definitions
namespace XML
{

    static const QString FILEINFO_LIST( "FileInfoList" );
    static const QString FILEINFO( "FileInfo" );
    static const QString FILE( "File" );
    static const QString TYPE( "Type" );
    static const QString SIZE( "Size" );
    static const QString LAST_MODIFIED( "LastModified" );
    static const QString USER( "user" );
    static const QString GROUP( "group" );
    static const QString PERMISSIONS( "permissions" );

}

//________________________________________________________________
BaseFileInfo::BaseFileInfo( const QDomElement& element ):
    type_( None ),
    size_(0),
    lastModified_(-1)
{

    Debug::Throw( "BaseFileInfo::BaseFileInfo (dom).\n" );

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( unsigned int i=0; i<attributes.length(); i++ )
    {

        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == XML::FILE ) setFile( File( XmlString( attribute.value() ).toText() ) );
        else if( attribute.name() == XML::TYPE ) setType( TypeFlags( attribute.value().toInt() ) );
        else if( attribute.name() == XML::SIZE ) setSize( attribute.value().toInt() );
        else if( attribute.name() == XML::LAST_MODIFIED ) setLastModified( attribute.value().toInt() );
        else if( attribute.name() == XML::USER ) setUser( attribute.value() );
        else if( attribute.name() == XML::GROUP ) setGroup( attribute.value() );
        else if( attribute.name() == XML::PERMISSIONS ) setPermissions( (QFile::Permissions) attribute.value().toUInt() );

    }

}

//________________________________________________________________
QDomElement BaseFileInfo::domElement( QDomDocument& document ) const
{

    Debug::Throw( "BaseFileInfo::DomElement.\n" );
    QDomElement out( document.createElement( XML::FILEINFO ) );
    out.setAttribute( XML::FILE, XmlString( file() ).toXml() );
    out.setAttribute( XML::TYPE, QString().setNum( type() ) );
    out.setAttribute( XML::SIZE, QString().setNum( size() ) );
    out.setAttribute( XML::LAST_MODIFIED, QString().setNum( lastModified() ) );
    out.setAttribute( XML::USER, user() );
    out.setAttribute( XML::GROUP, group() );
    out.setAttribute( XML::PERMISSIONS, QString().setNum( permissions() ) );

    return out;

}

//________________________________________________________________
QString BaseFileInfo::typeString( void ) const
{
    if( isNavigator() ) return "Navigator";
    else if( isLink() ) return "Symbolic Link";
    else if( isFolder() ) return "Folder";
    else if( isDocument() ) return "Document";
    else return "Item";
}

//________________________________________________________________
QString BaseFileInfo::permissionsString( void ) const
{
    QString permissionsString( File().permissionsString( permissions() ) );
    if( isLink() ) permissionsString[0] = 'l';
    else if( isFolder() ) permissionsString[0] = 'd';
    return permissionsString;
}

//________________________________________________________________
void BaseFileInfo::setPermissions( const QString& value )
{
    assert( value.size() == 10 );

    // first bit
    if( value[0] == 'l' ) setIsLink();
    else if( value[0] == 'd' ) setIsFolder();
    else setIsDocument();

    // reset
    permissions_  = QFile::Permissions();
    if( value[1] == 'r' ) permissions_ |= QFile::ReadUser;
    if( value[2] == 'w' ) permissions_ |= QFile::WriteUser;
    if( value[3] == 'x' ) permissions_ |= QFile::ExeUser;

    if( value[4] == 'r' ) permissions_ |= QFile::ReadGroup;
    if( value[5] == 'w' ) permissions_ |= QFile::WriteGroup;
    if( value[6] == 'x' ) permissions_ |= QFile::ExeGroup;

    if( value[7] == 'r' ) permissions_ |= QFile::ReadOther;
    if( value[8] == 'w' ) permissions_ |= QFile::WriteOther;
    if( value[9] == 'x' ) permissions_ |= QFile::ExeOther;

}

//________________________________________________________________
void BaseFileInfo::update( void )
{

    if( file().isEmpty() )
    {

        setUser( QString() );
        setGroup( QString() );
        setSize( 0 );
        setLastModified( TimeStamp() );
        setPermissions( QFile::Permissions() );

    } else {

        setSize( file().fileSize() );
        setUser( file().userName() );
        setGroup( file().groupName() );
        setLastModified( file().lastModified() );
        setPermissions( file().permissions() );

    }

}

//________________________________________________________________
void BaseFileInfo::updateFrom( const BaseFileInfo& info )
{

    setUser( info.user() );
    setGroup( info.group() );
    setSize( info.size() );
    setLastModified( info.lastModified() );
    setPermissions( info.permissions() );

}
