#ifndef _TreeView_h_
#define _TreeView_h_

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
  \file TreeView.h
  \brief customized Tree View
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QMenu>
#include <QTreeView>

#include <string>
#include <vector>

#include "Counter.h"
#include "Debug.h"

//! customized tree view
class TreeView: public QTreeView, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
  
  //! constructor
  TreeView( QWidget* parent );
     
  //! destructor
  ~TreeView( void )
  {}

  //!@name contextual menu
  //@{
  
  //! returns true if menu has been initialized
  virtual bool hasMenu( void ) const
  { return menu_; }
  
  //! retrieve popup menu
  virtual QMenu& menu( void );
   
  //@} 
  
  //! true if given index is visible
  bool isVisible( const QModelIndex& index ) const;
  
  //! return column visibility bitset. Is 1 for shown columns, 0 for hidden
  virtual unsigned int mask( void );
  
  //! show/hide columns according to mask bitset. 1 is for shown columns, 0 for hidden
  virtual void setMask( const unsigned int& mask );
  
  public slots:
    
  //! update alternate item color
  virtual void updateItemColor( void );
  
  protected:
  
  //! customized drawing to handle colored entries
  virtual void drawRow( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
  
  protected slots:

  //! columns changed
  void columnCountChanged ( int oldCount, int newCount )
  {
    Debug::Throw(0) << "TreeView::columnCountChanged - columns: " << newCount << std::endl;
    columns_ = newCount; 
  }
  
  //! popup contextual menu
  virtual void _raiseMenu( const QPoint& );

  private:

  //! popup menu for right click
  QMenu *menu_;
  
  //! columns
  int columns_;
  
  //! column mask
  /*! gets reinitialized anytime GetMask is called */
  unsigned int mask_;    
  
};

#endif
