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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "SpellCheckConfiguration.h"

#include "Debug.h"
#include "DictionarySelectionButton.h"
#include "FilterSelectionButton.h"
#include "GridLayout.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionComboBox.h"
#include "SpellInterface.h"
#include "SpellItemDialog.h"
#include "XmlOptions.h"

#include <QLabel>
#include <QToolButton>

namespace SpellCheck
{

    //___________________________________________
    SpellCheckConfiguration::SpellCheckConfiguration( QWidget* parent, Flags flags ):
        QWidget( parent ),
        OptionWidgetList( this ),
        Counter( QStringLiteral("SpellCheck::SpellCheckConfiguration") )
    {
        Debug::Throw( QStringLiteral("SpellCheckConfiguration::SpellCheckConfiguration.\n") );

        auto gridLayout( new GridLayout );
        gridLayout->setSpacing( 5 );
        gridLayout->setMargin( 5 );
        gridLayout->setMaxCount( 3 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        setLayout( gridLayout );

        // aspell command
        if( flags & ASpell )
        {
            gridLayout->addWidget( new QLabel( tr( "Aspell command:" ), this ), 0, 0, 1, 1 );
            auto editor( new OptionBrowsedLineEditor( this, QStringLiteral("ASPELL") ) );
            gridLayout->addWidget( editor, 0, 1, 1, 2 );
            editor->setToolTip( QStringLiteral("Aspell command used to retrieve filtering modes and dictionaries.") );
            addOptionWidget( editor );
        }

        // dictionaries
        if( flags & Dictionary )
        {
            gridLayout->addWidget( new QLabel( tr( "Default dictionary:" ), this ), 1, 0, 1, 1 );
            dictionariesComboBox_ =  new OptionComboBox( this, QStringLiteral("DICTIONARY") );
            gridLayout->addWidget( dictionariesComboBox_, 1, 1, 1, 1 );
            dictionariesComboBox_->setToolTip(
                tr( "Default dictionary used with files for which\n"
                "a dictionary has not been manually selected" ) );
            addOptionWidget( dictionariesComboBox_ );

            DictionarySelectionButton* dictionaryButton;
            gridLayout->addWidget( dictionaryButton = new DictionarySelectionButton( this ), 1, 2, 1, 1 );
            connect( dictionaryButton, &DictionarySelectionButton::modified, this, &SpellCheckConfiguration::_updateDictionaries );
            addOptionWidget( dictionaryButton );

        }

        // filters
        if( flags & Filter )
        {
            gridLayout->addWidget( new QLabel( tr( "Default filter:" ), this ), 2, 0, 1, 1 );
            filtersComboBox_ = new OptionComboBox( this, QStringLiteral("DICTIONARY_FILTER") );
            gridLayout->addWidget( filtersComboBox_, 2, 1, 1, 1 );
            filtersComboBox_->setToolTip(
                tr( "Default filtering mode used with files for which\n"
                "a filtering mode has not been manually selected" ) );
            addOptionWidget( filtersComboBox_ );

            FilterSelectionButton* filterButton;
            gridLayout->addWidget( filterButton = new FilterSelectionButton( this ), 2, 2, 1, 1 );
            connect( filterButton, &FilterSelectionButton::modified, this, &SpellCheckConfiguration::_updateFilters );
            addOptionWidget( filterButton );
        }

        read( XmlOptions::get() );

        _updateDictionaries();
        _updateFilters();

    }

    //___________________________________________
    void SpellCheckConfiguration::_updateDictionaries()
    {
        Debug::Throw( QStringLiteral("SpellCheckConfiguration::_updateDictionaries.\n") );

        if( !dictionariesComboBox_ ) return;

        // read list of disabled dictionaries
        const auto disabledDictionaries( QString( XmlOptions::get().raw( QStringLiteral("SPELLCHECK_DISABLED_DICTIONARIES") ) ).split( QLatin1Char(' ') ) );

        // clear combobox
        dictionariesComboBox_->clear();

        // get dictionary list and populate combobox
        SpellCheck::SpellInterface interface;
        for( const auto& dictionary:interface.dictionaries() )
        { if( !disabledDictionaries.contains( dictionary ) ) dictionariesComboBox_->addItem( dictionary ); }

        // read default value from options
        dictionariesComboBox_->read( XmlOptions::get() );

    }

    //___________________________________________
    void SpellCheckConfiguration::_updateFilters()
    {
        Debug::Throw( QStringLiteral("SpellCheckConfiguration::_updateFilters.\n") );

        if( !filtersComboBox_ ) return;

        // read list of disabled filters
        const auto disabledFilters( QString( XmlOptions::get().raw( QStringLiteral("SPELLCHECK_DISABLED_FILTERS") ) ).split( QLatin1Char(' ') ) );

        // clear combobox
        filtersComboBox_->clear();

        // get dictionary list and populate combobox
        SpellCheck::SpellInterface interface;
        for( const auto& filter:interface.filters() )
        { if( !disabledFilters.contains( filter ) ) filtersComboBox_->addItem( filter ); }

        // read default value from options
        filtersComboBox_->read( XmlOptions::get() );

    }

}
