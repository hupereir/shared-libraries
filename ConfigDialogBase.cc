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
#include <QToolTip>
#include <QGroupBox>
#include <QLayout>
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
ConfigDialogBase::ConfigDialogBase( QWidget* parent ):
  QDialog( parent ),
  modified_options_( XmlOptions::get() ),
  backup_options_( XmlOptions::get() )
{
  
  
  Debug::Throw( "ConfigDialogBase::ConfigDialog.\n" );
  setWindowTitle( "Configuration" );

  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setSpacing(5);
  layout->setMargin(5);
  setLayout( layout );
  
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setMargin(0);
  h_layout->setSpacing(5);
  layout->addLayout( h_layout );
  
  h_layout->addWidget( list_ = new CustomListBox( this ), 0 );
  h_layout->addWidget( stack_ = new QStackedWidget( this ), 1 );
  
  _list().setMaximumWidth(128);
  _list().setMovement(QListView::Static);

  connect( 
    list_, SIGNAL( currentItemChanged( QListWidgetItem*, QListWidgetItem* ) ), 
    SLOT( _display(QListWidgetItem*, QListWidgetItem*) ) );
  
  // button layout
  QHBoxLayout* button_layout( new QHBoxLayout() );
  button_layout->setMargin(5);
  button_layout->setSpacing(10);
  layout->addLayout( button_layout, 0 );
  
  // apply button
  QPushButton* button;
  button_layout->addWidget( button = new QPushButton( "&Apply", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _updateConfiguration() ) );  
  connect( button, SIGNAL( clicked() ), SIGNAL( apply() ) );  
  button->setToolTip( "apply changes to options" );
  
  // ok button
  button_layout->addWidget( button = new QPushButton( "&Ok", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _saveConfiguration() ) );  
  connect( button, SIGNAL( clicked() ), SIGNAL( ok() ) );  
  connect( button, SIGNAL( clicked() ), SLOT( accept() ) );  
  button->setToolTip( "apply changes to options and close window" );
  button->setAutoDefault( false );
  
  // cancel button
  button_layout->addWidget( button = new QPushButton( "&Cancel", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _restoreConfiguration() ) );
  connect( button, SIGNAL( clicked() ), SIGNAL( cancel() ) );  
  connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
  button->setToolTip( "discard changes to options and close window" );
  button->setAutoDefault( false );
  
  // close window shortcut
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );
  //resize( 550, 350 );

}

//_________________________________________________________
QWidget& ConfigDialogBase::addPage( const QString& title, const bool& expand )
{  
  Debug::Throw( "ConfigList::Item::Item.\n" );
  
  QWidget* main( new QWidget() );
  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setSpacing( 2 );
  layout->setMargin( 5 );
  main->setLayout( layout );
  
  // create new item and add to stack
  new ConfigListItem( &_list(), title, main );
  _stack().addWidget( main );

  // make sure first item is selected
  _list().setCurrentRow(0);
  _stack().setCurrentIndex(0);
  
  // in expanded mode, the main widget is returned directly
  if( expand ) return *main;
  
  // in non-expanded mode (the default)
  // a widget is created inside main, and a stretch is added at the bottom
  // the created widget is return
  QWidget* contents( new QWidget( main ) );
  contents->setLayout( new QVBoxLayout() );
  contents->layout()->setSpacing(2);
  contents->layout()->setMargin(0);
  
  layout->addWidget( contents );
  layout->addStretch();
  return *contents;
  
}

//__________________________________________________
void ConfigDialogBase::baseConfiguration( QWidget* parent, const unsigned int& flag )
{
  
  Debug::Throw( "ConfigDialogBase::baseConfiguration.\n" );
  
  if( !parent ) parent = &addPage( "base" );
  
  // base
  if( flag & BASE )
  { 
    
    QGroupBox *box = new QGroupBox( "base", parent );
    parent->layout()->addWidget( box );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->setSpacing(2);
    box->setLayout( layout );
        
    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->setMargin(0);
    h_layout->setSpacing(5);
    layout->addLayout( h_layout );
    
    QGridLayout* grid_layout = new QGridLayout();
    grid_layout->setMargin(0);
    grid_layout->setSpacing(2);
    h_layout->addLayout( grid_layout );

    // base font    
    grid_layout->addWidget( new QLabel( "default font: ", box ), 0, 0 );
    OptionFontEdit *edit = new OptionFontEdit( box, "FONT_NAME" );
    edit->setToolTip( "default font name for all widgets" );
    grid_layout->addWidget( edit, 0, 1 );
    addOptionWidget( edit );
    
    // fixed font
    grid_layout->addWidget( new QLabel( "fixed font: ", box ), 1, 0 );
    edit = new OptionFontEdit( box, "FIXED_FONT_NAME" );
    edit->setToolTip( "default font name (fixed) for text widgets" );
    grid_layout->addWidget( edit, 1, 1 );
    addOptionWidget( edit );

    // default icon path
    grid_layout->addWidget( new QLabel( "default icon path: ", box ), 2, 0 );
    OptionBrowsedLineEdit* icon_path_edit = new OptionBrowsedLineEdit( box, "DEFAULT_ICON_PATH" );
    grid_layout->addWidget( icon_path_edit, 2, 1 );
    addOptionWidget( icon_path_edit );
    
    // debug level
    grid_layout->addWidget( new QLabel( "debug level: ", box ), 3, 0 );
    OptionSpinBox* spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 5 );
    spinbox->setToolTip( "debug verbosity level" );
    grid_layout->addWidget( spinbox, 3, 1 );
    addOptionWidget( spinbox );
          
    // icon pixmap
    QVBoxLayout* v_layout( new QVBoxLayout() );
    v_layout->setMargin(0);
    v_layout->setSpacing(5);
    h_layout->addLayout( v_layout );
        
    OptionIconBrowsedButton* editor = new OptionIconBrowsedButton( box, "ICON_PIXMAP" );
    editor->setToolTip( "application icon" );
    addOptionWidget( editor );
    v_layout->addWidget( editor );
    v_layout->addWidget( new QLabel( "icon", box ), 0, Qt::AlignHCenter );
    v_layout->addStretch(1);
    
  }
    
  // toolbars
  if( flag & TOOLBAR ) { toolbarConfiguration( parent ); }
  
  // list
  if( flag & LIST ) { listConfiguration( parent ); }
  
  // tabs
  if( flag & TABS ) { tabConfiguration( parent ); }
  
  QLabel *label = new QLabel( 
    "Note: the application may have to be restarted so that "
    "all changes \nare taken into account.", parent );
  label->setMargin(10);
  parent->layout()->addWidget( label );
    
}

//__________________________________________________
void ConfigDialogBase::toolbarConfiguration( QWidget* parent )
{
  
  Debug::Throw( "ConfigDialogBase::toolbarConfiguration.\n" );
    
  // make sure parent is valid
  if( !parent ) parent = &addPage( "base" );
  QGroupBox* box = new QGroupBox( "toolbars", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(2);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
      
  layout->addWidget( new QLabel("pixmap pathname: ", box ) );
  OptionListBox *listbox = new OptionListBox( box, "PIXMAP_PATH" );
  listbox->setBrowsable( true );
  listbox->setMode( QFileDialog::Directory );
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
  if( !parent ) parent = &addPage( "lists" );
  
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
  if( !parent ) parent = &addPage( "Tabs" );
  
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
void ConfigDialogBase::_display( QListWidgetItem* current, QListWidgetItem* previous )
{
  Debug::Throw( "ConfigDialogBase::_display.\n" );
  
  if( !current ) current = previous;
  ConfigListItem* item( dynamic_cast<ConfigListItem*>(current) );
  Exception::checkPointer( item, DESCRIPTION( "invalid cast" ) );
  _stack().setCurrentWidget(&item->page());  
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
