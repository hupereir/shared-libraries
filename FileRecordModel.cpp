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

/*!
  \file FileRecordModel.cpp
  \brief model for object records
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <algorithm>
#include <assert.h>
#include <QIcon>
#include <QApplication>

#include "CustomPixmap.h"
#include "FileRecordBaseProperties.h"
#include "FileRecordModel.h"
#include "XmlOptions.h"

using namespace std;

//__________________________________________________________________
FileRecordModel::IconCache FileRecordModel::icons_;

//__________________________________________________________________
FileRecordModel::FileRecordModel( QObject* parent ):
  ListModel<FileRecord>( parent ),
  Counter( "FileRecordModel" ),
  show_icons_( true ),
  icon_property_id_( FileRecord::PropertyId::get( FileRecordProperties::ICON ) )
{
  Debug::Throw("FileRecordModel::FileRecordModel.\n" );

  column_titles_.push_back( "" );
  column_titles_.push_back( "file" );
  column_titles_.push_back( "path" );
  column_titles_.push_back( "last accessed" );

  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  
}
  
//__________________________________________________________________
int FileRecordModel::columnCount(const QModelIndex &parent) const
{ return column_titles_.size(); }

//__________________________________________________________________
int FileRecordModel::findColumn( const QString& value ) const
{
  for( unsigned int index = 0; index < column_titles_.size(); index++ )
  { if( column_titles_[index] == value ) return index;  }
  assert(0);
}

//__________________________________________________________________
Qt::ItemFlags FileRecordModel::flags(const QModelIndex &index) const
{
  
  // default flags
  Qt::ItemFlags flags;
  if( index.isValid() )
  {
  
    // check associated record validity
    const FileRecord& record( get(index) );
    if( record.isValid() ) flags |=  Qt::ItemIsEnabled |  Qt::ItemIsSelectable;
  
  }
  
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
        string local_name( record.file().localName() );
        
        // loop over previous rows to find a match and increment version number
        unsigned int version( 0 );
        for( int row = 0; row < index.row(); row++ )
        { 
          if( get( FileRecordModel::index( row, FILE ) ).file().localName() == local_name ) version++; 
        }
        
        // form output string.
        ostringstream what;
        if( local_name.empty() ) what << "untitled";
        else what << local_name;
        if( version ) what << " (" << version+1 << ")";
        return what.str().c_str();
      }
      
      case PATH: return QString( record.file().path().c_str() );
      case TIME: return QString( TimeStamp( record.time() ).string().c_str() );
      case ICON: return QVariant();
      default:
      if( index.column() < (int) column_titles_.size() && record.hasProperty( qPrintable( column_titles_[index.column()] ) ) )
      { return record.property( qPrintable( column_titles_[index.column()] ) ).c_str(); }
      else return QVariant();
   
    }
    
  } else if( _showIcons() && role == Qt::DecorationRole && index.column() == ICON && record.hasProperty( icon_property_id_ ) ) {
    
    // icon
    return _icon( record.property( icon_property_id_ ) );
    
  } else if( role == Qt::ToolTipRole ) return QString( record.file().c_str() );
 
  // alignment
  else if( role == Qt::TextAlignmentRole && index.column() == ICON ) return Qt::AlignCenter;
  
  return QVariant();
  
}

//__________________________________________________________________
QVariant FileRecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{

  if( 
    orientation == Qt::Horizontal && 
    role == Qt::DisplayRole && 
    section >= 0 && 
    section < (int) column_titles_.size() )
  { return column_titles_[section]; }
  
  // return empty
  return QVariant(); 

}
  
//____________________________________________________________
void FileRecordModel::_sort( int column, Qt::SortOrder order )
{ std::sort( _get().begin(), _get().end(), SortFTor( column, order, column_titles_ ) ); }

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
  icons_.clear();
}

//____________________________________________________________
void FileRecordModel::_updateColumns( const ValueType& value )
{
  
  Debug::Throw( "FileRecordModel::_updateColumns.\n" );
  
  // loop over available properties
  const FileRecord::PropertyMap& properties( value.properties() );
  for( FileRecord::PropertyMap::const_iterator iter = properties.begin(); iter != properties.end(); iter++ )
  {
    // look for property name in list of columns
    if( find( column_titles_.begin(), column_titles_.end(), QString( FileRecord::PropertyId::get( iter->first ).c_str() ) ) == column_titles_.end() )
    { column_titles_.push_back( FileRecord::PropertyId::get( iter->first ).c_str() ); }
    
  }
  
}
  
//________________________________________________________
bool FileRecordModel::SortFTor::operator () ( FileRecord first, FileRecord second ) const
{
  
  if( order_ == Qt::AscendingOrder ) swap( first, second );
  
  switch( type_ )
  {

    case FILE: return first.file().localName() < second.file().localName();
    case TIME: return (first.time() != second.time() ) ? (first.time() < second.time()):first.file().localName() < second.file().localName();
    default: 
    {
      string name( qPrintable( column_titles_[type_] ) );
      string first_property( first.property( name ) );
      string second_property( second.property( name ) );
      return ( first_property != second_property ) ? first_property < second_property :  first.file().localName() < second.file().localName();
    }
    
  }
 
}

//________________________________________________________
QIcon FileRecordModel::_icon( const std::string& name )
{
  
  Debug::Throw( "FileRecordModel::_icon.\n" );
  
  IconCache::const_iterator iter( icons_.find( name ) );
  if( iter != icons_.end() ) return iter->second;

  // pixmap size
  unsigned int pixmap_size = XmlOptions::get().get<unsigned int>( "LIST_ICON_SIZE" );
  QSize size( pixmap_size, pixmap_size );
  QSize scale(size*0.9);
  
  CustomPixmap base( CustomPixmap().find( name )  );
  
  QIcon icon;
  if( !base.isNull() )
  { icon = CustomPixmap().empty( size ).merge( base.scaled( scale, Qt::KeepAspectRatio, Qt::SmoothTransformation ), CustomPixmap::CENTER ); }
  
  // insert in map
  icons_.insert( make_pair( name, icon ) );
  
  return icon;
   
}
