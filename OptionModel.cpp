
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
  \file OptionModel.cpp
  \brief model for object IconCaches
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPalette>

#include "OptionModel.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
const char* OptionModel::column_titles_[ OptionModel::n_columns ] =
{ "name", "value" };

//_______________________________________________
Qt::ItemFlags OptionModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) return 0;
  else if( !get( index ).second.hasFlag( Option::RECORDABLE ) ) return 0;
  else if( !( isReadOnly() || rowCount( index ) ) && index.column() == VALUE ) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  else return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//__________________________________________________________________
QVariant OptionModel::data( const QModelIndex& index, int role ) const
{
  Debug::Throw( "OptionModel::data.\n" );
  
  // check index, role and column
  if( !index.isValid() ) return QVariant();
  
  const OptionPair& option( get( index ) );
  
  // return text associated to file and column
  if( role == Qt::DisplayRole )
  {
    switch( index.column() )
    {
      case NAME: return option.first.c_str();
      case VALUE: return ( option.second.hasFlag( Option::DEFAULT ) ? option.second.raw() + " (*)":option.second.raw() ).c_str();
      default: return QVariant();
    }
  }
  
  if( role == Qt::ToolTipRole && index.column() == NAME ) 
  { return option.second.comments().c_str(); }
  
  return QVariant();
  
}

//__________________________________________________________________
bool OptionModel::setData(const QModelIndex &index, const QVariant& value, int role )
{
  Debug::Throw( "OptionModel::setData.\n" );
  if( !(index.isValid() && index.column() == VALUE && role == Qt::EditRole ) ) return false;
  
  // retrieve option
  OptionPair option( get( index ) );
  
  if( !( value.toString().isNull() || value.toString() == option.second.raw().c_str() ) )
  {
    
    // remove old value
    remove( option );

    // add new one
    option.second.setRaw( qPrintable( value.toString() ) );
    add( option );
    
  }
  
  return true;
  
}

//__________________________________________________________________
QVariant OptionModel::headerData(int section, Qt::Orientation orientation, int role) const
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

//________________________________________________________
bool OptionModel::SortFTor::operator () ( OptionPair first, OptionPair second ) const
{
  
  if( order_ == Qt::AscendingOrder ) swap( first, second );
  
  switch( type_ )
  {
    
    case NAME: 
    {
      if( first.first != second.first ) return first.first < second.first;
      //else if( first.second.hasFlag( Option::DEFAULT ) ) return false;
      else return first.second.raw() < second.second.raw();
    }
    case VALUE: 
    {
      if( first.first != second.first ) return first.second.raw() < second.second.raw();
      //else if( first.second.hasFlag( Option::DEFAULT ) ) return false;
      else return false;
    }
    default:
    throw runtime_error( DESCRIPTION( "invalid column" ) );
    return true;
  }

}
