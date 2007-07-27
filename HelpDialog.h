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

#include "Counter.h"
#include "HelpItemList.h"

class CustomTextEdit;

namespace HELP
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
    
    //! true when items have been edited/modified
    const bool& modified( void ) const
    { return modified_; }
    
    signals:
    
    //! emmited when an item has been modified and should be saved
    void itemModified();
    
    protected:
    
    //! close event (overloaded)
    virtual void closeEvent( QCloseEvent* );
    
    private slots:
    
    //! display selected help text
    void _display(void);
    
    //! save modifications to current item
    void _save( void );
    
    //! toggle help edition
    void _toggleEdition( void );
    
    private:
    
    //! list of help items
    HelpItemList *list_;
    
    //! associated text editor
    CustomTextEdit *text_edit_;
    
    //! edition button
    QPushButton *edit_button_;
    
    //! true when help is being edited
    bool edited_;
    
    //! true when text was edited and modified
    bool modified_;
    
    //! pointer to current item
    HelpItemList::Item *current_item_;
    
  };
  
};

#endif
