#ifndef HelpItemList_h
#define HelpItemList_h

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
   \file    HelpItemList.h
   \brief   list to display help item entries
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QDrag>
#include <QString>

#include "CustomListView.h"
#include "HelpItem.h"

namespace BASE
{
  
  //! Help item list
  class HelpItemList:public CustomListView
  {
    
    public:
    //! constructor
    HelpItemList( QWidget *parent );
      
    //! used to tag Keyword drags
    static const QString DRAG;
  
    //! enable drag
    void setDragEnabled( bool value )
    { drag_enabled_ = value; }
      
    //! local item (overloaded) for counter dialog
    class Item: public QTreeWidgetItem
    {
        
      public:
      
      //! constructor
      Item( HelpItemList* parent, const HelpItem& item ):
        QTreeWidgetItem( parent ),
        item_( item )
      { setText( 0, item.label().c_str() ); }
      
      //! return associated item
      const HelpItem& item( void ) const
      { return item_; }
      
      //! return associated item
      HelpItem& item( void )
      { return item_; }
      
      private:
      
      //! associated help text
      HelpItem item_;
      
    };
    
    protected:

    //! mouse press events [needed to start drag]
    virtual void mousePressEvent( QMouseEvent *event );
    
    //! mouse move events [needed to start drag]
    virtual void mouseMoveEvent( QMouseEvent *event );
    
    //! drag enter event [overloaded]
    virtual void dragEnterEvent( QDragEnterEvent* event );
    
    //! drag move event [overloaded]
    virtual void dragMoveEvent( QDragMoveEvent* event );
    
    //! drag leave event [overloaded]
    virtual void dragLeaveEvent( QDragLeaveEvent* )
    {  _resetDrag(); }
    
    //! drop event [overload]
    virtual void dropEvent( QDropEvent* event );
    
    private:
    
    //!@name drag and drop methods
    //@{
    
    //! start drag sequence
    bool _startDrag( QMouseEvent *event );
    
    //! reset drag sequence
    void _resetDrag( void );
    
    //! return true if QMimeSource is an accepted TextDrag
    bool _acceptDrag( QDropEvent *event ) const;
    
    //! process drop action (for accepted drags)
    bool _processDrop( QDropEvent *event );
    
    //@}
    
    //!@name drag and drop
    //@{
    
    //! drag enabled
    bool drag_enabled_;
    
    //! drop target item
    Item *drop_item_;
    
    //! drop item 'initial' selection state
    bool drop_item_selected_;
    
    //! store possible mouse drag start position
    QPoint drag_start_;
    
    //@}
  };
  
};

#endif
