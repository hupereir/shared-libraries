#ifndef BaseFileInfoModel_h
#define BaseFileInfoModel_h

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

#include "BaseFileInfo.h"
#include "ListModel.h"
#include "TimeStamp.h"

#include <QHash>
#include <QApplication>
#include <QIcon>
#include <QFont>
#include <QPalette>

namespace Base
{
    //* custom role
    enum ItemDataRole
    {
        FileTypeRole = Qt::UserRole+1
    };
}

//* FileInfo model. Stores file information for display in lists
template<typename T>
class BaseFileInfoModel : public ListModel<T>
{

    public:

    //* constructor
    explicit BaseFileInfoModel( QObject* parent = nullptr ):
        ListModel<T>( parent )
    {}

    //* show icons
    void setShowIcons( bool value )
    { showIcons_ = value; }

    //* show icons
    bool showIcons( void ) const
    { return showIcons_; }

    //* column type enumeration
    enum ColumnType
    {
        Filename,
        Path,
        Size,
        User,
        Group,
        Permissions,
        Modified,
        nColumns
    };

    //*@name methods reimplemented from base class
    //@{

    // return data for a given index
    QVariant data( const QModelIndex&, int ) const override;

    //* header data
    QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const override;

    //* number of columns for a given index
    int columnCount( const QModelIndex& = QModelIndex() ) const override
    { return nColumns; }

    //@}

    //* used to sort FileInfo objects
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //* constructor
        explicit SortFTor( int type, Qt::SortOrder order ):
            ItemModel::SortFTor( type, order )
            {}

        //* prediction
        virtual bool operator() ( const T&, const T& ) const;

    };

    protected:

    //* sort
    void _sort( int column, Qt::SortOrder order ) override
    { std::sort( ListModel<T>::_get().begin(), ListModel<T>::_get().end(), SortFTor( (ColumnType) column, order ) ); }

    //* icon matching given id
    virtual QIcon _icon( const QModelIndex& ) const
    { return QIcon(); }

    private:

    //* true if icons are to be shown
    bool showIcons_ = true;

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

            if( fileInfo.type() & BaseFileInfo::Navigator ) return (index.column() == Filename ) ? fileInfo.file() : QVariant();

            switch( index.column() )
            {
                case Filename: return fileInfo.file().localName();
                case Path: return fileInfo.file().path();
                case Size: return (fileInfo.isFolder() || !fileInfo.size() ) ? "" : File::sizeString( fileInfo.size() );
                case User: return fileInfo.user();
                case Group: return fileInfo.group();
                case Permissions: return (fileInfo.type() & BaseFileInfo::Navigator) ? QString():fileInfo.permissionsString();
                case Modified: return QString( fileInfo.lastModified().isValid() ? fileInfo.lastModified().toString() : QString() );

                default:
                return QVariant();
            }

            break;
        }

        case Qt::TextAlignmentRole:
        {

            switch( index.column() )
            {
                case Size: return (int)(Qt::AlignRight|Qt::AlignVCenter);
                case Modified: return (int)(Qt::AlignCenter);

                default:
                return QVariant();
            }

        }

        case Qt::DecorationRole:
        {

            if( showIcons_ && index.column() == 0 )
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

            break;

        }

        case Base::FileTypeRole:
        {
            const T& fileInfo( ListModel<T>::get()[index.row()] );
            return fileInfo.type();
        }

        default: break;

    }

    return QVariant();

}

//__________________________________________________________________
template<typename T>
QVariant BaseFileInfoModel<T>::headerData( int column, Qt::Orientation orientation, int role ) const
{

    switch( role )
    {
        case Qt::DisplayRole:
        {
            switch( column )
            {
                case Filename: return ItemModel::tr( "File" );
                case Path: return ItemModel::tr( "Path" );
                case Size: return ItemModel::tr( "Size " );
                case User: return ItemModel::tr( "Owner" );
                case Group: return ItemModel::tr( "Group" );
                case Permissions: return ItemModel::tr( "Permissions" );
                case Modified: return ItemModel::tr( "Modified" );
                default: return QVariant();
            }
            break;

        }

        case Qt::TextAlignmentRole:
        {

            switch( column )
            {
                case Size: return (int)(Qt::AlignRight|Qt::AlignVCenter);
                case Modified: return (int)(Qt::AlignCenter);

                default:
                return QVariant();
            }
            break;

        }

        default: break;
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

        case Size:
        { return first.size() < second.size(); }
        break;

        case User:
        { return first.user() < second.user(); }

        case Group:
        { return first.group() < second.group(); }

        case Modified:
        { return first.lastModified() < second.lastModified(); }
        break;

        default:
        case Filename:
        { return first.file().localName().compare( second.file().localName(), Qt::CaseInsensitive ) < 0; }
        break;


    }

}

#endif
