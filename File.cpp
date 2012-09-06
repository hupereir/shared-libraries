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
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "File.h"
#include "Debug.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include <algorithm>
#include <cmath>

//_____________________________________________________________________
bool File::isAbsolute( void ) const
{ return QFileInfo( *this ).isAbsolute(); }

//_____________________________________________________________________
bool File::create( void ) const
{ return QFile( *this ).open( QIODevice::WriteOnly ); }

//_____________________________________________________________________
time_t File::created( void ) const
{
    if( !exists() ) return -1;
    return QFileInfo( *this ).created().toTime_t();
}

//_____________________________________________________________________
time_t File::lastModified( void ) const
{
    if( !exists() ) return -1;
    return QFileInfo( *this ).lastModified().toTime_t();
}

//_____________________________________________________________________
time_t File::lastAccessed( void ) const
{
    if( !exists() ) return -1;
    return QFileInfo( *this ).lastRead().toTime_t();
}


//_____________________________________________________________________
unsigned int File::userId( void ) const
{
    if( !exists() ) return 0;
    return QFileInfo( *this ).ownerId();
}

//_____________________________________________________________________
QString File::userName( void ) const
{
    if( !exists() ) return QString();
    QString out( QFileInfo( *this ).owner() );
    if( out.isNull() || out.isEmpty() ) out = QString().setNum( QFileInfo( *this ).ownerId() );
    return out;
}

//_____________________________________________________________________
unsigned int File::groupId( void ) const
{
    if( !exists() ) return 0;
    return QFileInfo( *this ).groupId();
}

//_____________________________________________________________________
QString File::groupName( void ) const
{
    if( !exists() ) return QString();
    return QFileInfo( *this ).group();
}

//_____________________________________________________________________
QFile::Permissions File::permissions( void ) const
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
unsigned int File::fileSize( void ) const
{
    if( !exists() ) return 0;
    return QFileInfo( *this ).size();
}

//_____________________________________________________________________
QString File::sizeString( const unsigned int& sizeInt )
{
    double size = (double) sizeInt;
    if( size < 0 ) return "-";

    static const QString unit[] = {
        "B",
        "KiB",
        "MiB",
        "GiB"
    };

    if( size == 0 ) return "0";

    int power = static_cast<int>( log( size )/log( static_cast<double>(1024) ) );
    QString out;

    if( !power ) out.sprintf( "%7.0f", size );
    else {

        if( power > 3 ) power = 3;
        size /= pow( static_cast<double>(1024), static_cast<double>(power) );
        out.sprintf( "%7.1f", size );

    }

    return out.trimmed() + " " + unit[power];

}

//_____________________________________________________________________
bool File::exists( void ) const
{ return QFileInfo( *this ).exists(); }

//_____________________________________________________________________
bool File::isWritable( void ) const
{ return isEmpty() || QFile( *this ).open( QIODevice::ReadWrite ); }

//_____________________________________________________________________
bool File::isDirectory( void ) const
{ return !isEmpty() && QFileInfo( *this ).isDir(); }

//_____________________________________________________________________
bool File::isBrokenLink( void ) const
{ return isLink() && !File( QFileInfo( *this ).symLinkTarget() ).exists(); }

//_____________________________________________________________________
bool File::isLink( void ) const
{ return !isEmpty() && QFileInfo( *this ).isSymLink(); }

//_____________________________________________________________________
bool File::isHidden( void ) const
{ return !isEmpty() && QFileInfo( *this ).isHidden(); }

//_____________________________________________________________________
File File::version( void ) const
{

    int version=0;
    QString expand( File::expand() );
    File out( expand );
    while( out.exists() )
    {
        out.clear();
        QTextStream(&out) << expand << "_" << version;
        version++;
    }

    return out;
}

//_____________________________________________________________________
File File::backup( void ) const
{

    // check filename is valid and file exists
    if( !exists() ) return File();

    QString expand( File::expand() );
    QString backup( expand+"~" );

    // open this file
    QFile in( expand );
    if( !in.open( QIODevice::ReadOnly ) ) return File();

    // open backup
    QFile out( backup );
    if( !out.open( QIODevice::WriteOnly ) ) return File();

    out.write( in.readAll() );
    out.close();
    in.close();
    return backup;

}

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
bool File::isEqual( const File& file ) const
{
    return expand() == file.expand();
}

//_____________________________________________________________________
File File::readLink( void ) const
{ return File( QFileInfo( *this ).canonicalFilePath() ); }

//_____________________________________________________________________
bool File::remove( void ) const
{

    Debug::Throw() << "File::remove - " << *this << endl;

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
bool File::removeRecursive( void ) const
{

    if( isLink() || !isDirectory() ) return remove();

    // list content of directory
    QDir dir( *this );
    foreach( const QString& value, dir.entryList( QDir::AllEntries|QDir::Hidden|QDir::System ) )
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
bool File::rename( File newFile ) const
{

    // check if file exists and rename
    // return false  otherwise
    if( exists() ) return QFile( *this ).rename( newFile );
    else return false;

}

//____________________________________________
File File::addPath( const QString& path, bool use_absolute ) const
{

    // returns 0 if either path nor file are given
    if( isEmpty() && path.isEmpty() ) return File();

    // return path if path is given but not the file
    if( isEmpty() ) return path;

    // if file is absolute, keep unchanged
    if( isAbsolute() ) return *this;

    // returns file if it is relative but path is not given
    if( !path.size() ) return QString("./")+(*this);

    QFileInfo info;
    info.setFile( QDir( path ), *this );
    return use_absolute ? info.absoluteFilePath(): info.filePath();

}

//_____________________________________________________________________
File File::expand( void ) const
{
    if( isEmpty() ) return File();
    File out( QFileInfo(*this).absoluteFilePath() );

    // remove trailing slash, except for root
    if( out.size() > 1 && out[out.size()-1] == '/' ) out.resize( out.size()-1 );

    return out;

}

//_____________________________________________________________________
File File::path( bool use_absolute ) const
{

    if( isEmpty() ) return File();
    if( use_absolute ) return QFileInfo(*this).absolutePath();
    else return QFileInfo(*this).path();

}

//_____________________________________________________________________
File File::localName( void ) const
{ return isEmpty() ? File(): File( QFileInfo(*this).fileName() ); }

//_____________________________________________________________________
File File::canonicalName( void ) const
{
    if( isEmpty() ) return File();
    QString canonicalName( QFileInfo(*this).canonicalFilePath() );
    return canonicalName.isEmpty() ? File():File( canonicalName );
}

//_____________________________________________________________________
File File::extension( void ) const
{
    // check file name
    if( isEmpty() ) return File();

    // loop over characters
    QString local( localName() );
    int dotpos = local.lastIndexOf(".");
    return File( dotpos < 0 ? "" : local.mid( dotpos+1 ) );

}

//_____________________________________________________________________
File File::truncatedName( void ) const
{

    // check file name
    if( isEmpty() ) return File();

    // loop over characters
    int dotpos = lastIndexOf(".");
    int slashpos = lastIndexOf( "/" );

    if( dotpos < 0 ) return *this;
    if( slashpos < 0 ) return File( dotpos >= 0 ? left(dotpos):"");
    if( slashpos < dotpos ) return left(dotpos);

    return *this;

}

//_____________________________________________________________________
File::List File::listFiles( const unsigned int& flags ) const
{

    Debug::Throw() << "File::listFiles - this: " << *this << " - hidden: " << (flags&SHOW_HIDDEN) << endl;

    List out;
    File fullName( expand() );
    if( !fullName.isDirectory() || (fullName.isLink() && !flags&FOLLOW_LINKS ) ) return out;
    if( !fullName.endsWith( "/" ) ) fullName += "/";

    // open directory
    QDir::Filters filter = QDir::AllEntries|QDir::System|QDir::NoDotDot;
    if( flags & SHOW_HIDDEN ) filter |= QDir::Hidden;

    foreach( const QString& value, QDir( fullName ).entryList( filter ) )
    {

        if( value == "." || value == ".." ) continue;

        QFileInfo fileInfo;
        fileInfo.setFile( QDir( *this ), value );
        const File file( fileInfo.absoluteFilePath() );
        out << file;

        // list subdirectory if recursive
        if( flags & RECURSIVE && file.isDirectory() )
        {

            // in case directory is a link
            // make sure it is not already in the list
            // to avoid recursivity
            if( file.isLink() && std::find_if( out.begin(), out.end(), SameLinkFTor( file ) ) != out.end() ) continue;

            // list subdirectory
            out << file.listFiles( flags );
        }

    }

    return out;

}

//_____________________________________________________________________
File File::find( const File& file, bool case_sensitive ) const
{

    Debug::Throw() << "File::find - this: " << *this << endl;
    if( !( exists() && isDirectory() ) ) return File();
    List files( listFiles( RECURSIVE ) );
    List directories;
    foreach( const File& file, files )
    {

        // check if file match
        if( file.localName().compare( file, case_sensitive ? Qt::CaseSensitive : Qt::CaseInsensitive ) == 0 )
        { return file; }

        // check if file is directory
        if( file.isDirectory() ) directories << file;
    }

    // loop over directories; search recursively
    foreach( const File& file, directories )
    {
        File found( file.find( file, case_sensitive ) );
        if( found != File() ) return found;
    }

    return File();

}
