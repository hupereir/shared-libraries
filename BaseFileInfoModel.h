#ifndef BaseFileInfoModel_h
#define BaseFileInfoModel_h

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

#include "BaseFileInfo.h"
#include "ListModel.h"
#include "TimeStamp.h"

#include <QtCore/QMimeData>
#include <QtCore/QHash>
#include <QtGui/QIcon>

//! FileInfo model. Stores file information for display in lists
template<typename T>
class BaseFileInfoModel : public ListModel<T>
{

    public:

    //! constructor
    BaseFileInfoModel( QObject* parent = 0 ):
        ListModel<T>( parent ),
        showIcons_( true )
    { _installIcons(); }

    //! destructor
    virtual ~BaseFileInfoModel()
    {}

    //! show icons
    void setShowIcons( const bool& value )
    { showIcons_ = value; }

    //! show icons
    bool showIcons( void ) const
    { return showIcons_; }

    //! column type enumeration
    enum ColumnType
    {
        FILE,
        PATH,
        SIZE,
        USER,
        GROUP,
        PERMISSIONS,
        MODIFIED,
        nColumns
    };

    //!@name methods reimplemented from base class
    //@{

    // return data for a given index
    virtual QVariant data(const QModelIndex& index, int role) const
    {
        // check index, role and column
        if( !index.isValid() ) return QVariant();

        // retrieve associated file info
        const T& fileInfo( ListModel<T>::get()[index.row()] );

        // return text associated to file and column
        if( role == Qt::DisplayRole ) {

            if( fileInfo.type() & BaseFileInfo::NAVIGATOR ) return (index.column() == FILE) ? fileInfo.file() : QVariant();

            switch( index.column() )
            {
                case FILE: return fileInfo.file().localName();
                case PATH: return fileInfo.file().path();
                case SIZE: return (fileInfo.type() & BaseFileInfo::FOLDER || !fileInfo.size() ) ? "" : File::sizeString( fileInfo.size() );
                case USER: return fileInfo.user();
                case GROUP: return fileInfo.group();
                case PERMISSIONS: return (fileInfo.type() & BaseFileInfo::NAVIGATOR) ? QString():fileInfo.permissionsString();
                case MODIFIED: return QString( fileInfo.lastModified() ? TimeStamp( fileInfo.lastModified() ).toString() : "" );

                default:
                return QVariant();
            }
        }

        // return icon associated to file
        if( showIcons() && role == Qt::DecorationRole && index.column() == 0 )
        {
            if( fileInfo.isNavigator() ) return _constIcons()[ BaseFileInfo::NAVIGATOR ];
            else return _constIcons()[fileInfo.type()&(BaseFileInfo::FOLDER|BaseFileInfo::DOCUMENT|BaseFileInfo::LINK)];
        }

        return QVariant();

    }

    //! header data
    virtual QVariant headerData(int column, Qt::Orientation orientation, int role = Qt::DisplayRole) const
    {
        if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        {
            switch( column )
            {
                case FILE: return "File";
                case PATH: return "Path";
                case SIZE: return "Size";
                case USER: return "User";
                case GROUP: return "Group";
                case PERMISSIONS: return "Permissions";
                case MODIFIED: return "Last Modified";
                default: return QVariant();
            }
        }

        return QVariant();

    }

    //! number of columns for a given index
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
    { return nColumns; }

    //@}

    //! used to sort FileInfo objects
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //! constructor
        SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
            ItemModel::SortFTor( type, order )
            {}

        //! destructor
        virtual ~SortFTor( void )
        {}

        //! prediction
        virtual bool operator() ( const T& constFirst, const T& constSecond ) const
        {

            T first( constFirst );
            T second( constSecond );
            if( order_ == Qt::DescendingOrder ) std::swap( first, second );

            switch( type_ )
            {

                case SIZE:
                { return first.size() < second.size(); }
                break;

                case USER:
                { return first.user() < second.user(); }

                case GROUP:
                { return first.group() < second.group(); }

                case MODIFIED:
                { return first.lastModified() < second.lastModified(); }
                break;

                default:
                case FILE:
                {
                    if( first.type() & BaseFileInfo::NAVIGATOR ) return true;
                    else if( second.type() & BaseFileInfo::NAVIGATOR ) return false;
                    else if( first.type() &  BaseFileInfo::FOLDER ) return (second.type() & BaseFileInfo::FOLDER) ? first.file() < second.file() : true;
                    else return (second.type() & BaseFileInfo::FOLDER) ? false : first.file().localName() < second.file().localName();
                }
                break;


            }

        }

    };

    protected:

    //! sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    { std::sort( ListModel<T>::_get().begin(), ListModel<T>::_get().end(), SortFTor( (ColumnType) column, order ) ); }


    //! icon cache
    typedef QHash<int, QIcon> IconCache;

    //! type icon cache
    IconCache& _icons( void )
    { return icons_; }

    //! type icon cache
    const IconCache& _constIcons( void ) const
    { return icons_; }

    //! install pixmaps
    virtual void _installIcons( void )
    { return; }

    private:

    //! icon cache
    IconCache icons_;

    //! true if icons are to be shown
    bool showIcons_;

};

#endif
