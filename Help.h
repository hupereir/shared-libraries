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

#ifndef Help_h
#define Help_h

/*!
   \file    Help.h
   \brief   reference manual system
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <list>
#include <QDomElement>
#include <QDomDocument>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <string>

#include "Counter.h"
#include "CustomListBox.h"
#include "Debug.h"
#include "File.h"

//! help object for eLogbook
class HelpItem : public Counter
{

  public:
    
  //! constructor
  HelpItem( const std::string& label = "", const std::string& text = "" ):
    Counter( "HelpItem" ),
    label_( label ),
    text_( text )
  {}
  
  //! constructor from Dom
  HelpItem( const QDomElement& dom );
  
  //! write to dom elelement
  QDomElement DomElement( QDomDocument& parent ) const;
  
  //! item label
  void setLabel( const std::string& label ) 
  { label_ = label; }
  
  //! label
  const std::string& label( void ) const
  { return label_; }
  
  //! item text
  void setText( const std::string& text ) 
  { text_ = text; }
   
  //! item text
  const std::string& text( void ) const
  { return text_; }
     
  private:
  
  //! help label
  std::string label_;
  
  //! help text
  std::string text_;
  
};

//! Help item list
class HelpList:public CustomListBox
{

  public:
  //! constructor
  HelpList( QWidget *parent ):
    CustomListBox( parent )
    {}
  
  //! local item (overloaded) for counter dialog
  class Item: public QListWidgetItem
  {
    
    public:
    
    //! constructor
    Item( HelpList* parent, const HelpItem& item ):
      QListWidgetItem( parent ),
      item_( item )
    { setText(item.label().c_str() ); }
    
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

};

//! Help display dialog
class HelpDialog:public QMainWindow, public Counter
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
  HelpList& list( void )
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
  HelpList *list_;
  
  //! associated text editor
  QTextEdit *text_edit_;
  
  //! edition button
  QPushButton *edit_button_;
  
  //! true when help is being edited
  bool edited_;
  
  //! true when text was edited and modified
  bool modified_;
  
  //! pointer to current item
  HelpList::Item *current_item_;
  
};

//! help manager class
class HelpManager:public QObject, public Counter 
{
  
  Q_OBJECT
  
  public:
  
  //! manager singleton
  static HelpManager& get( void )
  {
    static HelpManager singleton;
    return singleton;
  }
  
  //! destructor
  virtual ~HelpManager( void )
  {
    Debug::Throw( "HelpManager::~HelpManager.\n" );
    if( modified() ) save();
  }
  
  //! input file
  void setFile( const File& file )
  { file_ = file; }

  //! install help. Put items in the menu, create associated objects 
  bool install( const char *text[] );
 
  //! install help, from file
  bool install( const File& file );

  //! true when items have been edited/modified
  const bool& modified( void ) const
  { return dialog_->modified(); }
    
  //! set dialog caption
  void setWindowTitle( const std::string& value )
  { dialog_->setWindowTitle( value.c_str() ); }
  
  public slots:
  
  //! opens help dialog
  void display( void );
 
  //! dump help string in a TextEditor
  void dumpHelpString( void );
  
  //! save help to file
  void save( void );
  
  private:
  
  //! constructor
  HelpManager( void );

  //! associated dialog
  HelpDialog* dialog_;
  
  //! help filename
  File file_;
  
};

#endif
