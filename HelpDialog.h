#ifndef HelpDialog_h
#define HelpDialog_h

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
   \file    HelpDialog.h
   \brief   reference manual display dialog 
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QDialog>
#include <QPushButton>
#include <QStackedLayout>

#include "Counter.h"
#include "HelpItemList.h"

class CustomTextEdit;

namespace BASE
{
  
  //! reference manual display dialog
  class HelpDialog:public QDialog, public Counter
  {
    
    Q_OBJECT
      
    public:
  
    //! constructor
    HelpDialog( QWidget *parent = 0 );

    //! clear items
    void clear( void )
    { list().clear(); }

    //! adds Help item to the dialog
    void addItem( const HelpItem& item );
    
    //! retrieves list
    HelpItemList& list( void )
    { return *list_; }
    
    //! enable/disable edition
    void setEditEnabled( const bool& value )
    { edit_button_->setEnabled( value ); }
    
    protected:
    
    //! close event (overloaded)
    virtual void closeEvent( QCloseEvent* );
    
    private slots:
    
    //! display selected help text
    void _display( QTreeWidgetItem*, QTreeWidgetItem* );
    
    //! save modifications to current item
    void _save( bool forced = false );
    
    //! toggle help edition
    void _toggleEdition( void );

    //! new item
    void _newItem( void );
    
    //! delete current item
    void _deleteItem( void );
    
    private:
    
    //! list of help items
    HelpItemList *list_;
    
    //! stack layout to switch between editors
    QStackedLayout* stack_layout_;
    
    //! html edition frame
    QWidget* html_frame_;
    
    //! plain edition frame
    QWidget* plain_frame_;
    
    //! read-only text editor
    CustomTextEdit *html_editor_;
    
    //! plain text editor
    CustomTextEdit *plain_editor_; 
    
    //! edition button
    QPushButton *edit_button_;
    
    //! true when help is being edited
    bool edited_;
    
    //! pointer to current item
    HelpItemList::Item *current_item_;
    
  };
  
};

#endif
