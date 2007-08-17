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
   \file SpellCheckConfiguration.cc
   \brief   Spell checker common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QLabel>

#include "CustomGridLayout.h"
#include "Debug.h"
#include "OptionBrowsedLineEdit.h"
#include "OptionComboBox.h"
#include "SpellCheckConfiguration.h"
#include "SpellInterface.h"

using namespace std;
using namespace SPELLCHECK;

//___________________________________________
SpellCheckConfiguration::SpellCheckConfiguration( QWidget* parent ):
  QGroupBox( "Spell check", parent ),
  OptionWidget( "" ),
  Counter( "SpellCheckConfiguration" )
{
  Debug::Throw( "SpellCheckConfiguration::SpellCheckConfiguration.\n" );

  SpellInterface interface;

  CustomGridLayout* grid_layout( new CustomGridLayout() );
  grid_layout->setSpacing( 5 );
  grid_layout->setMargin( 5 );
  grid_layout->setMaxCount( 2 );
  setLayout( grid_layout );

  // aspell command
  grid_layout->addWidget( new QLabel( "Aspell Command: ", this ) );
  grid_layout->addWidget( aspell_ = new OptionBrowsedLineEdit( this, "ASPELL" ) );
  aspell_->setToolTip( "Aspell command used to retrieve filtering modes and dictionaries." );

  // dictionaries
  grid_layout->addWidget( new QLabel( "Default Dictionary: ", this ) );
  grid_layout->addWidget( dictionary_ = new OptionComboBox( this, "DICTIONARY" ) );
  dictionary_->setToolTip( 
    "Default dictionary used with files for which\n"
    "a dictionary has not been manually selected" );
  
  const set<string>& dictionaries( interface.dictionaries() );
  for( set<string>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); iter++ )
  { dictionary_->addItem( iter->c_str() ); }
  
  // filters
  grid_layout->addWidget( new QLabel( "Default Filter: ", this ) );
  grid_layout->addWidget( filter_ = new OptionComboBox( this, "DICTIONARY_FILTER" ) );
  dictionary_->setToolTip( 
    "Default filtering mode used with files for which\n"
    "a filtering mode has not been manually selected" );

  const set<string>& filters( interface.filters() );
  for( set<string>::iterator iter = filters.begin(); iter != filters.end(); iter++ )
  { filter_->addItem(iter->c_str() ); }

  
  
}

//_________________________________________________________
void SpellCheckConfiguration::read( void )
{
  Debug::Throw( "SpellCheckConfiguration::read.\n" );
  aspell_->read();
  dictionary_->read();
  filter_->read();
}

//________________________________________________________
void SpellCheckConfiguration::write( void ) const
{
  Debug::Throw( "SpellCheckConfiguration::Write.\n" );
  aspell_->write();
  dictionary_->write();
  filter_->write();
}
