#ifndef File_h
#define File_h

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

#include <QFileInfo>
#include <QList>
#include <QString>

//* file manipulation utility
class File: public QString
{

    public:

    //* shortcut to list of files
    using List=QList<File>;

    //* return list of files in a directory
    enum class ListFlag
    {
        None = 0,
        Recursive = 1<<0,
        FollowLinks = 1<<1,
        ShowHiddenFiles = 1<<2
    };

    Q_DECLARE_FLAGS(ListFlags, ListFlag)

    //* constructor
    File( void )
    {}

    //* constructor
    File( const QString& value ):
        QString( value )
    {}

    //*@name accessors
    //@{

    //* returns true if file has absolute pathname
    bool isAbsolute( void ) const;

    //* time of file creation
    virtual time_t created( void ) const;

    //* time of file last modification
    virtual time_t lastModified( void ) const;

    //* time of file last access
    virtual time_t lastAccessed( void ) const;

    //* user id
    virtual uint userId( void ) const;

    //* group id
    virtual uint groupId( void ) const;

    //* user name
    virtual QString userName( void ) const;

    //* group name
    virtual QString groupName( void ) const;

    //* permissions
    virtual QFile::Permissions permissions( void ) const;

    //* permission string
    virtual QString permissionsString( void ) const
    { return permissionsString( permissions() ); }

    //* permission string
    QString permissionsString( const QFile::Permissions& ) const;

    //* file size
    virtual qint64 fileSize( void ) const;

    //* file size (string version)
    virtual QString sizeString( void ) const
    { return sizeString( fileSize() ); }

    //* file size (string version)
    static QString sizeString( qint64 );

    //* raw file size
    static QString rawSizeString( qint64 );

    //* tells if a file exists
    virtual bool exists( void ) const;

    //* tells if a file exists and can be written into
    virtual bool isWritable( void ) const;

    //* tells if a file exists and can be written into
    virtual bool isReadOnly( void ) const
    { return !isWritable(); }

    //* tells if a file exists and is a directory
    virtual bool isDirectory( void ) const;

    //* tells if a file is hidden
    virtual bool isHidden( void ) const;

    //* tells if a file is a symbolic link
    virtual bool isLink( void ) const;

    //* tells if a file is a broken symbolic link
    virtual bool isBrokenLink( void ) const;

    //* returns true if two file differs
    virtual bool diff( const File& ) const;

    //* returns true if file is the same as argument
    /*
    this is a literal string comparison for files.
    for pathnames, the trailing backslash is ignored
    */
    virtual bool isEqual( const File& ) const;

    //* get path associated to full name
    virtual File path( bool absolute = true ) const;

    //* remove path from full name
    virtual File localName( void ) const;

    //* return canonical name (follow links and remove ".." and ".")
    virtual File canonicalName( void ) const;

    //* get file extension
    virtual File extension( void ) const;

    //* get truncated file (no extension)
    virtual File truncatedName( void ) const;

    //* return first file with matching short name, or empty string if not found
    /** search recursively in this directory and subdirectories */
    virtual File find( const File& file, bool = true ) const;

    //* find file matching this (assuming short name) in list of directries or empty if not found
    virtual File find( const List& ) const;

    //* return list of files in a directory
    virtual List listFiles( ListFlags flags ) const;


    //@}

    //*@name modifiers
    //@{

    //* try create
    bool create( void ) const;

    //* try create directory
    bool createDirectory( const File& = File(".") ) const;

    //* set file as hidden (WIN32 only)
    virtual void setHidden( void ) const;

    //* returns a versioned filename
    /** (add _i at the end with i the first integer for which file is not found) */
    virtual File version( void ) const;

    //* make a backup copy (terminated by ~) of a file, returns backup file
    virtual File backup( void ) const;

    //* return link destination file
    virtual File readLink( void ) const;

    //* removes file from disk
    /**
    returns true if the file does not exists
    or was successfully removed
    */
    virtual bool remove( void ) const;

    //* removes directory from disk, recursively
    virtual bool removeRecursive( void ) const;

    //* rename file
    /** returns true if the file exists and was renamed */
    virtual bool rename( const File& ) const;

    //* copy
    virtual bool copy( const File&, bool = false ) const;

    //* adds path to a file
    /** note: the file is taken raw. No truncation/expension performed.*/
    virtual File addPath( const File&, bool absolute = false ) const;

    //* expand a file name replacing .. or ~ to full path
    virtual File expand( void ) const;

    //@}

    //* used to find files pointing to the same link
    class SameLinkFTor
    {

        public:

        //* constructor
        SameLinkFTor( const File& file ):
            file_( file.readLink() )
        {}

        //* predicate
        bool operator() ( const File& file ) const
        { return file.readLink() == file_; }

        private:

        //* predicted file
        QString file_;

    };

};

Q_DECLARE_OPERATORS_FOR_FLAGS( File::ListFlags )

#endif
