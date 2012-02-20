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

#include "Debug.h"
#include "GridLayout.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionComboBox.h"
#include "SpellCheckConfiguration.h"
#include "SpellInterface.h"

#include <QtGui/QLabel>

//___________________________________________
SpellCheckConfiguration::SpellCheckConfiguration( QWidget* parent ):
QGroupBox( "Spell check", parent ),
Counter( "SpellCheckConfiguration" )
{
    Debug::Throw( "SpellCheckConfiguration::SpellCheckConfiguration.\n" );

    SPELLCHECK::SpellInterface interface;

    GridLayout* gridLayout( new GridLayout() );
    gridLayout->setSpacing( 5 );
    gridLayout->setMargin( 5 );
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    setLayout( gridLayout );

    // aspell command
    gridLayout->addWidget( new QLabel( "Aspell Command: ", this ) );
    OptionBrowsedLineEditor* editor( new OptionBrowsedLineEditor( this, "ASPELL" ) );
    gridLayout->addWidget( editor );
    editor->setToolTip( "Aspell command used to retrieve filtering modes and dictionaries." );
    addOptionWidget( editor );

    // dictionaries
    gridLayout->addWidget( new QLabel( "Default Dictionary: ", this ) );
    OptionComboBox* combobox(  new OptionComboBox( this, "DICTIONARY" ) );
    gridLayout->addWidget( combobox );
    combobox->setToolTip(
        "Default dictionary used with files for which\n"
        "a dictionary has not been manually selected" );
    addOptionWidget( combobox );

    const QSet<QString>& dictionaries( interface.dictionaries() );
    for( QSet<QString>::const_iterator iter = dictionaries.begin(); iter != dictionaries.end(); ++iter )
    { combobox->addItem( *iter ); }

    // filters
    gridLayout->addWidget( new QLabel( "Default Filter: ", this ) );
    combobox = new OptionComboBox( this, "DICTIONARY_FILTER" );
    gridLayout->addWidget( combobox );
    combobox->setToolTip(
        "Default filtering mode used with files for which\n"
        "a filtering mode has not been manually selected" );
    addOptionWidget( combobox );

    const QSet<QString>& filters( interface.filters() );
    for( QSet<QString>::const_iterator iter = filters.begin(); iter != filters.end(); ++iter )
    { combobox->addItem(*iter ); }

}
