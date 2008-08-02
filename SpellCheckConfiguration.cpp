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
   \file SpellCheckConfiguration.cpp
   \brief   Spell checker common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QLabel>

#include "GridLayout.h"
#include "Debug.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionComboBox.h"
#include "SpellCheckConfiguration.h"
#include "SpellInterface.h"

using namespace std;
using namespace SPELLCHECK;

//___________________________________________
SpellCheckConfiguration::SpellCheckConfiguration( QWidget* parent ):
  QGroupBox( "Spell check", parent ),
  Counter( "SpellCheckConfiguration" )
{
  Debug::Throw( "SpellCheckConfiguration::SpellCheckConfiguration.\n" );

  SpellInterface interface;

  GridLayout* grid_layout( new GridLayout() );
  grid_layout->setSpacing( 5 );
  grid_layout->setMargin( 5 );
  grid_layout->setMaxCount( 2 );
  setLayout( grid_layout );

  // aspell command
  grid_layout->addWidget( new QLabel( "Aspell Command: ", this ) );
  OptionBrowsedLineEditor* editor( new OptionBrowsedLineEditor( this, "ASPELL" ) );
  grid_layout->addWidget( editor );
  editor->setToolTip( "Aspell command used to retrieve filtering modes and dictionaries." );
  addOptionWidget( editor );
  
  // dictionaries
  grid_layout->addWidget( new QLabel( "Default Dictionary: ", this ) );
  OptionComboBox* combobox(  new OptionComboBox( this, "DICTIONARY" ) );
  grid_layout->addWidget( combobox );
  combobox->setToolTip( 
    "Default dictionary used with files for which\n"
    "a dictionary has not been manually selected" );
  addOptionWidget( combobox );
 
  const set<string>& dictionaries( interface.dictionaries() );
  for( set<string>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); iter++ )
  { combobox->addItem( iter->c_str() ); }
  
  // filters
  grid_layout->addWidget( new QLabel( "Default Filter: ", this ) );
  combobox = new OptionComboBox( this, "DICTIONARY_FILTER" );
  grid_layout->addWidget( combobox );
  combobox->setToolTip( 
    "Default filtering mode used with files for which\n"
    "a filtering mode has not been manually selected" );
  addOptionWidget( combobox );

  const set<string>& filters( interface.filters() );
  for( set<string>::iterator iter = filters.begin(); iter != filters.end(); iter++ )
  { combobox->addItem(iter->c_str() ); }
  
}
