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

#include "BaseFileInfo.h"
#include "ListModel.h"
#include "TimeStamp.h"

#include <QMimeData>
#include <QHash>
#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QPalette>

//! FileInfo model. Stores file information for display in lists
template<typename T>
class BaseFileInfoModel : public ListModel<T>
{

    public:

    //! constructor
    BaseFileInfoModel( QObject* parent = 0 ):
        ListModel<T>( parent ),
        showIcons_( true )
    {}

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
    virtual QVariant data( const QModelIndex&, int ) const;

    //! header data
    virtual QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const;

    //! number of columns for a given index
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const
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
        virtual bool operator() ( const T&, const T& ) const;

    };

    protected:

    //! sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    { std::sort( ListModel<T>::_get().begin(), ListModel<T>::_get().end(), SortFTor( (ColumnType) column, order ) ); }

    //! icon matching given id
    virtual QIcon _icon( const QModelIndex& ) const = 0;

    private:

    //! true if icons are to be shown
    bool showIcons_;

};

//__________________________________________________________________
template<typename T>
QVariant BaseFileInfoModel<T>::data( const QModelIndex& index, int role ) const
{
    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // return text associated to file and column
    switch( role )
    {

        case Qt::DisplayRole:
        {

            // retrieve associated file info
            const T& fileInfo( ListModel<T>::get()[index.row()] );

            if( fileInfo.type() & BaseFileInfo::Navigator ) return (index.column() == FILE) ? fileInfo.file() : QVariant();

            switch( index.column() )
            {
                case FILE: return fileInfo.file().localName();
                case PATH: return fileInfo.file().path();
                case SIZE: return (fileInfo.isFolder() || !fileInfo.size() ) ? "" : File::sizeString( fileInfo.size() );
                case USER: return fileInfo.user();
                case GROUP: return fileInfo.group();
                case PERMISSIONS: return (fileInfo.type() & BaseFileInfo::Navigator) ? QString():fileInfo.permissionsString();
                case MODIFIED: return QString( fileInfo.lastModified() ? TimeStamp( fileInfo.lastModified() ).toString() : "" );

                default:
                return QVariant();
            }

            break;
        }

        case Qt::DecorationRole:
        {

            if( showIcons() && index.column() == 0 )
            { return _icon( index ); }

            break;
        }

        case Qt::ForegroundRole:
        {
            const T& fileInfo( ListModel<T>::get()[index.row()] );
            if( fileInfo.isHidden() && ( this->flags( index )&Qt::ItemIsEnabled ) )
            {
                QColor color( QPalette().color( QPalette::Text ) );
                color.setAlphaF( 0.7 );
                return color;
            }

            break;

        }

        case Qt::FontRole:
        {
            const T& fileInfo( ListModel<T>::get()[index.row()] );
            if( fileInfo.isLink() )
            {
                QFont font( QApplication::font() );
                font.setItalic( true );
                return font;
            }
        }

        default: break;

    }

    return QVariant();

}

//__________________________________________________________________
template<typename T>
QVariant BaseFileInfoModel<T>::headerData( int column, Qt::Orientation orientation, int role ) const
{
    if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    {
        switch( column )
        {
            case FILE: return ItemModel::tr( "File" );
            case PATH: return ItemModel::tr( "Path" );
            case SIZE: return ItemModel::tr( "Size" );
            case USER: return ItemModel::tr( "Owner" );
            case GROUP: return ItemModel::tr( "Group" );
            case PERMISSIONS: return ItemModel::tr( "Permissions" );
            case MODIFIED: return ItemModel::tr( "Modified" );
            default: return QVariant();
        }
    }

    return QVariant();

}

//__________________________________________________________________
template<typename T>
bool BaseFileInfoModel<T>::SortFTor::operator() ( const T& constFirst, const T& constSecond ) const
{

    // Navigator come first
    // Folders come next
    if( constFirst.isNavigator() ) return true;
    else if( constSecond.isNavigator() ) return false;
    else if(
        (constFirst.type()&(BaseFileInfo::Folder|BaseFileInfo::Document)) !=
        (constSecond.type()&(BaseFileInfo::Folder|BaseFileInfo::Document)) )
    { return constFirst.isFolder(); }

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
        { return first.file().localName().compare( second.file().localName(), Qt::CaseInsensitive ) < 0; }
        break;


    }

}

#endif
