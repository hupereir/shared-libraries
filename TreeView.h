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

#include <QContextMenuEvent>
#include <QMenu>
#include <QTreeView>
#include <QAction>

#include <string>
#include <vector>

#include "Counter.h"

//! customized tree view
class TreeView: public QTreeView, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
  
  //! constructor
  TreeView( QWidget* parent );
     
  //! destructor
  virtual ~TreeView( void )
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
  
  //! number of visible columns
  virtual int visibleColumnCount( void ) const;
  
  //!@name column mask
  //@{
  
  //! option name
  virtual bool setMaskOptionName( const std::string& );
  
  //! option name 
  virtual bool hasMaskOptionName( void ) const
  { return !maskOptionName().empty(); }
  
  //! option name
  virtual const std::string& maskOptionName( void ) const
  { return mask_option_name_; }
  
  //! get mask
  virtual unsigned int mask( void ) const;

  //! set mask manually
  virtual void setMask( const unsigned int& );
   
  //@}
  
  //! set icon size manually and disable option
  void setIconSize( const QSize& size )
  {
    QTreeView::setIconSize( size );
    icon_size_from_options_ = false;
  }
  
  public slots:
  
  //! resize all visible columns to match contents
  virtual void resizeColumns()
  { resizeColumns( mask() ); }
   
  //! resize all visible columns to match contents
  virtual void resizeColumns( const unsigned int& mask );
 
  //!@name column mask
  //@{

  //! update column mask from option, if any
  virtual void updateMask( void );
  
  //! save column mask to option, if any
  virtual void saveMask( void );

  //@}

  protected:
  
  //! paint event
  virtual void paintEvent( QPaintEvent* );
    
  //! context menu
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  //! selected column color
  const QColor& _selectedColumnColor( void ) const
  { return selected_column_color_; }

  //! selected column color
  void _setSelectedColumnColor( const QColor& color )
  { selected_column_color_ = color; }
  
  //! install actions
  virtual void _installActions( void );
  
  protected slots:
   
  //! header menu
  virtual void _raiseHeaderMenu( const QPoint& );
  
  private slots:
    
  //! update alternate item color
  void _updateConfiguration( void );

  private:

  //!@name actions
  //@{
   
  //! select all document
  QAction* select_all_action_;
  
  //@}
  
  //! popup menu for right click
  QMenu *menu_;
  
  //! mask option name
  std::string mask_option_name_;
  
  //! true if icon size is to be set from options
  bool icon_size_from_options_;
  
  //! selected column background color
  QColor selected_column_color_;
  
};

#endif
