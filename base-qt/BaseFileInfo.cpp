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

#include "BaseFileInfo.h"
#include "Debug.h"
#include "TimeStamp.h"

namespace Xml
{
    // this one is kept for backward compatibility
    static const QString Name = "name";
};

//________________________________________________________________
const QString BaseFileInfo::MimeType( "internal/file-info-list" );

//________________________________________________________________
BaseFileInfo::BaseFileInfo( const QDomElement& element ):
    type_( None ),
    size_(0),
    lastModified_(-1)
{

    Debug::Throw( "BaseFileInfo::BaseFileInfo (dom).\n" );

    // parse attributes
    const auto attributes( element.attributes() );
    for( int i=0; i<attributes.count(); i++ )
    {

        const auto attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == Xml::File ) setFile( File( attribute.value() ) );
        else if( attribute.name() == Xml::Alias || attribute.name() == Xml::Name ) setAlias( attribute.value() );
        else if( attribute.name() == Xml::Type ) setType( TypeFlags( attribute.value().toInt() ) );
        else if( attribute.name() == Xml::Size ) setSize( attribute.value().toLongLong() );
        else if( attribute.name() == Xml::LastModified ) setLastModified( TimeStamp( attribute.value().toInt() ) );
        else if( attribute.name() == Xml::User ) setUser( attribute.value() );
        else if( attribute.name() == Xml::Group ) setGroup( attribute.value() );
        else if( attribute.name() == Xml::Permissions ) setPermissions( (QFile::Permissions) attribute.value().toUInt() );

    }

}

//________________________________________________________________
QDomElement BaseFileInfo::domElement( QDomDocument& document ) const
{

    Debug::Throw( "BaseFileInfo::DomElement.\n" );
    auto out( document.createElement( Xml::FileInfo ) );
    out.setAttribute( Xml::File, file() );
    out.setAttribute( Xml::Type, QString::number( type() ) );
    out.setAttribute( Xml::Size, QString::number( size() ) );
    out.setAttribute( Xml::Permissions, QString::number( permissions() ) );
    out.setAttribute( Xml::LastModified, QString::number( lastModified().unixTime() ) );

    if( !alias_.isEmpty() ) out.setAttribute( Xml::Alias, alias() );
    if( !user_.isEmpty() ) out.setAttribute( Xml::User, user_ );
    if( !group_.isEmpty() ) out.setAttribute( Xml::Group, group_ );

    return out;

}

//________________________________________________________________
QString BaseFileInfo::typeString() const
{
    if( isNavigator() ) return QObject::tr( "Navigator" );
    else if( isLink() ) return QObject::tr( "Symbolic Link" );
    else if( isFolder() ) return QObject::tr( "Folder" );
    else if( isDocument() ) return QObject::tr( "Document" );
    else return QObject::tr( "Item" );
}

//________________________________________________________________
QString BaseFileInfo::permissionsString() const
{
    QString permissionsString( File().permissionsString( permissions() ) );
    if( isLink() ) permissionsString[0] = 'l';
    else if( isFolder() ) permissionsString[0] = 'd';
    return permissionsString;
}

//________________________________________________________________
void BaseFileInfo::setPermissions( const QString& value )
{

    // first bit
    if( value[0] == 'l' ) setIsLink();
    else if( value[0] == 'd' ) setIsFolder();
    else setIsDocument();

    // reset
    permissions_  = QFile::Permissions();
    if( value[1] == 'r' ) permissions_ |= QFile::ReadOwner;
    if( value[2] == 'w' ) permissions_ |= QFile::WriteOwner;
    if( value[3] == 'x' ) permissions_ |= QFile::ExeOwner;

    if( value[4] == 'r' ) permissions_ |= QFile::ReadGroup;
    if( value[5] == 'w' ) permissions_ |= QFile::WriteGroup;
    if( value[6] == 'x' ) permissions_ |= QFile::ExeGroup;

    if( value[7] == 'r' ) permissions_ |= QFile::ReadOther;
    if( value[8] == 'w' ) permissions_ |= QFile::WriteOther;
    if( value[9] == 'x' ) permissions_ |= QFile::ExeOther;

}

//________________________________________________________________
void BaseFileInfo::update()
{

    if( file().isEmpty() )
    {

        setSize( 0 );
        setUser( QString() );
        setGroup( QString() );
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
