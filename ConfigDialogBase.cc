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
  \file ConfigDialogBase.cc
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QApplication>
#include <QSplitter>
#include <QToolTip>
#include <QGroupBox>
#include <QLabel>

#include "ConfigDialogBase.h"
#include "Debug.h"
#include "OptionBrowsedLineEdit.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionIconBrowsedButton.h"
#include "OptionFontEdit.h"
#include "OptionLineEdit.h"
#include "OptionListBox.h"
#include "OptionSlider.h"
#include "OptionSpinBox.h"
#include "QtUtil.h"
#include "Str.h"

using namespace std;
using namespace Qt;

//_________________________________________________________
ConfigList::Item::Item( ConfigList* parent, const string& title, const bool& expand ):
  QListWidgetItem( parent, 0 ),
  visible_( false )
{  
  Debug::Throw( "ConfigList::Item::Item.\n" );
  
  // set text
  setText( title.c_str() );
  
  //! create vbox
  if( expand ) {
    main_ = box_ = new QWidget();
    QVBoxLayout *layout( new QVBoxLayout() );
    layout->setSpacing(2);
    layout->setMargin(5);
    main_->setLayout( layout );
  } else 
  {    
    main_ = new QWidget( &parent->target() );
    QVBoxLayout *layout( new QVBoxLayout() );
    layout->setSpacing(2);
    layout->setMargin(5);
    main_->setLayout( layout );

    // insert box into main
    box_ = new QWidget( main_ );
    QVBoxLayout* box_layout = new QVBoxLayout();
    box_layout->setSpacing(2);
    box_layout->setMargin(0);
    box_->setLayout( box_layout );

    layout->addWidget( box_ );
    layout->addStretch(1);

  }
  
  parent->target().addWidget( main_ );

}

//_________________________________________________________
ConfigDialogBase::ConfigDialogBase( QWidget* parent ):
  CustomDialog( parent ),
  modified_options_( XmlOptions::get() ),
  backup_options_( XmlOptions::get() )
{
  
  
  Debug::Throw( "ConfigDialogBase::ConfigDialog.\n" );
  setWindowTitle( "Configuration" );

  // splitter  
  QSplitter* splitter( new QSplitter( &mainWidget() ) );
  splitter->setChildrenCollapsible( false );
  mainWidget().layout()->addWidget( splitter );
  
  // create list
  list_ = new ConfigList( splitter );
  
  // create right display panel
  QStackedWidget* right_display( new QStackedWidget( splitter ) );
  list_->setTarget( right_display );

  connect( list_, SIGNAL( itemSelectionChanged() ), this, SLOT( _display() ) );
  
  QList<int> sizes;
  sizes.push_back( 150 );
  sizes.push_back( 400 );
  splitter->setSizes( sizes );
  
  // apply button
  apply_button_ = new QPushButton( "&Apply", this );
  QtUtil::fixSize( apply_button_ );
  buttonLayout().insertWidget( 0, apply_button_ );
  connect( &applyButton(), SIGNAL( clicked() ), this, SLOT( _updateConfiguration() ) );  
  apply_button_->setToolTip( "apply changes to options" );
  
  // ok button
  connect( &okButton(), SIGNAL( clicked() ), this, SLOT( _saveConfiguration() ) );  
  okButton().setAutoDefault( false );
  okButton().setToolTip( "apply changes to options and close window" );

  // cancel button
  connect( &cancelButton(), SIGNAL( clicked() ), this, SLOT( _restoreConfiguration() ) );
  cancelButton().setAutoDefault( false );
  cancelButton().setToolTip( "discard changes to options and close window" );
  
  // close window shortcut
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), this, SLOT( close() ) );
  resize( 550, 350 );

}

//__________________________________________________
QWidget& ConfigDialogBase::addBox( const string& name, const bool& expand )
{ 
  Debug::Throw( "ConfigDialogBase::addBox.\n" );
  ConfigList::Item *item(new ConfigList::Item( list_, name, expand ));
  if( list().QListWidget::count() == 1 ) list().setItemSelected( item, true );
  
  return item->box(); 
}

//__________________________________________________
QWidget& ConfigDialogBase::getBox( const string& name )
{
  Debug::Throw( "ConfigDialogBase::getBox.\n" );
  
  // loop over items
  for( int row = 0; row < list().QListWidget::count(); row ++ )
  {
    if( name == qPrintable( list().item(row)->text() ) )
    { return static_cast<ConfigList::Item*>(list().item(row))->box(); }
  }
  
  // if no match found, return new one
  return addBox( name );
  
}

//_______________________________________________
ConfigList::Item& ConfigDialogBase::listItem( const std::string& name )
{
  Debug::Throw( "ConfigDialogBase::listItem.\n" );
  
  QList<QListWidgetItem*>items = list().findItems( name.c_str(), Qt::MatchExactly );
  
  if( !items.empty() ) 
  {  
    ConfigList::Item* local = static_cast<ConfigList::Item*>(items.front());
    if( local ) return *local;
  }
  
  return *(new ConfigList::Item( &list(), name ));
}

//__________________________________________________
void ConfigDialogBase::baseConfiguration( QWidget* parent, const unsigned int& flag )
{
  
  Debug::Throw( "ConfigDialogBase::baseConfiguration.\n" );
  
  if( !parent ) parent = &addBox( "base" );
  
  // base
  if( flag & BASE )
  { 
    
    QGroupBox *box = new QGroupBox( "base", parent );
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->setSpacing(2);
    box->setLayout( layout );
    parent->layout()->addWidget( box );
    
    QWidget* hbox = new QWidget( box );
    layout->addWidget( hbox );
    
    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->setMargin(0);
    h_layout->setSpacing(5);
    hbox->setLayout( h_layout );
    
    // base font
    QWidget* grid = new QWidget( hbox );
    h_layout->addWidget( grid );
    
    QGridLayout* grid_layout = new QGridLayout();
    grid_layout->setMargin(0);
    grid_layout->setSpacing(2);
    grid->setLayout( grid_layout );
    grid_layout->addWidget( new QLabel( "default font: ", grid ), 0, 0 );
    OptionFontEdit *edit = new OptionFontEdit( grid, "FONT_NAME" );
    edit->setToolTip( "default font name for all widgets" );
    grid_layout->addWidget( edit, 0, 1 );
    addOptionWidget( edit );
    
    // fixed font
    grid_layout->addWidget( new QLabel( "fixed font: ", grid ), 1, 0 );
    edit = new OptionFontEdit( grid, "FIXED_FONT_NAME" );
    edit->setToolTip( "default font name (fixed) for text widgets" );
    grid_layout->addWidget( edit, 1, 1 );
    QtUtil::fixSize( edit, QtUtil::HEIGHT );
    addOptionWidget( edit );

    // default icon path
    if( !XmlOptions::get().find( "DEFAULT_ICON_PATH" ) ) XmlOptions::get().add( Option( "DEFAULT_ICON_PATH", "" ) );

    grid_layout->addWidget( new QLabel( "default icon path: ", grid ), 2, 0 );
    OptionBrowsedLineEdit* icon_path_edit = new OptionBrowsedLineEdit( grid, "DEFAULT_ICON_PATH" );
    grid_layout->addWidget( icon_path_edit, 2, 1 );
    addOptionWidget( icon_path_edit );
    
    // debug level
    grid_layout->addWidget( new QLabel( "debug level: ", grid ), 3, 0 );
    OptionSpinBox* spinbox = new OptionSpinBox( grid, "DEBUG_LEVEL" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 5 );
    spinbox->setToolTip( "debug verbosity level" );
    grid_layout->addWidget( spinbox, 3, 1 );
    addOptionWidget( spinbox );
          
    // icon pixmap
    QWidget* iconbox = new QWidget( hbox );
    h_layout->addWidget( iconbox );
    
    QVBoxLayout* v_layout = new QVBoxLayout();
    iconbox->setLayout( v_layout );
    
    OptionIconBrowsedButton* editor = new OptionIconBrowsedButton( iconbox, "ICON_PIXMAP" );
    addOptionWidget( editor );
    editor->setToolTip( "application icon" );
    v_layout->addWidget( editor );
    v_layout->addWidget( new QWidget( iconbox ) );
    
  }
    
  // toolbars
  if( flag & TOOLBAR ) { toolbarConfiguration( parent ); }
  
  // list
  if( flag & LIST ) { listConfiguration( parent ); }
  
  // tabs
  if( flag & TABS ) { tabConfiguration( parent ); }
  
  string warning( 
    "Note: the application may have to be restarted so that "
    "all changes \nare taken into account." );
  parent->layout()->addWidget( new QLabel( warning.c_str(), parent ) );
    
}

//__________________________________________________
void ConfigDialogBase::toolbarConfiguration( QWidget* parent )
{
  
  Debug::Throw( "ConfigDialogBase::toolbarConfiguration.\n" );
    
  // make sure parent is valid
  if( !parent ) parent = &addBox( "base" );
  QGroupBox* box = new QGroupBox( "toolbars", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(2);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
      
  layout->addWidget( new QLabel("pixmap pathname: ", box ) );
  OptionListBox *listbox = new OptionListBox( box, "PIXMAP_PATH" );
  listbox->setBrowsable( true );
  addOptionWidget( listbox );
  listbox->setToolTip( "pathname to load toolbar pixmaps" );
  layout->addWidget( listbox );
  
  // menu big pixmaps
  layout->addWidget( new QLabel("",box) );
  OptionCheckBox* checkbox = new OptionCheckBox( "big pixmaps in tool bars", box, "USE_BIG_PIXMAP" );
  addOptionWidget( checkbox );
  checkbox->setToolTip( "use big pixmaps in toolbars" );
  layout->addWidget( checkbox );
  
  // menu text label
  checkbox = new OptionCheckBox( "button text in tool bars", box, "USE_TEXT_LABEL" );
  addOptionWidget( checkbox );
  checkbox->setToolTip( "use text under tool buttons icon" );
  layout->addWidget( checkbox );
  
}

//__________________________________________________
void ConfigDialogBase::listConfiguration( QWidget* parent )
{
  
  Debug::Throw( "ConfigDialogBase::listConfiguration.\n" );

  // make sure parent is valid
  if( !parent ) parent = &addBox( "lists" );
  
  QGroupBox* box = new QGroupBox( "lists", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(2);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
    
  QLabel* label = new QLabel( "list item background color: ", box );
  QtUtil::fixSize( label );
  layout->addWidget( label );
  
  OptionColorDisplay* color = new OptionColorDisplay( box, "ITEM_COLOR" );
  addOptionWidget( color );
  color->setToolTip( "item background color in lists" );
  layout->addWidget( color );
  
}

//__________________________________________________
void ConfigDialogBase::tabConfiguration( QWidget* parent )
{
  
  Debug::Throw( "ConfigDialogBase::tabConfiguration.\n" );

  // make sure parent is valid
  if( !parent ) parent = &addBox( "Tabs" );
  
  // tab emulation
  QGroupBox* box = new QGroupBox( "tab emulation", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(2);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
     
  QWidget* grid = new QWidget( box );
  QGridLayout* grid_layout( new QGridLayout() );
  grid_layout->setMargin(0);
  grid_layout->setSpacing(2);
  grid->setLayout( grid_layout );
  layout->addWidget( grid );
  
  grid_layout->addWidget(new QLabel( "tab size: ", grid ), 0, 0 );
  OptionSpinBox* spinbox = new OptionSpinBox( grid, "TAB_SIZE" );
  spinbox->setMinimum( 2 );
  spinbox->setMaximum( 20 );
  spinbox->setToolTip( "tab size (in unit of space characters)." );
  grid_layout->addWidget( spinbox, 0, 1 );
  addOptionWidget( spinbox );

  OptionCheckBox* checkbox = new OptionCheckBox( "emulate tabs", box, "TAB_EMULATION" );
  checkbox->setToolTip( "Turn on/off tab emulation using space characters" );
  grid_layout->addWidget( checkbox, 0, 1, 2, 1 );
  addOptionWidget( checkbox );

}

//__________________________________________________
void ConfigDialogBase::_display(void)
{
  Debug::Throw( "ConfigDialogBase::_display.\n" );
  
  // retrieve first selected item if any
  QList<ConfigList::Item*> items( list().selectedItems<ConfigList::Item>() );
  if( items.empty() ) return;
  ConfigList::Item* item( items.front() );
    
  // show current item
  list().target().setCurrentWidget( &item->main() );
  
}

//__________________________________________________
void ConfigDialogBase::_readConfiguration( void )
{
  Debug::Throw( "ConfigDialogBase::_readConfiguration.\n" );
  for( OptionWidgetList::iterator iter = option_widgets_.begin(); iter != option_widgets_.end(); iter++ )
  (*iter)->read(); 
}

//__________________________________________________
void ConfigDialogBase::_updateConfiguration( void )
{
   Debug::Throw( "ConfigDialogBase::_updateConfiguration.\n" );
  
  for( OptionWidgetList::iterator iter = option_widgets_.begin(); iter != option_widgets_.end(); iter++ )
  (*iter)->write(); 
  
  // check if options have changed since last call
  _checkModified();
  
}

//__________________________________________________
void ConfigDialogBase::_restoreConfiguration( void )
{ 
  Debug::Throw( "ConfigDialogBase::_restoreConfiguration.\n" );
  if( XmlOptions::get() == backup_options_ ) return;
  XmlOptions::get() = backup_options_;
  modified_options_ = backup_options_;
  _readConfiguration();
  emit configurationChanged();
}

//__________________________________________________
void ConfigDialogBase::_saveConfiguration( void )
{
  
  Debug::Throw( "ConfigDialogBase::_saveConfiguration.\n" );
  _updateConfiguration();
  XmlOptions::write();

}
