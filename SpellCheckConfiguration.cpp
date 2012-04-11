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
#include "GridLayout.h"
#include "IconEngine.h"
#include "OptionBrowsedLineEditor.h"
#include "OptionComboBox.h"
#include "SpellInterface.h"

#include "SpellItemDialog.h"

#include <QtGui/QLabel>
#include <QtGui/QToolButton>

namespace SPELLCHECK
{

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
        gridLayout->setMaxCount( 3 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        setLayout( gridLayout );

        // aspell command
        gridLayout->addWidget( new QLabel( "Aspell Command: ", this ), 0, 0, 1, 1 );
        OptionBrowsedLineEditor* editor( new OptionBrowsedLineEditor( this, "ASPELL" ) );
        gridLayout->addWidget( editor, 0, 1, 1, 2 );
        editor->setToolTip( "Aspell command used to retrieve filtering modes and dictionaries." );
        addOptionWidget( editor );

        // dictionaries
        gridLayout->addWidget( new QLabel( "Default Dictionary: ", this ), 1, 0, 1, 1 );
        dictionariesComboBox_ =  new OptionComboBox( this, "DICTIONARY" );
        gridLayout->addWidget( dictionariesComboBox_, 1, 1, 1, 1 );
        dictionariesComboBox_->setToolTip(
            "Default dictionary used with files for which\n"
            "a dictionary has not been manually selected" );
        addOptionWidget( dictionariesComboBox_ );

        const QSet<QString>& dictionaries( interface.dictionaries() );
        for( QSet<QString>::const_iterator iter = dictionaries.begin(); iter != dictionaries.end(); ++iter )
        { dictionariesComboBox_->addItem( *iter ); }

        QToolButton* toolButton;
        gridLayout->addWidget( toolButton = new QToolButton( this ), 1, 2, 1, 1 );
        toolButton->setIcon( IconEngine::get( ICONS::CONFIGURE ) );
        toolButton->setAutoRaise( true );
        connect( toolButton, SIGNAL( clicked( void ) ), SLOT( _editDictionaries( void ) ) );

        // filters
        gridLayout->addWidget( new QLabel( "Default Filter: ", this ), 2, 0, 1, 1 );
        filterComboBox_ = new OptionComboBox( this, "DICTIONARY_FILTER" );
        gridLayout->addWidget( filterComboBox_, 2, 1, 1, 1 );
        filterComboBox_->setToolTip(
            "Default filtering mode used with files for which\n"
            "a filtering mode has not been manually selected" );
        addOptionWidget( filterComboBox_ );

        const QSet<QString>& filters( interface.filters() );
        for( QSet<QString>::const_iterator iter = filters.begin(); iter != filters.end(); ++iter )
        { filterComboBox_->addItem(*iter ); }

        gridLayout->addWidget( toolButton = new QToolButton( this ), 2, 2, 1, 1 );
        toolButton->setIcon( IconEngine::get( ICONS::CONFIGURE ) );
        toolButton->setAutoRaise( true );
        connect( toolButton, SIGNAL( clicked( void ) ), SLOT( _editFilters( void ) ) );

    }

    //___________________________________________
    void SpellCheckConfiguration::_editDictionaries( void )
    {

        Debug::Throw( "SpellCheckConfiguration::_editDictionaries.\n" );

        // get backup
        QString backup(
            XmlOptions::get().contains( "SPELLCHECK_DISABLED_DICTIONARIES" ) ?
            XmlOptions::get().raw( "SPELLCHECK_DISABLED_DICTIONARIES" ) : "" );

        // get list of dictionaries
        SPELLCHECK::SpellInterface interface;
        const QSet<QString>& dictionaries( interface.dictionaries() );

        // create dialog
        SpellItemDialog dialog( this );
        dialog.setItems( dictionaries );
        dialog.setDisabledItems( backup );

        if( !dialog.centerOnWidget( this ).exec() ) return;

    }

    //___________________________________________
    void SpellCheckConfiguration::_editFilters( void )
    {

            Debug::Throw( "SpellCheckConfiguration::_editFilters.\n" );


    }

}
