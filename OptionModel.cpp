
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

#include "BaseIcons.h"
#include "IconEngine.h"
#include "OptionModel.h"
#include "XmlOptions.h"



//_______________________________________________
const QString OptionModel::column_titles_[ OptionModel::n_columns ] =
{ "Name", "", "Current Value", "Default Value", "Flags" };

//_______________________________________________
Qt::ItemFlags OptionModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) return 0;
  else if( !get( index ).second.hasFlag( Option::RECORDABLE ) ) return Qt::ItemIsSelectable;
  else if( !( isReadOnly() || rowCount( index ) ) && index.column() == VALUE ) return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
  else return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//__________________________________________________________________
QVariant OptionModel::data( const QModelIndex& index, int role ) const
{

  // check index, role and column
  if( !index.isValid() ) return QVariant();

  const OptionPair& option( get( index ) );

  // return text associated to file and column
  if( role == Qt::DisplayRole )
  {
    switch( index.column() )
    {
      case NAME: return option.first;
      case VALUE: return option.second.raw();
      case DEFAULT_VALUE: return option.second.defaultValue();
      case FLAGS: return option.second.flags();
      default: return QVariant();
    }
  }

  if( role == Qt::DecorationRole && index.column() == CURRENT )
  { return option.second.isCurrent() ? IconEngine::get( ICONS::DIALOG_ACCEPT ):QVariant(); }

  if( role == Qt::ToolTipRole && index.column() == NAME )
  { return option.second.comments(); }

  return QVariant();

}

//__________________________________________________________________
bool OptionModel::setData(const QModelIndex &index, const QVariant& value, int role )
{

  if( !(index.isValid() && index.column() == VALUE && role == Qt::EditRole ) ) return false;

  // retrieve option
  OptionPair option( get( index ) );

  if( !( value.toString().isNull() || value.toString() == option.second.raw() ) )
  {

    // remove old value
    remove( option );

    // add new one
    option.second.setRaw( value.toString() );
    add( option );

    if( XmlOptions::get().isSpecialOption( option.first ) ) emit specialOptionModified( option );
    else emit optionModified( option );

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
  { return column_titles_[section]; }

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
      else return first.second.raw() < second.second.raw();
    }
    case VALUE:
    {
      if( first.first != second.first ) return first.second.raw() < second.second.raw();
      else return false;
    }
    default: return true;
  }

}
