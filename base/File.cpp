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

#include "File.h"
#include "Debug.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

#include <algorithm>
#include <cmath>

//_____________________________________________________________________
template<> File::File( File& other ): value_( other.value_ ) {}

//_____________________________________________________________________
template<> File::File( const File& other ): value_( other.value_ ) {}

//_____________________________________________________________________
bool File::isAbsolute( const QString& value )
{ return QFileInfo( value ).isAbsolute(); }

//_____________________________________________________________________
TimeStamp File::created() const
{
    if( !exists() ) return TimeStamp();
    return TimeStamp(QFileInfo( *this ).created().toTime_t());
}

//_____________________________________________________________________
TimeStamp File::lastModified() const
{
    if( !exists() ) return TimeStamp();
    return TimeStamp( QFileInfo( *this ).lastModified().toTime_t() );
}

//_____________________________________________________________________
TimeStamp File::lastAccessed() const
{
    if( !exists() ) return TimeStamp();
    return TimeStamp( QFileInfo( *this ).lastRead().toTime_t() );
}

//_____________________________________________________________________
uint File::userId() const
{
    if( !exists() ) return 0;
    return QFileInfo( *this ).ownerId();
}

//_____________________________________________________________________
uint File::groupId() const
{
    if( !exists() ) return 0;
    return QFileInfo( *this ).groupId();
}

//_____________________________________________________________________
QString File::userName() const
{
    if( !exists() ) return QString();
    QString out( QFileInfo( *this ).owner() );
    if( out.isNull() || out.isEmpty() ) out = QString::number( QFileInfo( *this ).ownerId() );
    return out;
}

//_____________________________________________________________________
QString File::groupName() const
{
    if( !exists() ) return QString();
    return QFileInfo( *this ).group();
}

//_____________________________________________________________________
QFile::Permissions File::permissions() const
{ return QFileInfo( *this ).permissions(); }

//_____________________________________________________________________
QString File::permissionsString( const QFile::Permissions& mode ) const
{
    QString out;
    QTextStream what( &out );

    // link, directory or regular file
    if( QFileInfo( *this ).isSymLink() ) what << "l";
    else if( QFileInfo( *this ).isDir() ) what << "d";
    else what << "-";

    // user permissions
    what << ( ( mode & QFile::ReadUser ) ? "r":"-" );
    what << ( ( mode & QFile::WriteUser ) ? "w":"-" );
    what << ( ( mode & QFile::ExeUser ) ? "x":"-" );

    // group permissions
    what << ( ( mode & QFile::ReadGroup  ) ? "r":"-" );
    what << ( ( mode & QFile::WriteGroup ) ? "w":"-" );
    what << ( ( mode & QFile::ExeGroup   ) ? "x":"-" );

    // other permissions
    what << ( ( mode & QFile::ReadOther  ) ? "r":"-" );
    what << ( ( mode & QFile::WriteOther ) ? "w":"-" );
    what << ( ( mode & QFile::ExeOther   ) ? "x":"-" );

    return out;

}

//_____________________________________________________________________
qint64 File::fileSize() const
{
    if( !exists() ) return 0;
    return QFileInfo( *this ).size();
}

//_____________________________________________________________________
QString File::sizeString( qint64 sizeInt )
{
    if( sizeInt < 0 ) return QString();
    if( sizeInt == 0 ) return "0";

    qreal size = sizeInt;
    static const QString unit[] =
    {
        QObject::tr( "B" ),
        QObject::tr( "KiB" ),
        QObject::tr( "MiB" ),
        QObject::tr( "GiB" ),
        QObject::tr( "TiB" )
    };

    int power = log( size )/log( 1024 );
    QString out;

    if( !power ) out.sprintf( "%.0f", size );
    else {

        if( power > 4 ) power = 4;
        size /= pow( static_cast<double>(1024), static_cast<double>(power) );
        out.sprintf( "%.1f", size );

    }

    return QString( "%1 %2").arg( out ).arg( unit[power] );

}

//_____________________________________________________________________
QString File::rawSizeString( qint64 sizeInt )
{
    if( sizeInt < 0 ) return QString();
    if( sizeInt == 0 ) return "0";

    // format size to have space characters every three digits
    QString out = QString::number( sizeInt );
    int length( out.length() );
    for( int i = 1; i < length; i++ )
    { if( !(i%3) ) out.insert( out.size() - (i + i/3 - 1), ' ' ); }

    return out;
}

//_____________________________________________________________________
bool File::exists() const
{ return QFileInfo( *this ).exists(); }

//_____________________________________________________________________
bool File::isWritable() const
{ return value_.isEmpty() || QFile( *this ).open( QIODevice::ReadWrite ); }

//_____________________________________________________________________
bool File::isDirectory() const
{ return !value_.isEmpty() && QFileInfo( *this ).isDir(); }

//_____________________________________________________________________
bool File::isHidden() const
{ return !value_.isEmpty() && QFileInfo( *this ).isHidden(); }

//_____________________________________________________________________
bool File::isLink() const
{ return !value_.isEmpty() && QFileInfo( *this ).isSymLink(); }

//_____________________________________________________________________
bool File::isBrokenLink() const
{ return isLink() && !File( QFileInfo( *this ).symLinkTarget() ).exists(); }

//_____________________________________________________________________
bool File::diff(const File& file ) const
{

    // no file exists
    if( !( exists() || file.exists() ) ) return false;

    // one of the file does not exists and the other does
    if( !( exists() && file.exists() ) ) return true;

    QFile first( *this );
    QFile second( file );
    bool first_open( first.open( QIODevice::ReadOnly ) );
    bool second_open( second.open( QIODevice::ReadOnly ) );

    // no file exists
    if( !( first_open || second_open ) ) return false;

    // one of the file does not exists and the other does
    if( !( first_open && second_open ) ) return true;

    return( first.readAll() != second.readAll() );

}

//_____________________________________________________________________
bool File::isEqual( const File& other ) const
{ return expanded() == other.expanded(); }

//_____________________________________________________________________
File File::path( bool useAbsolute ) const
{

    if( value_.isEmpty() ) return File();
    if( useAbsolute ) return File( QFileInfo(*this).absolutePath() );
    else return File( QFileInfo(*this).path() );

}

//_____________________________________________________________________
File File::localName() const
{ return value_.isEmpty() ? File(): File( QFileInfo(*this).fileName() ); }

//_____________________________________________________________________
File File::canonicalName() const
{
    if( value_.isEmpty() ) return File();
    QString canonicalName( QFileInfo(*this).canonicalFilePath() );
    return canonicalName.isEmpty() ? File():File( canonicalName );
}

//_____________________________________________________________________
File File::extension() const
{
    // check file name
    if( value_.isEmpty() ) return File();

    // loop over characters
    QString local( localName() );
    int dotpos = local.lastIndexOf(".");
    return File( dotpos < 0 ? "" : local.mid( dotpos+1 ) );

}

//_____________________________________________________________________
File File::truncatedName() const
{

    // check file name
    if( value_.isEmpty() ) return File();

    // loop over characters
    int dotpos = value_.lastIndexOf( '.' );
    int slashpos = value_.lastIndexOf( '/' );

    if( dotpos < 0 ) return *this;
    if( slashpos < 0 ) return File( dotpos >= 0 ? value_.left(dotpos):"");
    if( slashpos < dotpos ) return File( value_.left(dotpos) );

    return *this;

}

//_____________________________________________________________________
File File::find( const File& file, bool caseSensitive ) const
{

    if( !( exists() && isDirectory() ) ) return File();

    // check local files
    {
        File copy( file );
        if( copy.addPath( *this ).exists() ) return copy;
    }

    // get subdirectories
    QString fullname( value_ );
    addTrailingSlash( fullname );

    // filter
    QDir::Filters filter = QDir::Dirs;
    #if QT_VERSION >= 0x040800
    filter |= QDir::NoDotDot;
    #endif

    const QDir dir( fullname );
    for( const auto& value:dir.entryList( filter ) )
    {
        if( value == "." || value == ".." ) continue;

        QFileInfo fileInfo;
        fileInfo.setFile( dir, value );
        const File local( fileInfo.absoluteFilePath() );
        File found( local.find( file, caseSensitive ) );
        if( !found.isEmpty() ) return found;

    }

    return File();

}

//_____________________________________________________________________
File File::find( const File::List& pathList ) const
{

    // if absolute, return this if exists
    if( isAbsolute() ) return exists() ? *this:File();

    for( const auto& path:pathList )
    {
        File copy( *this );
        if( copy.addPath( path ).exists() ) return copy;
    }

    return File();

}

//_____________________________________________________________________
File::List File::listFiles( ListFlags flags ) const
{

    List out;
    File fullname( expanded() );
    if( !fullname.isDirectory() || (fullname.isLink() && !(flags&ListFlag::FollowLinks) ) ) return out;
    fullname = fullname.addTrailingSlash();

    // open directory
    QDir::Filters filter = QDir::AllEntries|QDir::System;
    #if QT_VERSION >= 0x040800
    filter |= QDir::NoDotDot;
    #endif

    const QDir dir( fullname );
    if( flags & ListFlag::ShowHiddenFiles ) filter |= QDir::Hidden;
    for( const auto& value:dir.entryList( filter ) )
    {

        if( value == "." || value == ".." ) continue;

        QFileInfo fileInfo;
        fileInfo.setFile( dir, value );
        const File file( fileInfo.absoluteFilePath() );
        out.append( file );

        // list subdirectory if recursive
        if( flags & ListFlag::Recursive && file.isDirectory() )
        {

            // in case directory is a link
            // make sure it is not already in the list
            // to avoid recursivity
            if( file.isLink() && std::any_of( out.begin(), out.end(), SameLinkFTor( file.readLink() ) ) ) continue;

            // list subdirectory
            out.append( file.listFiles( flags ) );
        }

    }

    return out;

}

//_____________________________________________________________________
bool File::create() const
{ return QFile( *this ).open( QIODevice::WriteOnly ); }

//_____________________________________________________________________
bool File::createDirectory( const File& constPath ) const
{
    if( exists() && !isDirectory() ) return false;
    if( !QDir( *this ).mkpath( constPath ) ) return false;
    return true;
}

//_____________________________________________________________________
void File::setHidden() const
{
    #if defined(Q_OS_WIN)
    if( !value_.isEmpty() ) SetFileAttributes( value_.toLatin1(), FILE_ATTRIBUTE_HIDDEN );
    #endif
}

//_____________________________________________________________________
File File::version() const
{

    int version=0;
    File out( this->expanded() );
    QString expanded( out.get() );
    while( out.exists() )
    {
        out.value_.clear();
        QTextStream(&out.value_) << expanded << "_" << version;
        version++;
    }

    return out;
}

//_____________________________________________________________________
File File::backup() const
{

    // check filename is valid and file exists
    if( !exists() ) return File();

    QString expanded( this->expanded() );
    QString backup( expanded+"~" );

    // open this file
    QFile in( expanded );
    in.copy( backup );
    return File( backup );

}

//_____________________________________________________________________
File File::readLink() const
{ return File( QFileInfo( *this ).canonicalFilePath() ); }

//_____________________________________________________________________
bool File::remove() const
{

    // check if file exists and remove
    // if it does not exists, do nothing and returns true (file was removed already)
    if( isLink() || !isDirectory() )
    {

        if( isBrokenLink() || exists() ) return QFile( *this ).remove();
        else return true;

    } else {

        QDir dir( *this );
        dir.cdUp();
        return dir.rmdir( *this );

    }

}

//_____________________________________________________________________
bool File::removeRecursive() const
{

    if( isLink() || !isDirectory() ) return remove();

    // list content of directory
    QDir dir( *this );
    for( const auto& value:dir.entryList( QDir::AllEntries|QDir::Hidden|QDir::System ) )
    {
        // skip "." and ".."
        if( value == "." || value == ".." ) continue;
        File file = File( value ).addPath( *this );
        if( file.isLink() || !file.isDirectory() )
        {

            if( !file.remove() ) return false;

        } else if( !file.removeRecursive() ) return false;

    }
    dir.cdUp();
    dir.rmdir( *this );
    return true;
}

//_____________________________________________________________________
bool File::rename( const File& newFile ) const
{

    // check if file exists and rename
    // return false  otherwise
    if( exists() ) return QFile( *this ).rename( newFile );
    else return false;

}

//_____________________________________________________________________
bool File::copy( const File& newFile, bool force ) const
{
    // check existence
    if( !exists() ) return false;

    // check destination existance
    if( newFile.exists() && !( force && newFile.removeRecursive() ) ) return false;

    // check file type
    if( isLink() )
    {

        // get source and make a new link
        return QFile( QFile( *this ).symLinkTarget() ).link( newFile );

    } else if( isDirectory() ) {

        // create directory
        if( !newFile.createDirectory() ) return false;

        // list files, and copy, recursively
        for( const auto& file:listFiles( ListFlag::ShowHiddenFiles ) )
        { if( !file.copy( file.localName().addPath( newFile ) ) ) return false; }

        return true;

    } else {

        // copy plain file
        return QFile( *this ).copy( newFile );

    }

}

//____________________________________________
void File::addPath( QString& value, const QString& path, bool useAbsolute )
{

    // returns 0 if either path nor file are given
    if( value.isEmpty() && path.isEmpty() ) return;
    else if( value.isEmpty() ) value = path;
    else if( isAbsolute( value ) ) return;
    else if( path.isEmpty() ) value.prepend("./");
    else {

        QFileInfo info;
        info.setFile( QDir( path ), value );
        value = useAbsolute ? info.absoluteFilePath():info.filePath();
    }

    return;

}

//_____________________________________________________________________
void File::expand( QString& value )
{
    if( value.isEmpty() ) return;
    value = QFileInfo(value).absoluteFilePath();
    removeTrailingSlash( value );
    return;
}

//_____________________________________________________________________
void File::addTrailingSlash( QString& value )
{ if( !value.endsWith( '/' ) ) value += '/'; }

//_____________________________________________________________________
void File::removeTrailingSlash( QString& value )
{ if( value.size() > 1 && value.endsWith( '/' ) ) value.resize( value.size()-1 ); }
