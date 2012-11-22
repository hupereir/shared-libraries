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

#include "FileRecordModel.h"

#include "FileRecordBaseProperties.h"
#include "IconEngine.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <algorithm>
#include <cassert>

//__________________________________________________________________
FileRecordModel::IconCache& FileRecordModel::_icons( void )
{
    static IconCache cache;
    return cache;
}

//__________________________________________________________________
FileRecordModel::FileRecordModel( QObject* parent ):
    ListModel<FileRecord>( parent ),
    Counter( "FileRecordModel" ),
    dragEnabled_( false ),
    useLocalNames_( true ),
    showIcons_( true ),
    iconPropertyId_( FileRecord::PropertyId::get( FileRecordProperties::ICON ) )
{
    Debug::Throw("FileRecordModel::FileRecordModel.\n" );

    columnTitles_ << "File";
    columnTitles_ << "Path";
    columnTitles_ << "Last Accessed";

    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

}

//__________________________________________________________________
int FileRecordModel::columnCount(const QModelIndex &parent) const
{ return columnTitles_.size(); }

//__________________________________________________________________
int FileRecordModel::findColumn( const QString& value ) const
{
    for( int index = 0; index < columnTitles_.size(); ++index )
    { if( columnTitles_[index] == value ) return index;  }
    return -1;
}

//__________________________________________________________________
Qt::ItemFlags FileRecordModel::flags( const QModelIndex& index ) const
{

    // default flags
    Qt::ItemFlags flags;
    if( !( index.isValid() && contains( index ) ) ) return flags;

    // check associated record validity
    const FileRecord& record( get(index) );
    if( !record.isValid() ) return flags;

    // default flags
    flags |=  Qt::ItemIsEnabled |  Qt::ItemIsSelectable;
    if( dragEnabled() && index.column() == FILE ) flags |= Qt::ItemIsDragEnabled;

    return flags;

}

//__________________________________________________________________
QVariant FileRecordModel::data( const QModelIndex& index, int role ) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

    // retrieve associated file info
    const FileRecord& record( get(index) );

    // return text associated to file and column
    if( role == Qt::DisplayRole ) {

        switch( index.column() )
        {

            case FILE:
            {
                // store local nmae
                const QString localName( useLocalNames_ ? record.file().localName(): record.file() );

                // loop over previous rows to find a match and increment version number
                unsigned int version( 0 );
                for( int row = 0; row < index.row(); row++ )
                {
                    const QString rowName( useLocalNames_ ?
                        get( this->index( row, FILE ) ).file().localName() :
                        get( this->index( row, FILE ) ).file() );
                    if( localName == rowName ) version++;
                }

                // form output string.
                QString buffer;
                QTextStream what( &buffer );
                if( localName.isEmpty() ) what << "untitled";
                else what << localName;
                if( version ) what << " (" << version+1 << ")";
                return buffer;
            }

            case PATH: return record.file().path();
            case TIME: return TimeStamp( record.time() ).toString();
            default:
            if( index.column() < (int) columnTitles_.size() && record.hasProperty( columnTitles_[index.column()] ) )
            { return record.property( columnTitles_[index.column()] ); }
            else return QVariant();

        }

    } else if( showIcons_ && role == Qt::DecorationRole && index.column() == FILE ) {

        // icon
        return record.hasProperty( iconPropertyId_ ) ? _icon( record.property( iconPropertyId_ ) ):_icon();

    }

    return QVariant();

}

//__________________________________________________________________
QVariant FileRecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if(
        orientation == Qt::Horizontal &&
        role == Qt::DisplayRole &&
        section >= 0 &&
        section < (int) columnTitles_.size() )
    { return columnTitles_[section]; }

    // return empty
    return QVariant();

}

//____________________________________________________________
void FileRecordModel::_sort( int column, Qt::SortOrder order )
{ std::sort( _get().begin(), _get().end(), SortFTor( column, order, columnTitles_ ) ); }

//____________________________________________________________
void FileRecordModel::_add( const ValueType& value )
{
    _updateColumns( value );
    ListModel<FileRecord>::_add( value );
}

//____________________________________________________________
void FileRecordModel::_updateConfiguration( void )
{
    Debug::Throw( "FileRecordModel::_updateConfiguration.\n" );
    _icons().clear();
}

//____________________________________________________________
void FileRecordModel::_updateColumns( const ValueType& value )
{

    Debug::Throw( "FileRecordModel::_updateColumns.\n" );

    // loop over available properties
    const FileRecord::PropertyMap& properties( value.properties() );
    for( FileRecord::PropertyMap::const_iterator iter = properties.begin(); iter != properties.end(); ++iter )
    {
        // look for property name in list of columns
        if( std::find( columnTitles_.begin(), columnTitles_.end(), FileRecord::PropertyId::get( iter.key() ) ) == columnTitles_.end() )
        { columnTitles_ << FileRecord::PropertyId::get( iter.key() ); }

    }

}

//________________________________________________________
bool FileRecordModel::SortFTor::operator () ( FileRecord first, FileRecord second ) const
{

    if( order_ == Qt::DescendingOrder ) std::swap( first, second );

    switch( type_ )
    {

        case FILE: return first.file().localName().compare( second.file().localName(), Qt::CaseInsensitive ) < 0;
        case PATH: return first.file().path() < second.file().path();
        case TIME: return (first.time() != second.time() ) ? (first.time() < second.time()):first.file().localName() < second.file().localName();
        default:
        {
            if( type_ < columnTitles_.size() )
            {
                QString name( columnTitles_[type_] );
                QString firstProperty( first.property( name ) );
                QString secondProperty( second.property( name ) );
                return ( firstProperty != secondProperty ) ? firstProperty < secondProperty :  first.file().localName() < second.file().localName();
            } else return false;

        }

    }

}

//________________________________________________________
const QIcon& FileRecordModel::_icon( const QString& name )
{

    Debug::Throw( "FileRecordModel::_icon.\n" );

    IconCache::const_iterator iter( _icons().find( name ) );
    if( iter != _icons().end() ) return iter.value();
    else return _icons().insert( name, IconEngine::get( name ) ).value();

}
