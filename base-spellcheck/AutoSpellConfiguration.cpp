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

#include "AutoSpellConfiguration.h"
#include "GridLayout.h"
#include "Debug.h"
#include "OptionColorDisplay.h"
#include "OptionSpinBox.h"
#include "OptionFontInfo.h"
#include "XmlOptions.h"

#include <QGridLayout>
#include <QLabel>

namespace SpellCheck
{
    //___________________________________________
    AutoSpellConfiguration::AutoSpellConfiguration( QWidget* parent ):
        QWidget( parent ),
        OptionWidgetList( this ),
        Counter( QStringLiteral("SpellCheck::AutoSpellConfiguration") )
    {
        Debug::Throw( QStringLiteral("AutoSpellConfiguration::AutoSpellConfiguration.\n") );

        auto gridLayout( new GridLayout );
        gridLayout->setSpacing( 5 );
        gridLayout->setMargin( 5 );
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        setLayout( gridLayout );

        // suggestions
        gridLayout->addWidget( new QLabel( tr( "Maximum number of suggestions:" ), this ) );
        auto spinbox = new OptionSpinBox( this, QStringLiteral("MAX_SUGGESTIONS") ) ;
        gridLayout->addWidget( spinbox);
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 50 );
        spinbox->setSpecialValueText( tr( "Unlimited" ) );
        spinbox->setToolTip( tr( "Maximum number of suggestions in suggestion menu.\n 0 means no limit." ) );
        addOptionWidget( spinbox );

        // options
        gridLayout->addWidget( new QLabel( tr( "Highlight color:" ), this ), 1, 0 );
        auto colordisplay = new OptionColorDisplay( this, QStringLiteral("AUTOSPELL_COLOR") );
        gridLayout->addWidget( colordisplay  );
        colordisplay->setToolTip( tr( "Highlight color for misspelled words" ) );
        addOptionWidget( colordisplay );

        gridLayout->addWidget( new QLabel( tr( "Highlight font format:" ), this ), 2, 0 );
        auto fontinfo =  new OptionFontInfo( this, QStringLiteral("AUTOSPELL_FONT_FORMAT") );
        fontinfo->layout()->setMargin(0);
        gridLayout->addWidget( fontinfo, 2, 1, 5, 1 );
        fontinfo->setToolTip( tr( "Font format for misspelled words" ) );
        addOptionWidget( fontinfo );

        read( XmlOptions::get() );

    }
}
