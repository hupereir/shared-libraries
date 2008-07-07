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
  \file TabbedDialog.h
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef _TabbedDialog_h_
#define _TabbedDialog_h_

#include <QCloseEvent>
#include <QDialog>
#include <QLayout>
#include <QStackedWidget>
#include <list>

#include "TreeWidget.h"
#include "XmlOptions.h"
#include "OptionWidget.h"

//! configuration list. Stores panel names and panels
class ConfigListItem: public QTreeWidgetItem, public Counter
{

  public:

  //! constructor
  ConfigListItem( QTreeWidget* parent, const QString& title, QWidget* page = 0 ):
    QTreeWidgetItem( parent ),
    Counter( "ConfigListItem" ),
    page_( page )
  { 
    Debug::Throw( "ConfigListItem::ConfigListItem.\n" );
    setText( 0, title );
  }
    
  //! retrieve page
  QWidget& page( void )
  { return *page_; }

  private:

  //! associated group box
  QWidget *page_;
    
};

//! tabbed dialog
/*! a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class TabbedDialog: public QDialog, public Counter
{

  Q_OBJECT

  public:

  //! creator
  TabbedDialog( QWidget *parent );

  //! destructor
  virtual ~TabbedDialog()
  {
    Debug::Throw( "TabbedDialog::~TabbedDialog.\n" );
    return;
  }
  
  //! adds a new Item, returns associated Box
  virtual QWidget& addPage( const QString& title, const bool& expand = false );

  //! restore window size
  virtual QSize minimumSizeHint( void ) const;

  //! restore window size
  virtual QSize sizeHint( void ) const;
  
  protected slots: 
  
  //! display item page
  virtual void _display( QTreeWidgetItem* current, QTreeWidgetItem* previous );

  protected:

  //! close
  virtual void closeEvent( QCloseEvent* );
  
  //! size option name
  void _setSizeOptionName( const std::string& name )
  { size_option_name_ = name; }
  
  //! size option name
  const std::string _sizeOptionName( void ) const
  { return size_option_name_; }

  //! retrieve list
  virtual TreeWidget& _list( void )
  { return *list_; }

  //! retrieve stack
  virtual QStackedWidget& _stack( void )
  { return *stack_; }

  //! button layout
  QHBoxLayout& _buttonLayout( void ) const
  { return *button_layout_; }
  
  private slots:
  
  //! save window size to options
  void _saveWindowSize( void );
  
  private:

  //! window size option name
  std::string size_option_name_;
  
  //! Configuration list
  TreeWidget* list_;

  //! Widget stack
  QStackedWidget* stack_;
  
  //! button layout (needed to add extra buttons)
  QHBoxLayout *button_layout_;

};


#endif
