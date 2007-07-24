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
  \file ConfigDialogBase.h
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef _ConfigDialogBase_h_
#define _ConfigDialogBase_h_

#include <QPushButton>
#include <QStackedWidget>
#include <list>

#include "CustomDialog.h"
#include "CustomListBox.h"
#include "Exception.h"
#include "XmlOptions.h"
#include "OptionWidget.h"

//! configuration list. Stores panel names and panels
class ConfigList: public CustomListBox
{

  public:

  //! constructor
  ConfigList( QWidget *parent ):
    CustomListBox( parent ),
    target_( 0 )
  {};

  //! display target widget
  void setTarget( QStackedWidget* target )
  { target_ = target; }

  //! display target widget
  QStackedWidget& target( void )
  { return *target_; }

  //! list box item
  class Item: public QListWidgetItem
  {
    public:

    //! constructor
    Item( ConfigList* parent, const std::string& title, const bool& expand = false  );

    //! retrieve group box
    QWidget& main( void )
    { return *main_; }

    //! retrieve group box
    QWidget& box( void )
    { return *box_; }

    //! rename
    void rename( const std::string& name )
    { setText( name.c_str() ); }

    //! visibility
    const bool& visible( void ) const
    { return visible_; }

    private:

    //! main vbox
    QWidget *main_;

    //! associated group box
    QWidget *box_;

    //! visibility status
    bool visible_;
    
  };

  private:

  //! target display
  QStackedWidget *target_;
  
};

//! configuration dialog
class ConfigDialogBase: public CustomDialog
{

  //! Qt meta object macro
  Q_OBJECT

  public:

  //! list of option widgets
  typedef std::list<OptionWidget*> OptionWidgetList;

  //! creator
  ConfigDialogBase( QWidget *parent );

  //! destructor
  virtual ~ConfigDialogBase()
  {
    return;
  }

  //! add option widget
  virtual void addOptionWidget( OptionWidget* widget )
  { option_widgets_.push_back( widget ); }

  //! clear option widgets
  virtual void clearOptionWidgets( void )
  { option_widgets_.clear(); }

  //! apply button
  QPushButton& applyButton( void )
  {
    Exception::checkPointer( apply_button_, DESCRIPTION( "apply_button_ uninitialized" ) );
    return *apply_button_;
  }

  //! adds a new Item, returns associated Box
  QWidget& addBox( const std::string& name, const bool& expand = false );

  //! retrieve box of item matching a given name
  /*! create a new one if no match found */
  QWidget& getBox( const std::string& name );

  //! retrieve list item of a given name
  /*! create a new one if not found */
  virtual ConfigList::Item& listItem( const std::string& name );

  //! retrieve list
  ConfigList& list( void )
  { return *list_; }

  //! flag bitset for the Base configuration
  enum ConfigFlags
  {
    //! base (font, icons and debug level
    BASE = 1<<0,

    //! toolbars and pixmap path
    TOOLBAR = 1<<1,

    //! lists
    LIST = 1<<2,
    
    //! tab emulations
    TABS = 1<<3
    
  };

  //! adds configuration box for base options used in all appications
  void baseConfiguration( QWidget* parent = 0, const unsigned int& flag = BASE|TOOLBAR|LIST );

  //! toolbar configuration box
  void toolbarConfiguration( QWidget* parent = 0 );

  //! list configuration box
  void listConfiguration( QWidget* parent = 0 );

  //! Tab emulation configuration box
  void tabConfiguration( QWidget* parent = 0 );
  
  signals:

  //! emmited when configuration is changed
  void configurationChanged();

  protected slots:

  //! read configuration from options
  virtual void _readConfiguration();

  //! read configuration from options
  virtual void _updateConfiguration();

  //! restore configuration
  virtual void _restoreConfiguration();

  //! save configuration from options
   virtual void _saveConfiguration();

  //! see if options have been modified. Emit signal if yes
  virtual void _checkModified()
  {
    if( modified_options_ == XmlOptions::get() ) return;
    emit configurationChanged();
    modified_options_ = XmlOptions::get();
  }

  private slots:

  //! display box selected from list
  void _display();

  private:

  //! Configuration list
  ConfigList* list_;

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
