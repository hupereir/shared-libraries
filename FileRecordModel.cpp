
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

#include "IconEngine.h"
#include "FileRecordBaseProperties.h"
#include "FileRecordModel.h"

using namespace std;

//__________________________________________________________________
FileRecordModel::FileRecordModel( QObject* parent ):
  ListModel<FileRecord>( parent )
{
  Debug::Throw("FileRecordModel::FileRecordModel.\n" );

  column_titles_.push_back( "file name" );
  column_titles_.push_back( "path" );
  column_titles_.push_back( "last accessed" );

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
        what << local_name;
        if( version ) what << " (" << version+1 << ")";
        return what.str().c_str();
      }
      
      case PATH: return QString( record.file().path().c_str() );
      case TIME: return QString( TimeStamp( record.time() ).string().c_str() );
      
      default:
      if( index.column() < (int) column_titles_.size() && record.hasProperty( qPrintable( column_titles_[index.column()] ) ) )
      { return record.property( qPrintable( column_titles_[index.column()] ) ).c_str(); }
      else return QVariant();
   
    }
//   } else if( role == Qt::DecorationRole && index.column() == FILE && record.hasProperty( FileRecordProperties::ICON ) ) { 
//     
//     return IconEngine::get( record.property( FileRecordProperties::ICON ) );
//     
  } else if( role == Qt::ToolTipRole ) return QString( record.file().c_str() );
 
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
void FileRecordModel::_updateColumns( const ValueType& value )
{
  
  // loop over available properties
  const FileRecord::PropertyMap& properties( value.properties() );
  for( FileRecord::PropertyMap::const_iterator iter = properties.begin(); iter != properties.end(); iter++ )
  {
    // look for property name in list of columns
    if( find( column_titles_.begin(), column_titles_.end(), QString( iter->first.c_str() ) ) == column_titles_.end() )
    { column_titles_.push_back( iter->first.c_str() ); }
    
  }
  
}
  
//________________________________________________________
bool FileRecordModel::SortFTor::operator () ( FileRecord first, FileRecord second ) const
{
  
  if( order_ == Qt::AscendingOrder ) swap( first, second );
  
  switch( type_ )
  {

    case FILE: return first.file().localName() < second.file().localName();
    case TIME: return first.time() < second.time();
    default: 
    {
      string name( qPrintable( column_titles_[type_] ) );
      return first.property( name ) < second.property( name );
    }
  }
 
}
