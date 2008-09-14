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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/


/*!
   \file SvgConfiguration.cpp
   \brief   Svg common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QLabel>
#include <QPushButton>

#include "CustomDialog.h"
#include "Debug.h"
#include "GridLayout.h"
#include "OptionListBox.h"
#include "OptionCheckBox.h"
#include "OptionSpinBox.h"
#include "SvgConfiguration.h"
#include "XmlOptions.h"

using namespace std;

//___________________________________________
SvgConfiguration::SvgConfiguration( QWidget* parent ):
  QGroupBox( "Svg Background", parent ),
  Counter( "SvgConfiguration" )
{
  Debug::Throw( "SvgConfiguration::SvgConfiguration.\n" );

  QVBoxLayout *box_layout = new QVBoxLayout();
  box_layout->setSpacing(5);
  box_layout->setMargin(5);
  setLayout( box_layout );

  OptionCheckBox* checkbox;
  box_layout->addWidget( checkbox = new OptionCheckBox( "use svg background", this, "USE_SVG" )); 
  checkbox->setToolTip( "use svg to paint background" );
  addOptionWidget( checkbox ); 

  GridLayout *grid_layout = new GridLayout();
  grid_layout->setSpacing(5);
  grid_layout->setMargin(0);
  grid_layout->setMaxCount( 2 );
  box_layout->addLayout( grid_layout );

  grid_layout->addWidget( new QLabel( "filename: ", this ) );
  QPushButton *button = new QPushButton( "Edit svg file list", this );
  connect( button, SIGNAL( clicked() ), SLOT( _editSvgPathList() ) );
  grid_layout->addWidget( button );
  button->setToolTip( 
    "Set the list of valid backgrounds.\n"
    "Valid backgrounds are typical plasma svg backgrounds.\n"
    "They must contains element ids like \"topleft\",\n"
    "\"top\", \"center\", etc." );
  
  OptionSpinBox* spinbox;
  grid_layout->addWidget( new QLabel( "offset: ", this ) );
  grid_layout->addWidget( spinbox = new OptionSpinBox( this, "SVG_OFFSET" ) );
  spinbox->setMinimum( -16 );
  spinbox->setMaximum( 16 );
  spinbox->setToolTip( 
    "Offset used to draw svg.\n"
    "positive offset will make the SVG larger than the\n"
    "actual window size, thus shinking its edges\n" );
  addOptionWidget( spinbox );

}

//__________________________________________________
void SvgConfiguration::_editSvgPathList( void )
{
  
  Debug::Throw( "SvgConfiguration::_editSvgPathList.\n" );
  CustomDialog dialog( this );

  // store backup
  Options::List backup_options = XmlOptions::get().specialOptions( "SVG_BACKGROUND" );
  
  dialog.mainLayout().addWidget( new QLabel("Svg pathname: ", &dialog ) );
  OptionListBox *listbox = new OptionListBox( &dialog, "SVG_BACKGROUND" );
  listbox->setBrowsable( true );
  //listbox->setFileMode( QFileDialog::Directory );
  listbox->setToolTip( "Pathname to load background svg" );
  listbox->read();
  dialog.mainLayout().addWidget( listbox );
  
  // 
  if( dialog.exec() ) listbox->write();
  else { 
    // restore old values
    XmlOptions::get().clearSpecialOptions( "SVG_BACKGROUND" );
    for( Options::List::iterator iter = backup_options.begin(); iter != backup_options.end(); iter++ )
    { XmlOptions::get().add( "SVG_BACKGROUND", *iter ); }
  }
  return;
  
}
