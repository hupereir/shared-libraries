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
#include "HelpModel.h"

class CustomTextEdit;
class TreeView;

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
    { model_.clear(); }

    //! set items
    void setItems( const HelpItem::List& items );
    
    //! adds Help item to the dialog
    void addItem( const HelpItem& item );
    
    //! enable/disable edition
    void setEditEnabled( const bool& value )
    { edit_button_->setEnabled( value ); }
    
    public slots:
    
    //! close
    virtual void close( void )
    {
      
      Debug::Throw( "HelpDialog::close.\n" );
      if( model_.editionEnabled() ) _askForSave();
      QDialog::close();
      
    }
    
    protected:
    
    //! close event (overloaded)
    virtual void closeEvent( QCloseEvent* );
    
    private slots:
    
    //! display selected help text
    void _display( const QModelIndex&, const QModelIndex& );
        
    //! save modifications to current item
    void _updateItemFromEditor( QModelIndex index = QModelIndex(), bool forced = false );

    //! reload items from list and update Help manager
    void _updateHelpManager( void );
    
    //! toggle help edition
    void _toggleEdition( void );

    //! mode item
    void _moveItem( int row );
    
    //! rename item
    void _renameItem( QModelIndex index, QString value );
    
    //! new item
    void _newItem( void );
    
    //! delete current item
    void _deleteItem( void );
    
    private:
    
    //! if help manager is modified, ask for save
    void _askForSave( void );
    
    //! model
    HelpModel model_;
    
    //! list of help items
    TreeView *list_;
    
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
        
  };
  
};

#endif
