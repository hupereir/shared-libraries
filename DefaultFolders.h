#ifndef DefaultFolders_h
#define DefaultFolders_h

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

#include "File.h"

#include <QMap>
#include <QObject>

class DefaultFolders: public QObject
{

    Q_OBJECT

    public:

    //! return singleton
    static DefaultFolders& get( void );

    //! destructor
    virtual ~DefaultFolders( void )
    {}

    //! folder type
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

    //! folders
    typedef QMap<File, Type> FolderMap;
    typedef QMapIterator<File, Type> FolderMapIterator;
    const FolderMap& folders( void ) const
    { return folders_; }

    //! return type for a given folder
    Type type( const File& ) const;

    //! name for a given type
    QString name( Type ) const;

    //! return icon name for a given type
    QString iconName( Type ) const;

    private:

    //! constructor
    DefaultFolders( void );

    //! map folder and type
    FolderMap folders_;

    //! icon names
    typedef QMap<Type, QString> NameMap;
    NameMap names_;

    //! map folder type and icon name
    typedef QMap<Type, QString> IconMap;
    IconMap iconNames_;

};

#endif
