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


#if !defined(Q_OS_WIN)
#include <sys/stat.h>
#else
/* on widows need to define permissions flags manually */
/* Read, write, execute/search by owner */
#define S_IRUSR 0000400 /* R for owner */
#define S_IWUSR 0000200 /* W for owner */
#define S_IXUSR 0000100 /* X for owner */
/* Read, write, execute/search by group */
#define S_IRGRP 0000040 /* R for group */
#define S_IWGRP 0000020 /* W for group */
#define S_IXGRP 0000010 /* X for group */
/* Read, write, execute/search by others */
#define S_IROTH 0000004 /* R for other */
#define S_IWOTH 0000002 /* W for other */
#define S_IXOTH 0000001 /* X for other */
#endif

using PermissionPair = std::pair<mode_t,QFile::Permission>;
static const std::initializer_list<PermissionPair> permissionMap =
{
    {S_IRUSR, QFile::ReadOwner },
    {S_IWUSR, QFile::WriteOwner },
    {S_IXUSR, QFile::ExeOwner },

    {S_IRUSR, QFile::ReadUser },
    {S_IWUSR, QFile::WriteUser },
    {S_IXUSR, QFile::ExeUser },

    {S_IRGRP, QFile::ReadGroup },
    {S_IWGRP, QFile::WriteGroup },
    {S_IXGRP, QFile::ExeGroup },

    {S_IROTH, QFile::ReadOther },
    {S_IWOTH, QFile::WriteOther },
    {S_IXOTH, QFile::ExeOther  }
};

namespace Xml
{
    // this one is kept for backward compatibility
    static const QString Name = QStringLiteral("name");
};

//________________________________________________________________
const QString BaseFileInfo::MimeType( QStringLiteral("internal/file-info-list") );

//________________________________________________________________
BaseFileInfo::BaseFileInfo( const QDomElement& element ):
    type_( None ),
    size_(0),
    lastModified_(-1)
{

    Debug::Throw( QStringLiteral("BaseFileInfo::BaseFileInfo (dom).\n") );

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

    Debug::Throw( QStringLiteral("BaseFileInfo::DomElement.\n") );
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
mode_t BaseFileInfo::unixPermissions( QFile::Permissions value )
{
    return std::accumulate( permissionMap.begin(), permissionMap.end(), 0,
        [value]( int permissions, PermissionPair pair )
        { return (value&pair.second) ? std::move( permissions )|pair.first : std::move( permissions ); } );
}


//________________________________________________________________
QFile::Permissions BaseFileInfo::permissions( mode_t value )
{
    return std::accumulate(
        permissionMap.begin(), permissionMap.end(), (QFile::Permissions) 0,
        [value]( QFile::Permissions permissions, PermissionPair pair )
        { return (value&pair.first) ? std::move( permissions )|pair.second : std::move( permissions ); } );
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
