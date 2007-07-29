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
    column_count_(0),
    column_(0),
    line_(0)
  {}
  
  //! set columns
  setColumnCount( const int& columns )
  { columns_ = columns; }
  
  //! set orientation
  setOrientation( const Qt::Orientation& orientation )
  { orientation_ = orientation; }
  
  //! add widget
  void addWidget( QWidget* widget )
  {
    Exception::assert( column_count_ > 0, DESCRIPTION( "invalid number of columns" ) );
    if( orientation_ == Qt::Vertical )  QGridLayout::addWidget( widget, column_, line_ );
    else QGridLayout::addWidget( widget, line_, column_ );
    column_ ++;
    if( column_ >= column_count_ ) 
    {
      column_ = 0;
      line_ ++;
    }
  }
    
  private:
  
  //! orientation
  Qt::Orientation orientation_;
  
  //! number of columns
  int column_count_;
  
  //! current column
  int column_;
  
  //! current line
  int line_;
  
};

#endif
