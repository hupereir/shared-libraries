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

#include <assert.h>

#include <QContextMenuEvent>
#include <QMenu>
#include <QTreeView>
#include <QAction>

#include <string>
#include <vector>

#include "Counter.h"
#include "TextSelection.h"

class BaseFindDialog;

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

  //! enable list finding
  void setFindEnabled( bool value );
  
  //! TextSelection object from this selection, or clipboard
  TextSelection selection( void ) const;
  
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
  virtual bool setOptionName( const QString& );
  
  //! option name 
  virtual bool hasOptionName( void ) const
  { return !maskOptionName().isEmpty(); }
  
  //! option name
  virtual const QString& maskOptionName( void ) const
  { return mask_option_name_; }
  
  //! sort column option name
  virtual const QString& sortColumnOptionName( void ) const
  { return sort_column_option_name_; }
  
  //! sort order option name
  virtual const QString& sortOrderOptionName( void ) const
  { return sort_order_option_name_; }
  
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
  
  //!@name actions
  //@{
  
  //! select all
  QAction& selectAllAction( void ) const 
  { return *select_all_action_; }
  
  //! find from dialog
  QAction& findAction( void ) const
  { return *find_action_; }

  //! find selection again
  QAction& findSelectionAction( void ) const
  { return *find_selection_action_; }

  //! find again
  QAction& findAgainAction( void ) const
  { return *find_again_action_; }

  //@}
  
  signals:
  
  //! emmitted when selection could not be found
  void noMatchFound( void );
  
  //! emmitted when selection could be found
  void matchFound( void );
  
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
  
  //! sort order
  virtual void updateSortOrder( void );
  
  //! sort order
  virtual void saveSortOrder( void );
     
  //! find next occurence of TextSelection
  virtual void find( TextSelection selection );
  
  //! find current selection forward
  virtual void findSelectionForward( void );

  //! find current selection backward
  virtual void findSelectionBackward( void );
  
  //! find last search forward
  virtual void findAgainForward( void );

  //! find last search forward
  virtual void findAgainBackward( void );
  
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
  
  //! find dialog
  virtual BaseFindDialog& _findDialog( void )
  {
    assert( find_dialog_ );
    return *find_dialog_;
  }
  
  //! find dialog
  virtual void _createBaseFindDialog( void );
  
  //! find selection in forward direction
  virtual bool _findForward( const TextSelection& selection, bool rewind );

  //! find selection in backward direction
  virtual bool _findBackward( const TextSelection& selection, bool rewind );

  protected slots:
   
  //! header menu
  virtual void _raiseHeaderMenu( const QPoint& );

  //! find text from dialog
  virtual void _findFromDialog( void );

  private slots:
    
  //! update alternate item color
  void _updateConfiguration( void );

  private:
  
  //! install actions
  virtual void _installActions( void );   

  //! return first index
  QModelIndex _firstIndex() const;
  
  //! return last index
  QModelIndex _lastIndex() const;
  
  //! return next index of current
  QModelIndex _indexAfter( const QModelIndex& index ) const;

  //! return previous index
  QModelIndex _indexBefore( const QModelIndex& index ) const;
  
  //! find dialog
  BaseFindDialog* find_dialog_;

  //!@name actions
  //@{
   
  //! select all document
  QAction* select_all_action_;
    
  //! find from dialog
  QAction* find_action_;

  //! find selection again
  QAction* find_selection_action_;

  //! find selection backward
  QAction* find_selection_backward_action_;

  //! find again
  QAction* find_again_action_;
  
  //! find again backward
  QAction* find_again_backward_action_;

  //@}
  
  //! popup menu for right click
  QMenu *menu_;
  
  //! mask option name
  QString mask_option_name_;
  
  //! sort column option name
  QString sort_column_option_name_;
  
  //! sort order option name
  QString sort_order_option_name_;
  
  //! true if icon size is to be set from options
  bool icon_size_from_options_;
  
  //! selected column background color
  QColor selected_column_color_;
  
};

#endif
