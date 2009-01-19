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
  \file BaseConfigurationDialog.cpp
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QApplication>
#include <QScrollArea>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>

#include "BaseIcons.h"
#include "BaseConfigurationDialog.h"
#include "GridLayout.h"
#include "CustomDialog.h"
#include "Debug.h"
#include "IconEngine.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionIconBrowsedButton.h"
#include "OptionFontEditor.h"
#include "OptionLineEditor.h"
#include "OptionListBox.h"
#include "OptionModel.h"
#include "OptionSlider.h"
#include "OptionSpinBox.h"
#include "QuestionDialog.h"
#include "Str.h"
#include "TreeView.h"

using namespace std;
using namespace Qt;

//_________________________________________________________
BaseConfigurationDialog::BaseConfigurationDialog( QWidget* parent ):
  TabbedDialog( parent ),
  modified_options_( XmlOptions::get() ),
  backup_options_( XmlOptions::get() )
{
  
  Debug::Throw( "BaseConfigurationDialog::BaseConfigurationDialog.\n" );
  setWindowTitle( "Configuration" );
  setSizeOptionName( "CONFIGURATION_DIALOG" );

  // add restore default button to layout
  QPushButton* button;
  button = new QPushButton( IconEngine::get( ICONS::RELOAD ), "Restore &Defaults", this );
  button->setToolTip( "Restore default value for all options.");
  button->setAutoDefault( false );
  
  connect( button, SIGNAL( clicked() ), SLOT( _restoreDefaults() ) );
  _buttonLayout().addWidget( button );
  _buttonLayout().addStretch( 1 );
  
  // apply button
  _buttonLayout().addWidget( button = new QPushButton(IconEngine::get( ICONS::DIALOG_OK_APPLY ), "&Apply", this ), 1 );
  connect( button, SIGNAL( clicked() ), SLOT( _update() ) );  
  connect( button, SIGNAL( clicked() ), SIGNAL( apply() ) );  
  button->setToolTip( 
    "Apply changes to options.\n"
    "Note: the application may have to be restarted so that\n"
    "all changes are taken into account." );
  
  // ok button
  _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_OK ), "&Ok", this ), 1 );
  connect( button, SIGNAL( clicked() ), SLOT( _save() ) );  
  connect( button, SIGNAL( clicked() ), SIGNAL( ok() ) );  
  connect( button, SIGNAL( clicked() ), SLOT( accept() ) );  
  button->setToolTip( 
    "Apply changes to options and close window.\n"
    "Note: the application may have to be restarted so that\n"
    "all changes are taken into account." );
  button->setAutoDefault( false );
  
  // cancel button
  _buttonLayout().addWidget( button = new QPushButton( IconEngine::get( ICONS::DIALOG_CANCEL ), "&Cancel", this ), 1 );
  connect( button, SIGNAL( clicked() ), SLOT( _restore() ) );
  connect( button, SIGNAL( clicked() ), SIGNAL( cancel() ) );  
  connect( button, SIGNAL( clicked() ), SLOT( reject() ) );
  button->setToolTip( "Discard changes to options and close window" );
  button->setAutoDefault( false );
    
}

//__________________________________________________
void BaseConfigurationDialog::baseConfiguration( QWidget* parent, const unsigned int& flag )
{
  
  Debug::Throw( "BaseConfigurationDialog::baseConfiguration.\n" );
  
  if( !parent ) parent = &addPage( "Base", "Generic settings" );
  
  // base
  if( flag & BASE )
  { 
        
    // base
    QGroupBox* box;
    box = new QGroupBox(  parent );
    parent->layout()->addWidget( box );

    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->setMargin(5);
    h_layout->setSpacing(5);
    box->setLayout( h_layout );
    
    // icon pixmap
    QVBoxLayout* v_layout( new QVBoxLayout() );
    v_layout->setMargin(0);
    v_layout->setSpacing(5);
    h_layout->addLayout( v_layout );
        
    OptionIconBrowsedButton* editor = new OptionIconBrowsedButton( parent, "ICON_PIXMAP" );
    editor->setToolTip( "Application icon" );
    addOptionWidget( editor );
    v_layout->addWidget( editor );
    v_layout->addStretch(1);
    h_layout->addSpacing( 10 );
    
    // base options
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(5);
    h_layout->addLayout( layout );
            
    GridLayout* grid_layout = new GridLayout();
    grid_layout->setMargin(0);
    grid_layout->setSpacing(5);
    grid_layout->setMaxCount(2);
    layout->addLayout( grid_layout );
     
    // flat theme
    OptionCheckBox* checkbox( new OptionCheckBox( "Use flat plastique theme", box, "USE_FLAT_THEME" ) );
    grid_layout->addWidget( checkbox, 0, 0, 1, 2 );
    checkbox->setToolTip( "Use customized flat plastique theme for widget appearance and layout.\nThe application must be restarted to take changes to this option into account." );
    addOptionWidget( checkbox );
    
    // base font    
    grid_layout->addWidget( new QLabel( "Default font: ", box ) );
    OptionFontEditor *edit = new OptionFontEditor( box, "FONT_NAME" );
    edit->setToolTip( "Default font name for all widgets" );
    grid_layout->addWidget( edit );
    addOptionWidget( edit );
    
    // fixed font
    grid_layout->addWidget( new QLabel( "Fixed font: ", box ) );
    edit = new OptionFontEditor( box, "FIXED_FONT_NAME" );
    edit->setToolTip( "Default font name (fixed) for text widgets" );
    grid_layout->addWidget( edit );
    addOptionWidget( edit );
      
    // pixmap path
    grid_layout->addWidget( new QLabel( "Pixmaps: ", box ) );
    QPushButton *button = new QPushButton( "Edit pixmap path list", box );
    connect( button, SIGNAL( clicked() ), SLOT( _editPixmapPathList() ) );
    grid_layout->addWidget( button );
  
    // debug level
    grid_layout->addWidget( new QLabel( "Debug level: ", box ) );
    OptionSpinBox* spinbox = new OptionSpinBox( box, "DEBUG_LEVEL" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 5 );
    spinbox->setToolTip( "Debug verbosity level" );
    grid_layout->addWidget( spinbox );
    addOptionWidget( spinbox );
  
    // debug level
    grid_layout->addWidget( new QLabel( "Animations duration (ms): ", box ) );
    spinbox = new OptionSpinBox( box, "ANIMATION_DURATION" );
    spinbox->setMinimum( 0 );
    spinbox->setMaximum( 5000 );
    spinbox->setToolTip( "Animations duration.\nSetting this value to zero disables all animations." );
    grid_layout->addWidget( spinbox );
    addOptionWidget( spinbox );
  
  }
    
  // list
  if( flag & LIST ) { listConfiguration( parent ); }
  
  // tabs
  if( flag & TEXTEDIT ) { textEditConfiguration( parent ); }

  Debug::Throw( "BaseConfigurationDialog::baseConfiguration - done.\n" );
}

//__________________________________________________
void BaseConfigurationDialog::listConfiguration( QWidget* parent )
{
  
  Debug::Throw( "BaseConfigurationDialog::listConfiguration.\n" );

  // make sure parent is valid
  if( !parent ) parent = &addPage( "Lists", "Look-and-feel for item lists" );
  
  QGroupBox* box = new QGroupBox( "Lists", parent );
  GridLayout* grid_layout = new GridLayout();
  grid_layout->setMargin(5);
  grid_layout->setSpacing(5);
  grid_layout->setMaxCount(2);
  box->setLayout( grid_layout );
  parent->layout()->addWidget( box );
    
  OptionColorDisplay* color;
  grid_layout->addWidget( new QLabel( "Item alternate background color ", box ) );  
  grid_layout->addWidget( color = new OptionColorDisplay( box, "ALTERNATE_COLOR" ) );
  color->setToolTip( "Alternate item background color in lists.\n Set it to \"None\" do disable alternate item color." );
  addOptionWidget( color );
   
  grid_layout->addWidget( new QLabel( "Selected column background color ", box ) ); 
  grid_layout->addWidget( color = new OptionColorDisplay( box, "SELECTED_COLUMN_COLOR" ) );
  color->setToolTip( "Selected column background color in lists.\n Set it to \"None\" do disable alternate item color." );
  addOptionWidget( color );
  
  OptionSpinBox* spinbox;
  grid_layout->addWidget( new QLabel( "List items icon size ", box ) ); 
  grid_layout->addWidget( spinbox = new OptionSpinBox( box, "LIST_ICON_SIZE" ) );
  spinbox->setMinimum(8);
  spinbox->setMaximum(96);
  addOptionWidget( spinbox );
  
}

//__________________________________________________
void BaseConfigurationDialog::textEditConfiguration( QWidget* parent )
{
  
  Debug::Throw( "BaseConfigurationDialog::textEditConfiguration.\n" );

  // make sure parent is valid
  if( !parent ) parent = &addPage( "Text edition", "Configuration and look-and-feel for text edition" );
  
  // tab emulation
  QGroupBox* box = new QGroupBox( "Tab emulation", parent );
  QVBoxLayout* layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  parent->layout()->addWidget( box );
     
  GridLayout* grid_layout = new GridLayout();
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
  
  // paragraph highlighting
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
 
  grid_layout = new GridLayout();
  grid_layout->setMargin(0);
  grid_layout->setSpacing(5);
  grid_layout->setMaxCount(2);
  layout->addLayout( grid_layout );
  
  grid_layout->addWidget( new QLabel( "Paragraph highlight color: " ) );
  OptionColorDisplay* color = new OptionColorDisplay( box, "HIGHLIGHT_COLOR" );
  grid_layout->addWidget( color );
  addOptionWidget( color );

  // box selection
  box = new QGroupBox( "Box selection", parent );
  grid_layout = new GridLayout();
  grid_layout->setMargin(5);
  grid_layout->setSpacing(5);
  grid_layout->setMaxCount(2);
  box->setLayout( grid_layout );
  parent->layout()->addWidget( box );
  
  grid_layout->addWidget( new QLabel( "Alpha channel" ) );
  OptionSlider *slider = new OptionSlider( box, "BOX_SELECTION_ALPHA" );
  slider->slider().setMinimum( 0 );
  slider->slider().setMaximum( 100 );
  slider->setToolTip( "Alpha channel (i.e. opacity) of the box selection highlight color" );
  grid_layout->addWidget( slider );
  addOptionWidget( slider );
  
  if( XmlOptions::get().find( "BOX_SELECTION_COLOR" ) )
  {
    grid_layout->addWidget( new QLabel( "Box selection color" ) );
    OptionColorDisplay* color = new OptionColorDisplay( box, "BOX_SELECTION_COLOR" );
    grid_layout->addWidget( color );
    addOptionWidget( color );
  }
  
  grid_layout->addWidget( new QLabel(
    "Note: box selection is enabled in text editors\n"
    "only if the corresponding font has fixed pitch."), 2, 0, 1, 2 );

  // margins
  parent->layout()->addWidget( box = new QGroupBox( "Margin appearance", parent ) );
 
  box->setLayout( new QVBoxLayout() );
  box->layout()->setSpacing(5);
  box->layout()->setMargin(5);

  box->layout()->addWidget( checkbox = new OptionCheckBox( "Draw vertical line", box, "MARGIN_VERTICAL_LINE" ) );
  checkbox->setToolTip( 
    "Draw vertical line between margin and body text. "
    "Margins are used to display additional informations such as tags and line numbers."
    );
  addOptionWidget( checkbox );
  
  grid_layout = new GridLayout();
  grid_layout->setSpacing(5);
  grid_layout->setMargin(0);
  grid_layout->setMaxCount(2);
  box->layout()->addItem( grid_layout );

  OptionColorDisplay* color_display;
  
  grid_layout->addWidget( new QLabel( "Margin foreground", box ) );
  grid_layout->addWidget( color_display = new OptionColorDisplay( box, "MARGIN_FOREGROUND" ) );
  addOptionWidget( color_display );
  color_display->setToolTip( "Margins foreground color" );

  grid_layout->addWidget( new QLabel( "Margin background", box ) );
  grid_layout->addWidget( color_display = new OptionColorDisplay( box, "MARGIN_BACKGROUND" ) );
  addOptionWidget( color_display );
  color_display->setToolTip( "Margins background color" );

  // misc
  parent->layout()->addWidget( box = new QGroupBox( "Flags", parent ) );
  layout = new QVBoxLayout();
  layout->setMargin(5);
  layout->setSpacing(5);
  box->setLayout( layout );
  
  checkbox = new OptionCheckBox( "Wrap text", box, "WRAP_TEXT" );
  checkbox->setToolTip( "Turn on/off line wrapping at editor border" );
  layout->addWidget( checkbox );
  addOptionWidget( checkbox );
  
  layout->addWidget( checkbox = new OptionCheckBox( "Show line numbers", box, "SHOW_LINE_NUMBERS" ) );
  checkbox->setToolTip( "Turn on/off line numbers" );
  addOptionWidget( checkbox );

  Debug::Throw( "BaseConfigurationDialog::textEditConfiguration - done.\n" );

}

//__________________________________________________
void BaseConfigurationDialog::_editPixmapPathList( void )
{
  
  CustomDialog dialog( this );

  // store backup
  Options::List backup_options = XmlOptions::get().specialOptions( "PIXMAP_PATH" );
  
  dialog.mainLayout().addWidget( new QLabel("Pixmap pathname: ", &dialog ) );
  OptionListBox *listbox = new OptionListBox( &dialog, "PIXMAP_PATH" );
  listbox->setBrowsable( true );
  listbox->setFileMode( QFileDialog::Directory );
  listbox->setToolTip( "Pathname to load toolbar pixmaps" );
  listbox->read();
  dialog.mainLayout().addWidget( listbox );
  
  // 
  if( dialog.exec() ) listbox->write();
  else { 
    // restore old values
    XmlOptions::get().clearSpecialOptions( "PIXMAP_PATH" );
    for( Options::List::iterator iter = backup_options.begin(); iter != backup_options.end(); iter++ )
    { XmlOptions::get().add( "PIXMAP_PATH", *iter ); }
  }
  return;
  
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
void BaseConfigurationDialog::_restoreDefaults( void )
{ 
  Debug::Throw( "BaseConfigurationDialog::_restoreDefaults.\n" );
  
  // list options that have no default values
  const Options::Map& options( XmlOptions::get().options() );
  OptionModel::Set local;
  for( Options::Map::const_iterator iter = options.begin(); iter != options.end(); iter++ )
  { if( iter->second.defaultValue().empty() ) local.insert( OptionPair( *iter ) ); }

  QuestionDialog dialog( this );
  dialog.setSizeOptionName( "RESTORE_DEFAULT_DIALOG" );
  if( !local.empty() )
  {

    dialog.setText( "Following options have no default values. Restore default anyway ? " );
    TreeView* view = new TreeView( &dialog );
    OptionModel* model = new OptionModel( &dialog );
    model->add( local );
    view->setModel( model );
    view->setMask( (1<<OptionModel::NAME) | (1<<OptionModel::VALUE) );
    view->resizeColumns();
    dialog.mainLayout().addWidget( view );
    dialog.resize( 450, 300 );

  } else { dialog.setText( "Restore all options to their default values ?" ); }
  
  // ask confirmation
  if( !dialog.centerOnWidget( this ).exec() ) return;

  XmlOptions::get().restoreDefaults();
  _read();
  emit configurationChanged();
}

//__________________________________________________
void BaseConfigurationDialog::_read( void )
{ 
  Debug::Throw( "BaseConfigurationDialog::_read.\n" );
  OptionWidgetList::read(); 
}

//__________________________________________________
void BaseConfigurationDialog::_save( void )
{
  
  Debug::Throw( "BaseConfigurationDialog::_save.\n" );
  _update();
  XmlOptions::write();

}
