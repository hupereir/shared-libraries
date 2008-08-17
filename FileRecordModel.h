#ifndef _FileRecordModel_h_
#define _FileRecordModel_h_

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
  \file FileRecordModel.h
  \brief model for object counters
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <string.h>
#include <sstream>

#include "ListModel.h"
#include "FileRecord.h"
#include "Debug.h"
  
//! qlistview for object counters
class FileRecordModel: public ListModel<FileRecord>
{

  public:

  //! number of columns
  enum { n_columns = 3 };

  //! column type enumeration
  enum ColumnType {
    FILE, 
    PATH,
    TIME
  };

  //! constructor
  FileRecordModel( QObject* parent = 0 );
    
  //! show icons
  void setShowIcons( const bool& value )
  { show_icons_ = value; }
  
  //!@name methods reimplemented from base class
  //@{

  //! flags
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  
  // return data for a given index
  virtual QVariant data(const QModelIndex &, int ) const;
   
  //! header data
  virtual QVariant headerData(int, Qt::Orientation, int role = Qt::DisplayRole) const;
   
  //! number of columns for a given index
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

  //! column matching given name, if any
  virtual int findColumn( const QString& ) const;
  
  //@}
  
  //! set values (overloaded)
  void set( const List& values )
  {
    _updateColumns( values );
    ListModel<FileRecord>::set( values );
  }

  //! set values (overloaded)
  void update( const List& values )
  {
    _updateColumns( values );
    ListModel<FileRecord>::update( values );
  }
  
  protected:
  
  const bool& _showIcons( void ) const
  { return show_icons_; }
  
  //! sort
  virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder );
           
  //! add, without update
  virtual void _add( const ValueType& );

  private:
  
  //! used to sort Counters
  class SortFTor
  {
    
    public:
    
    //! constructor
    SortFTor( const int& type, Qt::SortOrder order, const std::vector<QString>& column_titles ):
      column_titles_( column_titles ),
      type_( type ),
      order_( order )
      {}
      
    //! prediction
    bool operator() ( FileRecord first, FileRecord second ) const;
    
    private:

    // column titles
    std::vector<QString> column_titles_;
    
    //! column
    int type_;
    
    //! order
    Qt::SortOrder order_;
    
  };

  //! update columns
  void _updateColumns( const List& values )
  {
    
    // update columns
    for( List::const_iterator iter = values.begin(); iter != values.end(); iter++ )
    { _updateColumns( *iter ); }
    
  }

  //! update columns
  void _updateColumns( const ValueType& value );

  //! true if icons are to be shown
  bool show_icons_;
  
  //! column titles
  std::vector<QString> column_titles_;
   
};

#endif
