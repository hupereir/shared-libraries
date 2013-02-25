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

#include "SpellCheckConfiguration.h"

#include "BaseIcons.h"
#include "Debug.h"
#include "DictionarySelectionButton.h"
#include "FilterSelectionButton.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionComboBox.h"
#include "SpellInterface.h"
#include "SpellItemDialog.h"

#include <QLabel>
#include <QToolButton>

namespace SPELLCHECK
{

    //___________________________________________
    SpellCheckConfiguration::SpellCheckConfiguration( QWidget* parent ):
        QGroupBox( "Spell Check", parent ),
        Counter( "SpellCheckConfiguration" )
    {
        Debug::Throw( "SpellCheckConfiguration::SpellCheckConfiguration.\n" );

        GridLayout* gridLayout( new GridLayout() );
        gridLayout->setSpacing( 5 );
        gridLayout->setMargin( 5 );
        gridLayout->setMaxCount( 3 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        setLayout( gridLayout );

        // aspell command
        gridLayout->addWidget( new QLabel( "Aspell command: ", this ), 0, 0, 1, 1 );
        OptionBrowsedLineEditor* editor( new OptionBrowsedLineEditor( this, "ASPELL" ) );
        gridLayout->addWidget( editor, 0, 1, 1, 2 );
        editor->setToolTip( "Aspell command used to retrieve filtering modes and dictionaries." );
        addOptionWidget( editor );

        // dictionaries
        gridLayout->addWidget( new QLabel( "Default dictionary: ", this ), 1, 0, 1, 1 );
        dictionariesComboBox_ =  new OptionComboBox( this, "DICTIONARY" );
        gridLayout->addWidget( dictionariesComboBox_, 1, 1, 1, 1 );
        dictionariesComboBox_->setToolTip(
            "Default dictionary used with files for which\n"
            "a dictionary has not been manually selected" );
        addOptionWidget( dictionariesComboBox_ );

        DictionarySelectionButton* dictionaryButton;
        gridLayout->addWidget( dictionaryButton = new DictionarySelectionButton( this ), 1, 2, 1, 1 );
        connect( dictionaryButton, SIGNAL( modified( void ) ), SLOT( _updateDictionaries( void ) ) );
        addOptionWidget( dictionaryButton );

        // filters
        gridLayout->addWidget( new QLabel( "Default filter: ", this ), 2, 0, 1, 1 );
        filtersComboBox_ = new OptionComboBox( this, "DICTIONARY_FILTER" );
        gridLayout->addWidget( filtersComboBox_, 2, 1, 1, 1 );
        filtersComboBox_->setToolTip(
            "Default filtering mode used with files for which\n"
            "a filtering mode has not been manually selected" );
        addOptionWidget( filtersComboBox_ );

        FilterSelectionButton* filterButton;
        gridLayout->addWidget( filterButton = new FilterSelectionButton( this ), 2, 2, 1, 1 );
        connect( filterButton, SIGNAL( modified( void ) ), SLOT( _updateFilters( void ) ) );
        addOptionWidget( filterButton );

        read();

        _updateDictionaries();
        _updateFilters();

    }

    //___________________________________________
    void SpellCheckConfiguration::_updateDictionaries( void )
    {
        Debug::Throw( "SpellCheckConfiguration::_updateDictionaries.\n" );

        // read list of disabled dictionaries
        const QStringList disabledDictionaries( QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_DICTIONARIES" ) ).split( " " ) );

        // clear combobox
        dictionariesComboBox_->clear();

        // get dictionary list and populate combobox
        SPELLCHECK::SpellInterface interface;
        foreach( const QString& dictionary, interface.dictionaries() )
        { if( !disabledDictionaries.contains( dictionary ) ) dictionariesComboBox_->addItem( dictionary ); }

        // read default value from options
        dictionariesComboBox_->read();

    }

    //___________________________________________
    void SpellCheckConfiguration::_updateFilters( void )
    {
        Debug::Throw( "SpellCheckConfiguration::_updateFilters.\n" );

        // read list of disabled filters
        const QStringList disabledFilters( QString( XmlOptions::get().raw( "SPELLCHECK_DISABLED_FILTERS" ) ).split( " " ) );

        // clear combobox
        filtersComboBox_->clear();

        // get dictionary list and populate combobox
        SPELLCHECK::SpellInterface interface;
        foreach( const QString& filter, interface.filters() )
        { if( !disabledFilters.contains( filter ) ) filtersComboBox_->addItem( filter ); }

        // read default value from options
        filtersComboBox_->read();

    }

}
