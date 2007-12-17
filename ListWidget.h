#ifndef ListWidget_h
#define ListWidget_h

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
  \file ListWidget.h
  \brief qlistbox with even objects highlighted with different color
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QListWidget>
#include <string>

#include "Counter.h"
#include "Debug.h"

//! qlistbox with even objects highlighted with different color
class ListWidget: public QListWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:

  //! empty constructor
  ListWidget( QWidget* parent );

  //! retrieve all items
  QList<QListWidgetItem*> items( void )
  {
    QList<QListWidgetItem*> out;
    for( int row = 0; row < QListWidget::count(); row++ )
    {
      QListWidgetItem* local_item( item( row ) );
      if( local_item ) out.push_back( local_item );
    }

    return out;
  }

  //! current item
  template <typename T>
  T* currentItem( void )
  { return dynamic_cast<T*>( QListWidget::currentItem() ); }

  //! retrieve all items
  template <typename T>
  QList< T* > items( void )
  {

    QList<T*> out;
    for( int row = 0; row < QListWidget::count(); row++ )
    {
      T* local_item( dynamic_cast<T*>( item( row ) ) );
      if( local_item ) out.push_back( local_item );
    }
    return out;

  }

  //! selected items
  template <typename T>
  QList< T* > selectedItems( void )
  {
    QList<QListWidgetItem*> found( QListWidget::selectedItems() );
    QList<T*> out;
    for( QList<QListWidgetItem*>::const_iterator iter = found.begin(); iter!=found.end(); iter++ )
    {
      T* local_item( dynamic_cast<T*>( *iter ) );
      if( local_item ) out.push_back( local_item );
    }
    return out;
  }

  //! find item matching name.
  /*! Creates a new one if none found */
  virtual QListWidgetItem& findItem( const std::string& name );

  private slots:

  //! update alternate item background color
  void _updateConfiguration( void );
    
};

#endif
