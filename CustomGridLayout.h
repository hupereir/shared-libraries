#ifndef CustomGridLayout_h
#define CustomGridLayout_h
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
  \file CustomGridLayout.h
  \brief Customized grid layout with fixed number of column + orientation
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QGridLayout>
#include "Counter.h"
#include "Exception.h"

class CustomGridLayout: public QGridLayout, public Counter
{
  
  public:
  
  //! constructor
  CustomGridLayout( void ):
    QGridLayout(),
    Counter( "CustomGridLayout" ),
    orientation_( Qt::Vertical ),
    max_count_(0),
    column_(0),
    row_(0)
  {}
  
  //! set columns
  void setMaxCount( const int& max_count )
  { max_count_ = max_count; }
  
  //! set orientation
  void setOrientation( const Qt::Orientation& orientation )
  { orientation_ = orientation; }
  
  //! add widget
  void addWidget( QWidget* widget, int row, int column, Qt::Alignment alignment = 0 )
  { addWidget( widget, row, column, 1, 1, alignment ); } 
  
  //! add wodget
  void addWidget ( QWidget * widget, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 )
  {
    Exception::check( rowSpan > 0, DESCRIPTION( "invalid rowSpan" ) );
    Exception::check( columnSpan > 0, DESCRIPTION( "invalid columnSpan" ) );
    QGridLayout::addWidget( widget, row, column, rowSpan, columnSpan, alignment ); 
    setLocation( row+rowSpan-1, column+columnSpan-1 );
    _increment();
  }    

  //! add widget
  void addWidget( QWidget* widget, Qt::Alignment alignment = 0 )
  {
    Exception::check( max_count_ > 0, DESCRIPTION( "invalid number of columns" ) );
    QGridLayout::addWidget( widget, row_, column_, alignment );
    _increment();
  }
    
  //! set current position in grid
  void setLocation( const int& row, const int& column )
  { row_ = row; column_ = column; }
  
  private:
  
  //! increment from last position
  void _increment()
  {
    
    if( orientation_ == Qt::Vertical )
    {
      column_ ++;
      if( column_ >= max_count_ ) 
      {
        column_ = 0;
        row_ ++;
      }
    } else {
      row_++;
      if( row_ >= max_count_ )
      {
        row_ = 0;
        column_++;
      }
    }
  }
  
  //! orientation
  Qt::Orientation orientation_;
  
  //! number of columns
  int max_count_;
  
  //! current column
  int column_;
  
  //! current row
  int row_;
  
};

#endif
