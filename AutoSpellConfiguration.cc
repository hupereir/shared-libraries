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
   \file AutoSpellConfiguration.cc
   \brief automatic spelling configuration
   \version $Revision$
   \date $Date$
*/

#include <QGridLayout>
#include <QLabel>

#include "AutoSpellConfiguration.h"
#include "CustomGridLayout.h"
#include "Debug.h"
#include "OptionColorDisplay.h"
#include "OptionSpinBox.h"
#include "OptionFontInfo.h"

using namespace std;

//___________________________________________
AutoSpellConfiguration::AutoSpellConfiguration( QWidget* parent ):
  QGroupBox( "Automatic spell check", parent ),
  OptionWidget( "" ),
  Counter( "AutoSpellConfiguration" )
{
  Debug::Throw( "AutoSpellConfiguration::AutoSpellConfiguration.\n" );
  
  CustomGridLayout* grid_layout( new CustomGridLayout() );
  grid_layout->setSpacing( 2 );
  grid_layout->setMargin( 5 );
  grid_layout->setMaxCount( 2 );
  setLayout( grid_layout );
  
  // suggestions
  grid_layout->addWidget( new QLabel( "Max number of suggestions: ", this ) );
  grid_layout->addWidget( suggestions_ = new OptionSpinBox( this, "MAX_SUGGESTIONS" ) );
  suggestions_->setMinimum( 0 );
  suggestions_->setMaximum( 50 );
  suggestions_->setToolTip( "Maximum number of suggestions in suggestion menu.\n 0 means no limit." );

  // options
  grid_layout->addWidget( new QLabel( "Highlight color: ", this ), 1, 0 );
  grid_layout->addWidget( highlight_ = new OptionColorDisplay( this, "AUTOSPELL_COLOR" ) );
  highlight_->setToolTip( "Highlight color for misspelled words" );

  grid_layout->addWidget( new QLabel( "Highlight font format: ", this ), 2, 0 );
  grid_layout->addWidget( font_info_ = new OptionFontInfo( this, "AUTOSPELL_FONT_FORMAT" ) );
  font_info_->setToolTip( "Font format for misspelled words" );
}

//____________________________________________________________
void AutoSpellConfiguration::read( void )
{
  Debug::Throw( "AutoSpellConfiguration::read.\n" );
  suggestions_->read();
  highlight_->read();
  font_info_->read();
}

//_____________________________________________________________
void AutoSpellConfiguration::write( void ) const
{
  Debug::Throw( "AutoSpellConfiguration::Write.\n" );
  suggestions_->write();
  highlight_->write();
  font_info_->write();
}
