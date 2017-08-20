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

#include "Functors.h"
#include "TimeStamp.h"

#include <QFileInfo>
#include <QList>
#include <QString>
#include <QTextStream>

//* file manipulation utility
class File
{

    public:

    //* shortcut to list of files
    using List=QList<File>;

    //* return list of files in a directory
    enum ListFlag
    {
        None = 0,
        Recursive = 1<<0,
        FollowLinks = 1<<1,
        ShowHiddenFiles = 1<<2
    };

    Q_DECLARE_FLAGS(ListFlags, ListFlag)

    //* universal constructor
    template<typename... Args>
    explicit File(Args&&... args):
        value_( std::forward<Args>(args)... )
    {}

    //* destructor
    virtual ~File() = default;

    //*@name accessors
    //@{

    //* convert to string
    operator const QString& () const { return value_; }

    //* accessor
    const QString& get() const { return value_; }

    template<class T>
        bool startsWith( const T& t ) const
    { return value_.startsWith( t ); }

    template<class T>
        bool endsWith( const T& t ) const
    { return value_.endsWith( t ); }

    template<typename... Args>
        bool contains( Args&&... args ) const
    { return value_.contains( args... ); }

    //* true if empty
    bool isEmpty() const { return value_.isEmpty(); }

    //* returns true if file has absolute pathname
    bool isAbsolute() const
    { return isAbsolute( value_ ); }

    //* time of file creation
    TimeStamp created() const;

    //* time of file last modification
    TimeStamp lastModified() const;

    //* time of file last access
    TimeStamp lastAccessed() const;

    //* user id
    uint userId() const;

    //* group id
    uint groupId() const;

    //* user name
    QString userName() const;

    //* group name
    QString groupName() const;

    //* permissions
    QFile::Permissions permissions() const;

    //* permission string
    QString permissionsString() const
    { return permissionsString( permissions() ); }

    //* permission string
    QString permissionsString( const QFile::Permissions& ) const;

    //* file size
    qint64 fileSize() const;

    //* file size (string version)
    QString sizeString() const
    { return sizeString( fileSize() ); }

    //* file size (string version)
    static QString sizeString( qint64 );

    //* raw file size
    static QString rawSizeString( qint64 );

    //* tells if a file exists
    bool exists() const;

    //* tells if a file exists and can be written into
    bool isWritable() const;

    //* tells if a file exists and can be written into
    bool isReadOnly() const
    { return !isWritable(); }

    //* tells if a file exists and is a directory
    bool isDirectory() const;

    //* tells if a file is hidden
    bool isHidden() const;

    //* tells if a file is a symbolic link
    bool isLink() const;

    //* tells if a file is a broken symbolic link
    bool isBrokenLink() const;

    //* returns true if two file differs
    bool diff( const File& ) const;

    //* returns true if file is the same as argument
    /*
    this is a literal string comparison for files.
    for pathnames, the trailing backslash is ignored
    */
    bool isEqual( const File& ) const;

    //* get path associated to full name
    File path( bool absolute = true ) const;

    //* remove path from full name
    File localName() const;

    //* return canonical name (follow links and remove ".." and ".")
    File canonicalName() const;

    //* get file extension
    File extension() const;

    //* get truncated file (no extension)
    File truncatedName() const;

    //* return first file with matching short name, or empty string if not found
    /** search recursively in this directory and subdirectories */
    File find( const File& file, bool = true ) const;

    //* find file matching this (assuming short name) in list of directries or empty if not found
    File find( const List& ) const;

    //* return list of files in a directory
    List listFiles( ListFlags flags ) const;

    //* expand a file name replacing .. or ~ to full path
    File expanded() const
    { return File(*this).expand(); }

    //@}

    //*@name modifiers
    //@{

    //* accessor
    QString& get() { return value_; }

    //* clear
    void clear() { value_.clear(); }

    //* try create
    bool create() const;

    //* try create directory
    bool createDirectory( const File& = File(".") ) const;

    //* set file as hidden (WIN32 only)
    void setHidden() const;

    //* returns a versioned filename
    /** (add _i at the end with i the first integer for which file is not found) */
    File version() const;

    //* make a backup copy (terminated by ~) of a file, returns backup file
    File backup() const;

    //* return link destination file
    File readLink() const;

    //* removes file from disk
    /**
    returns true if the file does not exists
    or was successfully removed
    */
    bool remove() const;

    //* removes directory from disk, recursively
    bool removeRecursive() const;

    //* rename file
    /** returns true if the file exists and was renamed */
    bool rename( const File& ) const;

    //* copy
    bool copy( const File&, bool = false ) const;

    //* adds path to a file
    /** note: the file is taken raw. No truncation/expension performed.*/
    File& addPath( const File& path, bool absolute = false )
    {
        addPath( value_, path, absolute );
        return *this;
    }

    //* expand a file name replacing .. or ~ to full path
    File& expand()
    {
        expand( value_ );
        return *this;
    }

    //* add trailing slash
    File& addTrailingSlash()
    { addTrailingSlash( value_ ); return *this; }

    //* remove trailing slash
    File& removeTrailingSlash()
    { removeTrailingSlash( value_ ); return *this; }

    //@}

    //*@name utilities
    //@{

    static bool isAbsolute( const QString& );
    static void expand( QString& );
    static void addTrailingSlash( QString& );
    static void removeTrailingSlash( QString& );
    static void addPath( QString& file, const QString& path, bool absolute = false );

    //@}

    //* used to find files pointing to the same link
    using SameLinkFTor = Base::Functor::Unary<File, File, &File::readLink>;

    //* streamer
    friend QTextStream& operator >> ( QTextStream& in, File& file )
    {
        in >> file.value_;
        return in;
    }

    //* streamer
    friend QTextStream& operator >> ( QTextStream& out, const File& file )
    {
        out << file.value_;
        return out;
    }

    private:

    //* value
    QString value_;

};

//* less than operator
inline bool operator < (const File& first, const File& second)
{ return first.get() < second.get(); }

//* equal to operator
inline bool operator == (const File& first, const File& second)
{ return first.get() == second.get(); }

//* specialized copy constructor
template<> File::File( File& );
template<> File::File( const File& );

#endif
