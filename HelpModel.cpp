
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
\file HelpModel.h
\brief model for help items
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <assert.h>
#include <QMimeData>
#include <QStringList>

#include "HelpModel.h"

using namespace std;
using namespace BASE;

//______________________________________________________________
const QString HelpModel::DRAG = "base/helpmodel/drag";

//__________________________________________________________________
Qt::ItemFlags HelpModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) return (editionEnabled() ? Qt::ItemIsDropEnabled: Qt::ItemFlags(0) );
  Qt::ItemFlags flags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
  if(editionEnabled()) flags |= (Qt::ItemIsDragEnabled | Qt::ItemIsEditable);
  return flags;
}

//__________________________________________________________________
QVariant HelpModel::data( const QModelIndex& index, int role ) const
{
    
  // check index, role and column
  if( !index.isValid() ) return QVariant();
  if( role != Qt::DisplayRole ) return QVariant();
  if( index.column() != LABEL ) return QVariant();
  return QString( get(index).label().c_str() ); 
  
}

//__________________________________________________________________
bool HelpModel::setData(const QModelIndex &index, const QVariant& value, int role )
{ return false; }


//______________________________________________________________________
QStringList HelpModel::mimeTypes( void ) const
{
  QStringList types;
  types << DRAG;
  return types;
}

//______________________________________________________________________
QMimeData* HelpModel::mimeData(const QModelIndexList &indexes) const
{

  assert( indexes.size() == 1 );
  
  // create mime data
  QMimeData *mime = new QMimeData();
  
  // set DRAG type
  for( QModelIndexList::const_iterator iter = indexes.begin(); iter != indexes.end(); iter++ )
  { if( iter->isValid() ) mime->setData( DRAG, get( *iter ).label().c_str() ); }
  
  return mime;

}


//__________________________________________________________________
bool HelpModel::dropMimeData(const QMimeData* data , Qt::DropAction action, int row, int column, const QModelIndex& parent)
{ return false; }
