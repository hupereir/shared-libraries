#ifndef DefaultFolders_h
#define DefaultFolders_h

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

#include <QMap>
#include <QObject>

class DefaultFolders: public QObject
{

    Q_OBJECT

    public:

    //* return singleton
    static DefaultFolders& get( void );

    //* folder type
    enum Type
    {
        Unknown,
        Home,
        Desktop,
        Documents,
        Downloads,
        Music,
        Pictures,
        Templates,
        Videos
    };

    //* folders
    using FolderMap = QMap<File, Type>;
    using FolderMapIterator = QMapIterator<File, Type>;
    const FolderMap& folders( void ) const
    { return folders_; }

    //* return type for a given folder
    Type type( const File& ) const;

    //* name for a given type
    QString name( Type ) const;

    //* return icon name for a given type
    QString iconName( Type ) const;

    protected:

    //* insert folder in map
    void _insert( const QStringList&, Type );

    //* insert folder in map
    void _insert( const QString&, Type );

    //* return default folder name under windows (vista and above)
    QString _defaultFolderName( qint64 );

    private:

    //* constructor
    DefaultFolders( void );

    //* map folder and type
    FolderMap folders_;

    //* icon names
    using NameMap = QMap<Type, QString>;
    NameMap names_;

    //* map folder type and icon name
    using IconMap = QMap<Type, QString>;
    IconMap iconNames_;

};

#endif
