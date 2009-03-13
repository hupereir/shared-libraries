#ifndef _PixmapCacheModel_h_
#define _PixmapCacheModel_h_

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
  \file PixmapCacheModel.h
  \brief model for object PixmapCaches
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <string.h>


#include "Counter.h"
#include "Debug.h"
#include "PixmapEngine.h"
#include "ListModel.h"

//! qlistview for object PixmapCaches
class PixmapCacheModel: public ListModel<PixmapEngine::Pair>, public Counter
{

  public:

  //! number of columns
  enum { n_columns = 1 };

  //! column type enumeration
  enum ColumnType {
    ICON
  };

  //! constructor
  PixmapCacheModel( QObject* parent = 0 ):
    ListModel<PixmapEngine::Pair>( parent ),
    Counter( "PixmapCacheModel" )
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
  
  //! used to sort PixmapCaches
  class SortFTor: public ItemModel::SortFTor
  {
    
    public:
    
    //! constructor
    SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
      ItemModel::SortFTor( type, order )
      {}
      
    //! prediction
    bool operator() ( PixmapEngine::Pair, PixmapEngine::Pair ) const;
    
  };

  //! column titles
  static const QString column_titles_[ n_columns ];
   
};

#endif
