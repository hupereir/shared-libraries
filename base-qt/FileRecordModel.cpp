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

#include "FileRecordModel.h"

#include "FileRecordBaseProperties.h"
#include "IconEngine.h"
#include "Operators.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QUrl>

#include <algorithm>

//__________________________________________________________________
FileRecordModel::IconCache& FileRecordModel::_icons()
{
    static IconCache cache;
    return cache;
}

//__________________________________________________________________
FileRecordModel::FileRecordModel( QObject* parent ):
    ListModel( parent ),
    Counter( "FileRecordModel" ),
    iconPropertyId_( FileRecord::PropertyId::get( FileRecordProperties::Icon ) ),
    columnTitles_( { tr( "File" ), tr( "Path" ), tr( "Last Accessed" ) } )
{
    Debug::Throw("FileRecordModel::FileRecordModel.\n" );
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &FileRecordModel::_updateConfiguration );
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
    if( !contains( index ) ) return flags;

    // check associated record validity
    const FileRecord& record( get(index) );
    if( !record.isValid() ) return flags;

    // default flags
    flags |=  Qt::ItemIsEnabled |  Qt::ItemIsSelectable;
    if( index.column() == FileName ) flags |= Qt::ItemIsDragEnabled;

    return flags;

}

//__________________________________________________________________
QVariant FileRecordModel::data( const QModelIndex& index, int role ) const
{

    // check index
    if( !contains( index ) ) return QVariant();

    // retrieve associated file info
    const FileRecord& record( get(index) );

    // return text associated to file and column
    if( role == Qt::DisplayRole ) {

        switch( index.column() )
        {

            case FileName:
            {
                // store local nmae
                const QString localName( useLocalNames_ ? record.file().localName(): record.file() );

                // loop over previous rows to find a match and increment version number
                int version( 0 );
                for( int row = 0; row < index.row(); row++ )
                {
                    const QString rowName( useLocalNames_ ?
                        get( this->index( row, FileName ) ).file().localName() :
                        get( this->index( row, FileName ) ).file() );
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

            case Path: return record.file().path().get();
            case Time: return TimeStamp( record.time() ).toString();
            default:
            if( index.column() < (int) columnTitles_.size() && record.hasProperty( columnTitles_[index.column()] ) )
            { return record.property( columnTitles_[index.column()] ); }
            else return QVariant();

        }

    } else if( showIcons_ && role == Qt::DecorationRole && index.column() == FileName ) {

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

//______________________________________________________________________
QMimeData* FileRecordModel::mimeData(const QModelIndexList &indexes) const
{

    // get selected filenames
    QOrderedSet<QString> filenames;
    for( const auto& index:indexes )
    {

        if( !index.isValid() ) continue;
        const FileRecord record( get(index) );
        filenames.insert( record.file() );

    }

    if( filenames.empty() ) return nullptr;
    else {

        QMimeData* mimeData = new QMimeData;

        // fill text data
        {
            QString fullText;
            QTextStream buffer( &fullText );
            for( const auto& filename:filenames )
            { buffer << QString( "file://%1" ).arg(filename) << endl; }
            mimeData->setText( fullText );
        }

        // fill url list
        {
            QList<QUrl> urlList;
            for( const auto& filename:filenames )
            { urlList.append( QUrl( QString( "file://%1" ).arg(filename) ) ); }
            mimeData->setUrls( urlList );
        }

        return mimeData;

    }

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
void FileRecordModel::_updateConfiguration()
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
    for( auto iter = properties.begin(); iter != properties.end(); ++iter )
    {
        // look for property name in list of columns
        if( std::none_of( columnTitles_.begin(), columnTitles_.end(), Base::Functor::SameFTor<QString>(FileRecord::PropertyId::get( iter.key() )) ) )
        { columnTitles_.append( FileRecord::PropertyId::get( iter.key() ) ); }

    }

}

//________________________________________________________
bool FileRecordModel::SortFTor::operator () ( FileRecord first, FileRecord second ) const
{

    if( order_ == Qt::DescendingOrder ) std::swap( first, second );

    switch( type_ )
    {

        case FileName: return first.file().localName().get().compare( second.file().localName(), Qt::CaseInsensitive ) < 0;
        case Path: return first.file().path() < second.file().path();
        case Time: return (first.time() != second.time() ) ? (first.time() < second.time()):first.file().localName() < second.file().localName();
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
