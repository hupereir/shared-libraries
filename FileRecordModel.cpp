
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
  \brief model for object file_records
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "FileRecordModel.h"
#include "Str.h"

using namespace std;

//_______________________________________________
const char* FileRecordModel::column_titles_[ FileRecordModel::n_columns ] =
{ 
  "filename",
  "full name",
  "last accessed"
};

//__________________________________________________________________
QVariant FileRecordModel::data( const QModelIndex& index, int role ) const
{
  Debug::Throw( "FileRecordModel::data.\n" );
  
  // check index, role and column
  if( !index.isValid() ) return QVariant();
  
  // retrieve associated file info
  const FileRecord& file_record( get(index) );
  
  // return text associated to file and column
  if( role == Qt::DisplayRole ) {
          
    switch( index.column() )
    {
      
      case LOCAL_FILENAME:
      {
        // store local nmae
        string local_name( file_record.file().localName() );
        
        // loop over previous rows to find a match and increment version number
        unsigned int version( 0 );
        for( int row = 0; row < index.row(); row++ )
        { 
          if( get( FileRecordModel::index( row, LOCAL_FILENAME ) ).file().localName() == local_name ) version++; 
        }
        
        // form output string.
        ostringstream what;
        what << local_name;
        if( version ) what << " (" << version+1 << ")";
        return QString( what.str().c_str() );
      }
      
      case FULL_FILENAME: return QString( file_record.file().c_str() );
      case TIME: return QString( TimeStamp( file_record.time() ).string().c_str() );
      
      default:
      {
        // check if provided column already corresponds to an information field
        // if yes, return relevant information.
        // if no, loop over informations, find one that is not already covered by
        // existing columns. If found, add a matching column and returns the information.
        // if not, returns QVariant.
      }
      return QVariant();
    }
  } else if( role == Qt::ToolTipRole ) return QString( file_record.file().c_str() );
 
  return QVariant();
  
}

//__________________________________________________________________
QVariant FileRecordModel::headerData(int section, Qt::Orientation orientation, int role) const
{

  if( 
    orientation == Qt::Horizontal && 
    role == Qt::DisplayRole && 
    section >= 0 && 
    section < n_columns )
  { return QString( column_titles_[section] ); }
  
  // return empty
  return QVariant(); 

}

//____________________________________________________________
void FileRecordModel::_sort( int column, Qt::SortOrder order )
{ 

  Debug::Throw() << "FileRecordModel::sort - column: " << column << " order: " << order << endl;
  std::sort( _get().begin(), _get().end(), SortFTor( (ColumnType) column, order ) );
      
}

//________________________________________________________
bool FileRecordModel::SortFTor::operator () ( FileRecord first, FileRecord second ) const
{
  
  if( order_ == Qt::AscendingOrder ) swap( first, second );
  
  switch( type_ )
  {

    case LOCAL_FILENAME: return first.file().localName() < second.file().localName();
    case TIME: return first.time() < second.time();
    default:
    throw runtime_error( DESCRIPTION( "invalid column" ) );
    return true;
  }
 
}
