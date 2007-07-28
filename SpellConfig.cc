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
   \file SpellConfig.cc
   \brief   Spell checker common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QGridLayout>
#include <QLabel>

#include "Debug.h"
#include "OptionBrowsedLineEdit.h"
#include "OptionComboBox.h"
#include "SpellConfig.h"
#include "SpellInterface.h"

using namespace std;
using namespace SPELLCHECK;

//___________________________________________
SpellConfig::SpellConfig( QWidget* parent ):
  QGroupBox( "Spell check", parent ),
  OptionWidget( "" ),
  Counter( "SpellConfig" )
{
  Debug::Throw( "SpellConfig::SpellConfig.\n" );

  SpellInterface interface;

  QGridLayout* grid_layout( new QGridLayout() );
  grid_layout->setSpacing( 2 );
  grid_layout->setMargin( 5 );
  setLayout( grid_layout );

  // aspell command
  grid_layout->addWidget( new QLabel( "Aspell command: ", this ), 0, 0 );
  grid_layout->addWidget( aspell_ = new OptionBrowsedLineEdit( this, "ASPELL" ), 0, 1 );
  aspell_->setToolTip( "Aspell command. It is used to retrieve spell-checking modes." );

  // dictionaries
  grid_layout->addWidget( new QLabel( "Dictionary: ", this ), 1, 0 );
  grid_layout->addWidget( dictionary_ = new OptionComboBox( this, "DICTIONARY" ), 1, 1 );
  
  const set<string>& dictionaries( interface.dictionaries() );
  for( set<string>::iterator iter = dictionaries.begin(); iter != dictionaries.end(); iter++ )
  { dictionary_->addItem( iter->c_str() ); }
  
  dictionary_->setToolTip( "Dictionary language" );

  // filters
  grid_layout->addWidget( new QLabel( "Filter: ", this ), 2, 0 );
  grid_layout->addWidget( filter_ = new OptionComboBox( this, "DICTIONARY_FILTER" ), 2, 1 );

  const set<string>& filters( interface.filters() );
  for( set<string>::iterator iter = filters.begin(); iter != filters.end(); iter++ )
  { filter_->addItem(iter->c_str() ); }
  filter_->setToolTip( "Filtering mode" );

}

//_________________________________________________________
void SpellConfig::read( void )
{
  Debug::Throw( "SpellConfig::read.\n" );
  aspell_->read();
  dictionary_->read();
  filter_->read();
}

//________________________________________________________
void SpellConfig::write( void ) const
{
  Debug::Throw( "SpellConfig::Write.\n" );
  aspell_->write();
  dictionary_->write();
  filter_->write();
}
