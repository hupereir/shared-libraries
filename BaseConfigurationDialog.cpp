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
  \file BaseConfigurationDialog.cc
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QApplication>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>

#include "BaseConfigurationDialog.h"
#include "CustomGridLayout.h"
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
BaseConfigurationDialog::BaseConfigurationDialog( QWidget* parent ):
  QDialog( parent ),
  Counter( "BaseConfigurationDialog" ),
  modified_options_( XmlOptions::get() ),
  backup_options_( XmlOptions::get() )
{
  
  
  Debug::Throw( "BaseConfigurationDialog::BaseConfigurationDialog.\n" );
  setWindowTitle( "Configuration" );

  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setSpacing(10);
  layout->setMargin(10);
  setLayout( layout );
  
  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setMargin(0);
  h_layout->setSpacing(5);
  layout->addLayout( h_layout );
  
  h_layout->addWidget( list_ = new CustomListBox( this ), 0 );
  h_layout->addWidget( stack_ = new QStackedWidget( this ), 1 );
  
  _list().setMaximumWidth(150);
  _list().setMovement(QListView::Static);

  connect( 
    list_, SIGNAL( currentItemChanged( QListWidgetItem*, QListWidgetItem* ) ), 
    SLOT( _display(QListWidgetItem*, QListWidgetItem*) ) );
  
  // button layout
  QHBoxLayout* button_layout( new QHBoxLayout() );
  button_layout->setMargin(0);
  button_layout->setSpacing(5);
  layout->addLayout( button_layout, 0 );
  
  // apply button
  QPushButton* button;
  button_layout->addWidget( button = new QPushButton( "&Apply", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _update() ) );  
  connect( button, SIGNAL( clicked() ), SIGNAL( apply() ) );  
  button->setToolTip( "apply changes to options" );
  
  // ok button
  button_layout->addWidget( button = new QPushButton( "&Ok", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _save() ) );  
  connect( button, SIGNAL( clicked() ), SIGNAL( ok() ) );  
  connect( button, SIGNAL( clicked() ), SLOT( accept() ) );  
  button->setToolTip( "apply changes to options and close window" );
  button->setAutoDefault( false );
  
  // cancel button
  button_layout->addWidget( button = new QPushButton( "&Cancel", this ) );
  connect( button, SIGNAL( clicked() ), SLOT( _restore() ) );
  connect( button, SIGNAL( clicked() ), SIGNAL( cancel() ) );  
  connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
  button->setToolTip( "discard changes to options and close window" );
  button->setAutoDefault( false );
  
  // close window shortcut
  connect( new QShortcut( CTRL+Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );

}

//_________________________________________________________
QWidget& BaseConfigurationDialog::addPage( const QString& title, const bool& expand )
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
void BaseConfigurationDialog::baseConfiguration( QWidget* parent, const unsigned int& flag )
{
  
  Debug::Throw( "BaseConfigurationDialog::baseConfiguration.\n" );
  
  if( !parent ) parent = &addPage( "Base" );
  
  // base
  if( flag & BASE )
  { 
    
    QGroupBox *box = new QGroupBox( "Base", parent );
    parent->layout()->addWidget( box );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->setSpacing(5);
    box->setLayout( layout );
        
    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->setMargin(0);
    h_layout->setSpacing(5);
    layout->addLayout( h_layout );
    
    CustomGridLayout* grid_layout = new CustomGridLayout();
    grid_layout->setMargin(0);
    grid_layout->setSpacing(5);
    grid_layout->setMaxCount(2);
    h_layout->addLayout( grid_layout );

    // base font    
    grid_layout->addWidget( new QLabel( "Default font: ", box ) );
    OptionFontEdit *edit = new OptionFontEdit( box, "FONT_NAME" );
    edit->setToolTip( "Default font name for all widgets" );
    grid_layout->addWidget( edit );
    addOptionWidget( edit );
    
    // fixed font
    grid_layout->addWidget( new QLabel( "Fixed font: ", box ) );
    edit = new OptionFontEdit( box, "FIXED_FONT_NAME" );
    edit->setToolTip( "Default font name (fixed) for text widgets" );
    grid_layout->addWidget( edit );
    addOptionWidget( edit );

    // default icon path
    grid_layout->addWidget( new QLabel( "Default icon path: ", box ) );
    OptionBrowsedLineEdit* icon_path_edit = new OptionBrowsedLineEdit( box, "DEFAULT_ICON_PATH" );
    grid_layout->addWidget( icon_path_edit );
    addOptionWidget( icon_path_edit );
    
    // debug level
    grid_layout->addWidget( new QLabel( "Debug level: ", box ) );
    OptionSpinBox* spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 5 );
    spinbox->setToolTip( "Debug verbosity level" );
    grid_layout->addWidget( spinbox );
    addOptionWidget( spinbox );
          
    // icon pixmap
    QVBoxLayout* v_layout( new QVBoxLayout() );
    v_layout->setMargin(0);
    v_layout->setSpacing(5);
    h_layout->addLayout( v_layout );
        
    OptionIconBrowsedButton* editor = new OptionIconBrowsedButton( box, "ICON_PIXMAP" );
    editor->setToolTip( "Application icon" );
    addOptionWidget( editor );
    v_layout->addWidget( editor );
    v_layout->addWidget( new QLabel( "Icon", box ), 0, Qt::AlignHCenter );
    v_layout->addStretch(1);
    
  }
    
  // toolbars
  if( flag & TOOLBAR ) { toolbarConfiguration( parent ); }
  
  // list
  if( flag & LIST ) { listConfiguration( parent ); }
  
  // tabs
  if( flag & TEXTEDIT ) { textEditConfiguration( parent ); }
  
  QLabel *label = new QLabel( 
    "Note: the application may have to be restarted so that "
    "all changes \nare taken into account.", parent );
  label->setMargin(10);
  parent->layout()->addWidget( label );
    
}

//__________________________________________________
void BaseConfigurationDialog::toolbarConfiguration( QWidget* parent )
{
  
  Debug::Throw( "BaseConfigurationDialog::toolbarConfiguration.\n" );
    
  // make sure parent is valid
  if( !parent ) parent = &addPage( "Base" );
  QGroupBox* box = new QGroupBox( "Toolbars", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
      
  layout->addWidget( new QLabel("Pixmap pathname: ", box ) );
  OptionListBox *listbox = new OptionListBox( box, "PIXMAP_PATH" );
  listbox->setBrowsable( true );
  listbox->setFileMode( QFileDialog::Directory );
  addOptionWidget( listbox );
  listbox->setToolTip( "Pathname to load toolbar pixmaps" );
  layout->addWidget( listbox );
  
  // menu big pixmaps
  layout->addWidget( new QLabel("",box) );
  OptionCheckBox* checkbox = new OptionCheckBox( "Big pixmaps in toolbars", box, "USE_BIG_PIXMAP" );
  addOptionWidget( checkbox );
  checkbox->setToolTip( "Use big pixmaps in toolbars" );
  layout->addWidget( checkbox );
  
  // menu text label
  checkbox = new OptionCheckBox( "Button text in toolbars", box, "USE_TEXT_LABEL" );
  addOptionWidget( checkbox );
  checkbox->setToolTip( "Use text under tool buttons icon" );
  layout->addWidget( checkbox );

  // menu text label
  checkbox = new OptionCheckBox( "lock toolbars position", box, "LOCK_TOOLBARS" );
  addOptionWidget( checkbox );
  checkbox->setToolTip( "Lock toolbars position" );
  layout->addWidget( checkbox );
  
}

//__________________________________________________
void BaseConfigurationDialog::listConfiguration( QWidget* parent )
{
  
  Debug::Throw( "BaseConfigurationDialog::listConfiguration.\n" );

  // make sure parent is valid
  if( !parent ) parent = &addPage( "Lists" );
  
  QGroupBox* box = new QGroupBox( "Lists", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
    
  QLabel* label = new QLabel( "List item background color: ", box );
  QtUtil::fixSize( label );
  layout->addWidget( label );
  
  OptionColorDisplay* color = new OptionColorDisplay( box, "ITEM_COLOR" );
  addOptionWidget( color );
  color->setToolTip( "Item background color in lists" );
  layout->addWidget( color );
  
}

//__________________________________________________
void BaseConfigurationDialog::textEditConfiguration( QWidget* parent )
{
  
  Debug::Throw( "BaseConfigurationDialog::textEditConfiguration.\n" );

  // make sure parent is valid
  if( !parent ) parent = &addPage( "Text edition" );
  
  // tab emulation
  QGroupBox* box = new QGroupBox( "Tab emulation", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
     
  CustomGridLayout* grid_layout( new CustomGridLayout() );
  grid_layout->setMargin(0);
  grid_layout->setSpacing(5);
  grid_layout->setMaxCount(2);
  layout->addLayout( grid_layout );
  
  grid_layout->addWidget(new QLabel( "Tab size: ", box ) );

  OptionCheckBox* checkbox = new OptionCheckBox( "Emulate tabs", box, "TAB_EMULATION" );
  checkbox->setToolTip( "Turn on/off tab emulation using space characters" );
  layout->addWidget( checkbox );
  addOptionWidget( checkbox );
  
  OptionSpinBox* spinbox = new OptionSpinBox( box, "TAB_SIZE" );
  spinbox->setMinimum( 2 );
  spinbox->setMaximum( 20 );
  spinbox->setToolTip( "Tab size (in unit of space characters)." );
  grid_layout->addWidget( spinbox );
  addOptionWidget( spinbox );
  
  box = new QGroupBox( "Paragrap highlighting", parent );
  layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
  
  checkbox = new OptionCheckBox( "Highlight current paragraph", box, "HIGHLIGHT_PARAGRAPH" );
  checkbox->setToolTip( "Turn on/off current paragraph highlighting" );
  layout->addWidget( checkbox );
  addOptionWidget( checkbox );
 
  layout->addWidget( new QLabel( "Paragraph highlight color: " ) );
  OptionColorDisplay* color = new OptionColorDisplay( box, "HIGHLIGHT_COLOR" );
  layout->addWidget( color );
  addOptionWidget( color );
  
  box = new QGroupBox( "Misc", parent );
  layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
  
  checkbox = new OptionCheckBox( "Wrap text", box, "WRAP_TEXT" );
  checkbox->setToolTip( "Turn on/off line wrapping at editor border" );
  layout->addWidget( checkbox );
  addOptionWidget( checkbox );
  
}

//__________________________________________________
void BaseConfigurationDialog::_display( QListWidgetItem* current, QListWidgetItem* previous )
{
  Debug::Throw( "BaseConfigurationDialog::_display.\n" );
  
  if( !current ) current = previous;
  ConfigListItem* item( dynamic_cast<ConfigListItem*>(current) );
  Exception::checkPointer( item, DESCRIPTION( "invalid cast" ) );
  _stack().setCurrentWidget(&item->page());  
}

//__________________________________________________
void BaseConfigurationDialog::_read( void )
{
  Debug::Throw( "BaseConfigurationDialog::_read.\n" );
  for( OptionWidgetList::iterator iter = option_widgets_.begin(); iter != option_widgets_.end(); iter++ )
  (*iter)->read(); 
}

//__________________________________________________
void BaseConfigurationDialog::_update( void )
{
   Debug::Throw( "BaseConfigurationDialog::_update.\n" );
  
  for( OptionWidgetList::iterator iter = option_widgets_.begin(); iter != option_widgets_.end(); iter++ )
  (*iter)->write(); 
  
  // check if options have changed since last call
  _checkModified();
  
}

//__________________________________________________
void BaseConfigurationDialog::_restore( void )
{ 
  Debug::Throw( "BaseConfigurationDialog::_restore.\n" );
  if( XmlOptions::get() == backup_options_ ) return;
  XmlOptions::get() = backup_options_;
  modified_options_ = backup_options_;
  _read();
  emit configurationChanged();
}

//__________________________________________________
void BaseConfigurationDialog::_save( void )
{
  
  Debug::Throw( "BaseConfigurationDialog::_save.\n" );
  _update();
  XmlOptions::write();

}
