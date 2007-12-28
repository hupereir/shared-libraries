
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
  \file CounterModel.cpp
  \brief model for object counters
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CounterModel.h"
#include "Str.h"

using namespace std;

//_______________________________________________
const char* CounterModel::column_titles_[ CounterModel::n_columns ] =
{ 
  "name",
  "counts"
};

//__________________________________________________________________
QVariant CounterModel::data( const QModelIndex& index, int role ) const
{
  Debug::Throw( "CounterModel::data.\n" );
  
  // check index, role and column
  if( !index.isValid() ) return QVariant();
  
  // retrieve associated file info
  const Counter::Pair& counter( get(index) );
  
  // return text associated to file and column
  if( role == Qt::DisplayRole ) {
    
    switch( index.column() )
    {
      case NAME:
      return QString( counter.first.c_str() );
      
      case COUNT:
      return QString( Str().assign<int>(counter.second).c_str() );
      
      default:
      return QVariant();
    }
  }
 
  return QVariant();
  
}

//__________________________________________________________________
QVariant CounterModel::headerData(int section, Qt::Orientation orientation, int role) const
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
void CounterModel::sort( int column, Qt::SortOrder order )
{ 
  Debug::Throw() << "CounterModel::sort - column: " << column << " order: " << order << endl;

  // base class implementation
  ItemModel::sort( column, order );

  // prepare modifications
  emit layoutAboutToBeChanged();
  std::sort( _get().begin(), _get().end(), SortFTor( (ColumnType) column, order ) );
  emit layoutChanged();
      
}

//________________________________________________________
bool CounterModel::SortFTor::operator () ( Counter::Pair first, Counter::Pair second ) const
{
  
  if( order_ == Qt::AscendingOrder ) swap( first, second );
  
  switch( type_ )
  {

    case NAME: return first.first < second.first;
    case COUNT: return first.second < second.second;
    default:
    throw runtime_error( DESCRIPTION( "invalid column" ) );
    return true;
  }
 
}

