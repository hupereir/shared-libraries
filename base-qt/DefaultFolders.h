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

#include "CppUtil.h"

#include <QHash>
#include <QMap>
#include <QObject>

class DefaultFolders: public QObject
{

    Q_OBJECT

    public:

    //* return singleton
    static DefaultFolders& get();

    //*@name accessors
    //@{

    //* folder type
    enum class Type
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
    const FolderMap& folders() const
    { return folders_; }

    //* return type for a given folder
    Type type( const File& ) const;

    //* return file for a given type
    File file( Type ) const;

    //* name for a given type
    QString name( Type ) const;

    //* return icon name for a given type
    QString iconName( Type ) const;

    //@}


    //*@name modifiers
    //@{

    //* initialize folder names
    void initializeFolderNames();

    //* initialize icon names
    void initializeIconNames();

    //@}

    protected:

    //* insert folder in map
    void _insert( const QStringList&, Type );

    //* insert folder in map
    void _insert( const QString&, Type );

    private:

    //* constructor
    explicit DefaultFolders();

    //* map folder and type
    /* this list keeps only one folder per type */
    FolderMap folders_;

    //* map 'all' folders to type
    /* it also uses a hash, for faster access */
    using FolderHash = QHash<File, Type>;
    FolderHash allFolders_;

    //* folder names
    bool folderNamesInitialized_ = false;
    using NameMap = QHash<Type, QString>;
    NameMap names_;

    //* icon names
    bool iconNamesInitialized_ = false;
    using IconMap = QHash<Type, QString>;
    IconMap iconNames_;

};

#endif
