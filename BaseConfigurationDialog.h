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
  \file BaseConfigurationDialog.h
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef _BaseConfigurationDialog_h_
#define _BaseConfigurationDialog_h_

#include <QDialog>
#include <QPushButton>
#include <QStackedWidget>
#include <list>

#include "CustomListBox.h"
#include "Exception.h"
#include "XmlOptions.h"
#include "OptionWidget.h"

//! configuration list. Stores panel names and panels
class ConfigListItem: public QListWidgetItem, public Counter
{

  public:

  //! constructor
  ConfigListItem( QListWidget* parent, const QString& title, QWidget* page = 0 ):
    QListWidgetItem( parent ),
    Counter( "ConfigListItem" ),
    page_( page )
  { 
    Debug::Throw( "ConfigListItem::ConfigListItem.\n" );
    setText( title );
  }
    
  //! retrieve page
  QWidget& page( void )
  { return *page_; }

  private:

  //! associated group box
  QWidget *page_;
    
};

//! configuration dialog
class BaseConfigurationDialog: public QDialog, public Counter
{

  Q_OBJECT

  public:

  //! list of option widgets
  typedef std::list<OptionWidget*> OptionWidgetList;

  //! creator
  BaseConfigurationDialog( QWidget *parent );

  //! destructor
  virtual ~BaseConfigurationDialog()
  {
    Debug::Throw( "BaseConfigurationDialog::~BaseConfigurationDialog.\n" );
    return;
  }

  //! adds a new Item, returns associated Box
  virtual QWidget& addPage( const QString& title, const bool& expand = false );

  //! add option widget
  void addOptionWidget( OptionWidget* widget )
  { 
    Debug::Throw( "BaseConfigurationDialog::addOptionWidget.\n" );
    option_widgets_.push_back( widget ); 
  }

  //! clear option widgets
  virtual void clearOptionWidgets( void )
  { 
    Debug::Throw( "BaseConfigurationDialog::clearOptionWidgets.\n" );
    option_widgets_.clear(); 
  }

  //! flag bitset for the Base configuration
  enum ConfigFlags
  {
    //! base (font, icons and debug level
    BASE = 1<<0,

    //! toolbars and pixmap path
    TOOLBAR = 1<<1,

    //! lists
    LIST = 1<<2,
    
    //! textEdit emulations
    TEXTEDIT = 1<<3
    
  };

  //! adds configuration box for base options used in all appications
  void baseConfiguration( QWidget* parent = 0, const unsigned int& flag = BASE|TOOLBAR|LIST );

  //! toolbar configuration box
  void toolbarConfiguration( QWidget* parent = 0 );

  //! list configuration box
  void listConfiguration( QWidget* parent = 0 );

  //! TextEdit configuration box
  void textEditConfiguration( QWidget* parent = 0 );

  signals:

  //! apply button pressed
  void apply( void );
  
  //! ok button pressed
  void ok( void );
  
  //! canceled button pressed
  void cancel( void );
  
  //! emmited when configuration is changed
  void configurationChanged();

  protected slots:

  //! read configuration from options
  virtual void _read();

  //! read configuration from options
  virtual void _update();

  //! restore configuration
  virtual void _restore();

  //! save configuration from options
   virtual void _save();

  //! see if options have been modified. Emit signal if yes
  virtual void _checkModified( void )
  {
    Debug::Throw( "BaseConfigurationDialog::_checkModified.\n" );
    if( modified_options_ == XmlOptions::get() ) return;
    emit configurationChanged();
    modified_options_ = XmlOptions::get();
  }

  //! display item page
  virtual void _display( QListWidgetItem* current, QListWidgetItem* previous );

  protected:
  
  //! retrieve list
  virtual CustomListBox& _list( void )
  { return *list_; }

  //! retrieve stack
  virtual QStackedWidget& _stack( void )
  { return *stack_; }
    
  private:

  //! Configuration list
  CustomListBox* list_;

  //! Widget stack
  QStackedWidget* stack_;
  
  //! Apply button
  QPushButton *apply_button_;

  //! list of option widgets
  OptionWidgetList option_widgets_;

  //! pointer to modified options
  /*!
    it is needed to keep track of the changes
    so that the ConfigChanged signal is sent only
    when required
  */
  Options modified_options_;

  //! pointer to original options set
  /*!
    it is needed to keep track of the changes
    so that initial set is restored when pressing the cancel button
  */
  Options backup_options_;

};


#endif
