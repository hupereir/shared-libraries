#ifndef _IconCacheModel_h_
#define _IconCacheModel_h_

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
  \file IconCacheModel.h
  \brief model for object IconCaches
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <string.h>
#include <sstream>

#include "Counter.h"
#include "Debug.h"
#include "IconEngine.h"
#include "ListModel.h"

//! qlistview for object IconCaches
class IconCacheModel: public ListModel<IconEngine::Pair>, public Counter
{

  public:

  //! number of columns
  enum { n_columns = 1 };

  //! column type enumeration
  enum ColumnType {
    ICON
  };

  //! constructor
  IconCacheModel( QObject* parent = 0 ):
    ListModel<IconEngine::Pair>( parent ),
    Counter( "IconCacheModel" )
  {}
  
  //!@name methods reimplemented from base class
  //@{
  
  // return data for a given index
  virtual QVariant data(const QModelIndex &index, int role) const;
   
  //! header data
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
   
  //! number of columns for a given index
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
  { return n_columns; }

  //@}
  
  protected:
  
  //! sort
  virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder );
            
  private:
  
  //! used to sort IconCaches
  class SortFTor: public ItemModel::SortFTor
  {
    
    public:
    
    //! constructor
    SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
      ItemModel::SortFTor( type, order )
      {}
      
    //! prediction
    bool operator() ( IconEngine::Pair, IconEngine::Pair ) const;
    
  };

  //! column titles
  static const char* column_titles_[ n_columns ];
   
};

#endif
