#ifndef BaseFileInfo_h
#define BaseFileInfo_h

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
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Debug.h"
#include "File.h"

#include <QString>
#include <QTextStream>
#include <QList>
#include <QPair>
#include <QDomElement>
#include <QDomDocument>

#include <algorithm>

//! Some Xml definitions
namespace XML
{

    static const QString FILEINFO_LIST( "FileInfoList" );
    static const QString FILEINFO( "FileInfo" );
    static const QString FILE( "File" );
    static const QString TYPE( "Type" );
    static const QString SIZE( "Size" );
    static const QString ALIAS( "Alias" );
    static const QString LAST_MODIFIED( "LastModified" );
    static const QString USER( "user" );
    static const QString GROUP( "group" );
    static const QString PERMISSIONS( "permissions" );

}

//! store a file and needed informations for Ftp transfer
class BaseFileInfo
{

    public:

    //! file properties
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
        Last = 1<<7
    };

    enum {Shift = 7};

    Q_DECLARE_FLAGS( TypeFlags, Type )

    //! drag string
    static const QString MimeType;

    //! constructor
    BaseFileInfo( const QString& file = QString(), TypeFlags type = None ):
        file_( file ),
        type_( type ),
        size_( 0 ),
        lastModified_( -1 )
    {}

    //! constructor from DOM element
    BaseFileInfo( const QDomElement& );

    //! destructor
    virtual ~BaseFileInfo( void )
    {}

    //! dump to dom element
    virtual QDomElement domElement( QDomDocument& ) const;

    //! equal to operator
    virtual bool operator == (const BaseFileInfo& other) const
    { return file() == other.file() && type() == other.type(); }

    //! equal to operator
    virtual bool operator != (const BaseFileInfo& other) const
    { return !( *this == other ); }

    //! less than operator
    virtual bool operator < (const BaseFileInfo& other) const
    {
        if( file() != other.file() ) return file() < other.file();
        return type() < other.type();
    }

    //!@name accessors
    //@{

    //! file
    virtual const File& file( void ) const
    { return file_; }

    //! alias
    virtual bool hasAlias( void ) const
    { return !alias_.isEmpty(); }

    //! alias
    virtual const QString& alias( void ) const
    { return alias_; }

    //! file type
    virtual int type( void ) const
    { return type_; }

    //! location
    virtual TypeFlags location( void ) const
    { return type_ & Remote; }

    //! file is local
    virtual bool isLocal( void ) const
    { return !(type_&Remote); }

    //! file is remote
    virtual bool isRemote( void ) const
    { return type_&Remote; }

    //! file is document
    virtual bool isDocument( void ) const
    { return type_&Document; }

    //! file is directory
    virtual bool isFolder( void ) const
    { return type_&Folder; }

    //! file is navigator (.|..)
    virtual bool isNavigator( void ) const
    { return type_&Navigator; }

    //! file is link
    virtual bool isLink( void ) const
    { return type_&Link; }

    //! file is broken link
    virtual bool isBrokenLink( void ) const
    { return (type_&Broken); }

    //! file is hidden
    virtual bool isHidden( void ) const
    { return type_&Hidden; }

    //! file size
    virtual qint64 size( void ) const
    { return size_; }

    //! return true if lastModified_ is valid and smaller than argument
    virtual bool isOlder( const time_t& time ) const
    { return ( lastModified_ >= 0 && lastModified_ < time ); }

    //! file last modification
    virtual time_t lastModified( void ) const
    { return lastModified_; }

    //! user
    virtual const QString& user( void ) const
    { return user_; }

    //! group
    virtual const QString& group( void ) const
    { return group_; }

    //! permissions
    virtual QFile::Permissions permissions( void ) const
    { return permissions_; }

    //! type string
    virtual QString typeString( void ) const;

    //! permission string
    virtual QString permissionsString( void ) const;

    //@}

    //!@name modifiers
    //@{

    //! file
    virtual void setFile( const File& file )
    { file_ = file; }

    //! alias
    virtual void setAlias( const QString& value )
    { alias_ = value; }

    //! file type
    virtual void setType( TypeFlags type )
    { type_ = type; }

    //! set file as local
    virtual void setLocal( void )
    { type_ &= (~Remote); }

    //! set file as remote
    virtual void setRemote( void )
    { type_ |= Remote; }

    //! set file as file
    virtual void setIsDocument( void )
    {
        type_ &= (~Folder);
        type_ &= (~Navigator);
        type_ |= Document;
    }

    //! set file as directory
    virtual void setIsFolder( void )
    {
        type_ &= (~Document);
        type_ |= Folder;
    }

    //! set file as file
    virtual void setIsNavigator( void )
    {
        type_ &= (~Document);
        type_ |= TypeFlags( Folder|Navigator );
    }

    //! set file as link
    virtual void setIsLink( bool value = true )
    {
        if( value ) type_ |= Link;
        else type_ &= ~Link;
    }

    //! set file as broken link
    virtual void setIsBrokenLink( bool value = true )
    {
        if( value ) type_ |= TypeFlags( Link | Broken );
        else type_ &= ~Broken;
    }

    //! set file as hidden
    virtual void setIsHidden( bool value = true )
    {
        if( value ) type_ |= Hidden;
        else type_ &= ~Hidden;
    }

    //! file size
    virtual void setSize( qint64 size )
    { size_ = size; }

    //! file last modification
    virtual void setLastModified( const time_t& time )
    { lastModified_ = time; }

    //! user
    virtual void setUser( const QString& user )
    { user_ = user; }

    //! group
    virtual void setGroup( const QString& group )
    { group_ = group; }

    //! permissions
    virtual void setPermissions( const QFile::Permissions& permissions )
    { permissions_ = permissions; }

    //! permissions (from string)
    virtual void setPermissions( const QString& );

    //! update internal storage from file
    virtual void update( void );

    //! update internal storage from existing file info
    virtual void updateFrom( const BaseFileInfo& );

    //@}

    //! used to count files of a given type
    class SameTypeFTor
    {
        public:

        //! constructor
        SameTypeFTor( TypeFlags type ):
            type_( type )
        {}

        //! predicate
        bool operator() (const BaseFileInfo& info ) const
        { return (info.type()&type_) == type_; }

        private:

        //! predicted type
        TypeFlags type_;

    };

    //! used to retrieve documents
    class IsDocumentFTor
    {
        public:

        //! predicate
        bool operator() (const BaseFileInfo& info ) const
        { return info.isDocument() && !info.isNavigator(); }

    };

    //! used to retrieve folders
    class IsFolderFTor
    {
        public:

        //! predicate
        bool operator() (const BaseFileInfo& info ) const
        { return info.isFolder() && !info.isNavigator(); }

    };

    //! used to retrieve folder
    class IsLinkFTor
    {
        public:

        //! predicate
        bool operator() (const BaseFileInfo& info ) const
        { return info.isLink(); }

    };

    //! used to retrieve FileInfo with matching file
    class SameFileFTor
    {
        public:

        //! constructor
        SameFileFTor( const BaseFileInfo& info ):
            file_( info.file() )
        {}

        //! constructor
        SameFileFTor( const File& file ):
            file_( file )
        {}

        //! predicate
        bool operator() (const BaseFileInfo& info ) const
        { return info.file() == file_; }

        private:

        //! predicted file
        File file_;

    };

    //! file info list
    template < typename T >
    class BaseList: public QList<T>
    {

        public:

        //! constructor
        BaseList( void )
        {}

        //! constructor
        BaseList( const QList<T>& other ):
            QList<T>( other )
        {}

        //! destructor
        virtual ~BaseList( void )
        {}

        //! description
        enum Flag
        {
            None = 0,
            Designate = 1<<0,
            LowerCase = 1<<1
        };

        QString description( unsigned int = 0 ) const;

    };

    private:

    //! file
    File file_;

    //! alias
    QString alias_;

    //! file type
    TypeFlags type_;

    //! file size
    qint64 size_;

    //! last modification
    time_t lastModified_;

    //! user name
    QString user_;

    //! group name
    QString group_;

    //! permissions
    QFile::Permissions permissions_;

    //! streamer
    friend QTextStream& operator << ( QTextStream& out, const BaseFileInfo& file )
    {
        out << file.file() << " (";

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


//________________________________________________________________
template <typename T>
QString BaseFileInfo::BaseList<T>::description( unsigned int flags ) const
{
    Debug::Throw( "BaseFileInfo::BaseList::description.\n" );

    if( this->empty() ) return QString();
    QString buffer;
    if( this->size() == 1 )
    {


        if( this->front().isLink() ) buffer = QString( QObject::tr( "%1 Symbolic Link" ) ).arg( (flags&Designate) ? QObject::tr( "This" ): QObject::tr( "One" ) );
        else if( this->front().isFolder() ) buffer = QString( QObject::tr( "%1 Folder" ) ).arg( (flags&Designate) ? QObject::tr( "This" ): QObject::tr( "One" ) );
        else buffer = QString( QObject::tr( "%1 File" ) ).arg( (flags&Designate) ? QObject::tr( "This" ): QObject::tr( "One" ) );

    } else {

        if( int(std::count_if( this->begin(), this->end(), BaseFileInfo::IsLinkFTor() )) == this->size() ) buffer = QString( QObject::tr( "%1 Symbolic Links" ) ).arg( this->size() );
        else if( int(std::count_if( this->begin(), this->end(), BaseFileInfo::IsFolderFTor() )) == this->size() ) buffer = QString( QObject::tr( "%1 Folders" ) ).arg( this->size() );
        else if( int(std::count_if( this->begin(), this->end(), BaseFileInfo::IsDocumentFTor() )) == this->size() ) buffer = QString( QObject::tr( "%1 Files" ) ).arg( this->size() );
        else buffer = QString( QObject::tr( "%1 Items" ) ).arg( this->size() );

    }

    return (flags&LowerCase) ? buffer.toLower() : buffer;
}

Q_DECLARE_OPERATORS_FOR_FLAGS( BaseFileInfo::TypeFlags)

#endif
