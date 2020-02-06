#ifndef BaseFileInfo_h
#define BaseFileInfo_h

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

#include "base_qt_export.h"
#include "Debug.h"
#include "File.h"
#include "Functors.h"
#include "Operators.h"
#include "TimeStamp.h"

#include <QString>
#include <QTextStream>
#include <QList>
#include <QPair>
#include <QDomElement>
#include <QDomDocument>

#include <algorithm>

//* Some Xml definitions
namespace Xml
{

    static const QString FileInfoList( QStringLiteral("FileInfoList") );
    static const QString FileInfo( QStringLiteral("FileInfo") );
    static const QString File( QStringLiteral("File") );
    static const QString Type( QStringLiteral("Type") );
    static const QString Size( QStringLiteral("Size") );
    static const QString Alias( QStringLiteral("Alias") );
    static const QString LastModified( QStringLiteral("LastModified") );
    static const QString User( QStringLiteral("user") );
    static const QString Group( QStringLiteral("group") );
    static const QString Permissions( QStringLiteral("permissions") );

}

//* store a file and needed informations for Ftp transfer
class BASE_QT_EXPORT BaseFileInfo
{

    public:

    using List=QList<BaseFileInfo>;

    //* file properties
    enum Type
    {
        None = 0,
        Document = 1<<0,
        Folder = 1<<1,
        Navigator = 1<<2,
        Link = 1<<3,
        Broken = 1<<4,
        Hidden = 1<<5,
        Remote = 1<<6,
        Clipped = 1<<7,
        Last = 1<<8
    };

    enum {Shift = 8};

    Q_DECLARE_FLAGS( TypeFlags, Type )

    //* drag string
    static const QString MimeType;

    //* constructor
    explicit BaseFileInfo( const QString& file = QString(), TypeFlags type = None ):
        file_( file ),
        type_( type )
    {}

    //* constructor from DOM element
    explicit BaseFileInfo( const QDomElement& );

    //* destructor
    virtual ~BaseFileInfo() = default;

    //* dump to dom element
    virtual QDomElement domElement( QDomDocument& ) const;

    //*@name accessors
    //@{

    //* validity
    bool isValid() const
    { return !file_.isEmpty(); }

    //* file
    virtual const File& file() const
    { return file_; }

    //* alias
    virtual bool hasAlias() const
    { return !alias_.isEmpty(); }

    //* alias
    virtual const QString& alias() const
    { return alias_; }

    //* file type
    virtual int type() const
    { return type_; }

    //* location
    virtual TypeFlags location() const
    { return type_ & Remote; }

    //* file is local
    virtual bool isLocal() const
    { return !(type_&Remote); }

    //* file is remote
    virtual bool isRemote() const
    { return type_&Remote; }

    //* file is document
    virtual bool isDocument() const
    { return type_&Document; }

    //* file is directory
    virtual bool isFolder() const
    { return type_&Folder; }

    //* file is navigator (.|..)
    virtual bool isNavigator() const
    { return type_&Navigator; }

    //* file is link
    virtual bool isLink() const
    { return type_&Link; }

    //* file is broken link
    virtual bool isBrokenLink() const
    { return (type_&Broken); }

    //* file is hidden
    virtual bool isHidden() const
    { return type_&Hidden; }

    //* file is clipped
    virtual bool isClipped() const
    { return type_&Clipped; }

    //* file size
    virtual qint64 size() const
    { return size_; }

    //* return true if lastModified_ is valid and smaller than argument
    virtual bool isOlder( const TimeStamp& time ) const
    { return ( lastModified_.isValid() && lastModified_ < time ); }

    //* file last modification
    virtual const TimeStamp& lastModified() const
    { return lastModified_; }

    //* user
    virtual const QString& user() const
    { return user_; }

    //* group
    virtual const QString& group() const
    { return group_; }

    //* permissions from unix
    static QFile::Permissions permissions( mode_t );

    //* permissions
    virtual QFile::Permissions permissions() const
    { return permissions_; }

    //* permissions
    static mode_t unixPermissions( QFile::Permissions );

    //* permissions
    mode_t unixPermissions() const
    { return unixPermissions( permissions_ ); }

    //* type string
    virtual QString typeString() const;

    //* permission string
    virtual QString permissionsString() const;

    //@}

    //*@name modifiers
    //@{

    //* file
    virtual void setFile( const File& file )
    { file_ = file; }

    //* alias
    virtual void setAlias( const QString& value )
    { alias_ = value; }

    //* file type
    virtual void setType( TypeFlags type )
    { type_ = type; }

    //* set file as local
    virtual void setLocal()
    { type_ &= (~Remote); }

    //* set file as remote
    virtual void setRemote()
    { type_ |= Remote; }

    //* set file as file
    virtual void setIsDocument()
    {
        type_ &= (~Folder);
        type_ &= (~Navigator);
        type_ |= Document;
    }

    //* set file as directory
    virtual void setIsFolder()
    {
        type_ &= (~Document);
        type_ |= Folder;
    }

    //* set file as file
    virtual void setIsNavigator()
    {
        type_ &= (~Document);
        type_ |= TypeFlags( Folder|Navigator );
    }

    //* set file as link
    virtual void setIsLink( bool value = true )
    {
        if( value ) type_ |= Link;
        else type_ &= ~Link;
    }

    //* set file as broken link
    virtual void setIsBrokenLink( bool value = true )
    {
        if( value ) type_ |= TypeFlags( Link | Broken );
        else type_ &= ~Broken;
    }

    //* set file as hidden
    virtual void setIsHidden( bool value = true )
    {
        if( value ) type_ |= Hidden;
        else type_ &= ~Hidden;
    }

    //* file size
    virtual void setSize( qint64 size )
    { size_ = size; }

    //* file last modification
    virtual void setLastModified( const TimeStamp& time )
    { lastModified_ = time; }

    //* user
    virtual void setUser( const QString& user )
    { user_ = user; }

    //* group
    virtual void setGroup( const QString& group )
    { group_ = group; }

    //* permissions
    virtual void setUnixPermissions( mode_t value )
    { setPermissions( permissions( value ) ); }

    //* permissions
    virtual void setPermissions( QFile::Permissions permissions )
    { permissions_ = permissions; }

    //* permissions (from string)
    virtual void setPermissions( const QString& );

    //* update internal storage from file
    virtual void update();

    //* update internal storage from existing file info
    virtual void updateFrom( const BaseFileInfo& );

    //@}

    //* used to count files of a given type
    class BASE_QT_EXPORT SameTypeFTor
    {
        public:

        //* constructor
        explicit SameTypeFTor( TypeFlags type ):
            type_( type )
        {}

        //* predicate
        bool operator() (const BaseFileInfo& info ) const
        { return (info.type()&type_) == type_; }

        private:

        //* predicted type
        TypeFlags type_;

    };

    //* used to retrieve documents
    using IsDocumentFTor = Base::Functor::UnaryTrue<BaseFileInfo, &BaseFileInfo::isDocument>;

    //* used to retrieve local files
    using IsLocalFTor = Base::Functor::UnaryTrue<BaseFileInfo, &BaseFileInfo::isLocal>;

    //* used to retrieve remote files
    using IsRemoteFTor = Base::Functor::UnaryTrue<BaseFileInfo, &BaseFileInfo::isRemote>;

    //* used to retrieve folders
    class BASE_QT_EXPORT IsFolderFTor
    {
        public:

        //* predicate
        bool operator() (const BaseFileInfo& info ) const
        { return info.isFolder() && !info.isNavigator(); }

    };

    //* used to retrieve folder
    using IsLinkFTor = Base::Functor::UnaryTrue<BaseFileInfo, &BaseFileInfo::isLink>;

    //* used to retrieve FileInfo with matching file
    using SameFileFTor = Base::Functor::Unary<BaseFileInfo, const File&, &BaseFileInfo::file>;

    //* file info list
    class BASE_QT_EXPORT Description
    {

        public:

        //* description flags
        enum Flag
        {
            None = 0,
            Designate = 1<<0,
            LowerCase = 1<<1,
            Detail = 1<<2
        };

        //* constructor
        explicit Description( int flags = 0 ):
            flags_( flags )
        {}

        //* destructor
        ~Description() = default;

        template<class T>
        QString get( const QList<T>& ) const;

        private:

        //* description flags
        int flags_ = 0;

    };

    private:

    //* file
    File file_;

    //* alias
    QString alias_;

    //* file type
    TypeFlags type_;

    //* file size
    qint64 size_ = 0;

    //* last modification
    TimeStamp lastModified_;

    //* user name
    QString user_;

    //* group name
    QString group_;

    //* permissions
    QFile::Permissions permissions_;

    //* streamer
    friend QTextStream& operator << ( QTextStream& out, const BaseFileInfo& file )
    {
        out << file.file_ << " (";

        if( file.isNavigator() ) out << "navigator";
        else if( file.isFolder() ) out << "folder";
        else if( file.isDocument() ) out << "document";
        else out << "unknown type";

        if( file.isBrokenLink() ) out << " broken";
        if( file.isLink() ) out << " link";
        if( file.isHidden() ) out << " hidden";

        out << ",";
        if( file.isLocal() ) out << " local";
        else if( file.isRemote() ) out << " remote";
        else out << " unknown location";
        out << ")";
        return out;
    }

};


//* equal to operator
inline bool operator == (const BaseFileInfo& first, const BaseFileInfo& second)
{ return first.file() == second.file() && first.type() == second.type(); }

//* less than operator
inline bool operator < (const BaseFileInfo& first, const BaseFileInfo& second)
{
    if( first.file() != second.file() ) return first.file() < second.file();
    else return first.type() < second.type();
}

//________________________________________________________________
template <typename T>
QString BaseFileInfo::Description::get( const QList<T>& files ) const
{
    if( files.empty() ) return QString();

    QString buffer;
    if( files.size() == 1 )
    {


        if( files.front().isLink() ) buffer = QString( QObject::tr( "%1 Symbolic Link" ) ).arg( (flags_&Designate) ? QObject::tr( "This" ): QObject::tr( "One" ) );
        else if( files.front().isFolder() ) buffer = QString( QObject::tr( "%1 Folder" ) ).arg( (flags_&Designate) ? QObject::tr( "This" ): QObject::tr( "One" ) );
        else buffer = QString( QObject::tr( "%1 File" ) ).arg( (flags_&Designate) ? QObject::tr( "This" ): QObject::tr( "One" ) );

    } else {

        if( flags_ & Detail )
        {
            const int nFolders( std::count_if( files.begin(), files.end(), BaseFileInfo::IsFolderFTor() ) );
            if( nFolders == 1 ) buffer = QObject::tr( "one Folder" );
            else if( nFolders > 1 ) buffer = QObject::tr( "%1 Folders" ).arg( nFolders );

            const int nDocuments( std::count_if( files.begin(), files.end(), BaseFileInfo::IsDocumentFTor() ) );
            if( nDocuments > 0 && nFolders > 0 ) buffer += ", ";
            if( nDocuments == 1 ) buffer += QObject::tr( "one File" );
            else if( nDocuments > 1 ) buffer += QObject::tr( "%1 Files" ).arg( nDocuments );

        } else {

            if( int(std::count_if( files.begin(), files.end(), BaseFileInfo::IsLinkFTor() )) == files.size() ) buffer = QString( QObject::tr( "%1 Symbolic Links" ) ).arg( files.size() );
            else if( int(std::count_if( files.begin(), files.end(), BaseFileInfo::IsFolderFTor() )) == files.size() ) buffer = QString( QObject::tr( "%1 Folders" ) ).arg( files.size() );
            else if( int(std::count_if( files.begin(), files.end(), BaseFileInfo::IsDocumentFTor() )) == files.size() ) buffer = QString( QObject::tr( "%1 Files" ) ).arg( files.size() );
            else buffer = QString( QObject::tr( "%1 Items" ) ).arg( files.size() );
        }

    }

    return (flags_&LowerCase) ? buffer.toLower() : buffer;
}

Q_DECLARE_OPERATORS_FOR_FLAGS( BaseFileInfo::TypeFlags)

#endif
