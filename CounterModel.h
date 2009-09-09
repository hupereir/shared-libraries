#ifndef _CounterModel_h_
#define _CounterModel_h_

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
  \file CounterModel.h
  \brief model for object counters
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "ListModel.h"
#include "Counter.h"
#include "Debug.h"

//! counter pair
class CounterPair: public std::pair<QString, int>
{
  public:

  //! constructor
  CounterPair( void )
  {}

  //! constructor
  CounterPair( const std::pair<QString, int>& pair ):
    std::pair<QString, int>( pair )
    {}

  //! less than operator
  bool operator < ( const CounterPair& pair ) const
  {
    return first < pair.first;
  }

  //! equal to operator
  bool operator == (const CounterPair& pair ) const
  { return first == pair.first; }

};

//! qlistview for object counters
class CounterModel: public ListModel<CounterPair>
{

  public:

  //! number of columns
  enum { n_columns = 2 };

  //! column type enumeration
  enum ColumnType {
    NAME,
    COUNT
  };


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
    bool operator() ( CounterPair, CounterPair ) const;

  };

  //! column titles
  static const QString column_titles_[ n_columns ];

};

#endif
