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
#include <QLayout>
#include <QStackedWidget>
#include <list>

#include "TreeWidget.h"
#include "XmlOptions.h"
#include "OptionWidget.h"
#include "OptionWidgetList.h"

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

//! configuration dialog
class BaseConfigurationDialog: public QDialog, public OptionWidgetList, public Counter
{

  Q_OBJECT

  public:

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

  //! restore window size
  virtual QSize minimumSizeHint( void ) const;

  //! restore window size
  virtual QSize sizeHint( void ) const;
  
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

  //! show pixmap path dialog
  virtual void _editPixmapPathList( void );
  
  //! read configuration from options
  virtual void _read()
  { 
    Debug::Throw( "BaseConfigurationDialog::_read.\n" );
    OptionWidgetList::read(); 
  }

  //! update configuration
  virtual void _update()
  {
    Debug::Throw( "BaseConfigurationDialog::_update.\n" );
    OptionWidgetList::write(); 
    _checkModified();
  }

  //! restore configuration
  virtual void _restore();
  
  //! restore default options
  virtual void _restoreDefaults( void ) = 0;

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
  virtual void _display( QTreeWidgetItem* current, QTreeWidgetItem* previous );

  //! save window size
  virtual void _saveWindowSize( void );
  
  protected:
  
  //! retrieve list
  virtual TreeWidget& _list( void )
  { return *list_; }

  //! retrieve stack
  virtual QStackedWidget& _stack( void )
  { return *stack_; }

  //! button layout
  QHBoxLayout& _buttonLayout( void ) const
  { return *button_layout_; }
    
  private:

  //! Configuration list
  TreeWidget* list_;

  //! Widget stack
  QStackedWidget* stack_;
  
  //! button layout (needed to add extra buttons)
  QHBoxLayout *button_layout_;

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
