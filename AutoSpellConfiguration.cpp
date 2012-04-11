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

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include "AutoSpellConfiguration.h"
#include "GridLayout.h"
#include "Debug.h"
#include "OptionColorDisplay.h"
#include "OptionSpinBox.h"
#include "OptionFontInfo.h"

namespace SPELLCHECK
{
    //___________________________________________
    AutoSpellConfiguration::AutoSpellConfiguration( QWidget* parent ):
        QGroupBox( "Automatic spell check", parent ),
        Counter( "AutoSpellConfiguration" )
    {
        Debug::Throw( "AutoSpellConfiguration::AutoSpellConfiguration.\n" );

        GridLayout* gridLayout( new GridLayout() );
        gridLayout->setSpacing( 5 );
        gridLayout->setMargin( 5 );
        gridLayout->setMaxCount( 2 );
        setLayout( gridLayout );

        // suggestions
        gridLayout->addWidget( new QLabel( "Max number of suggestions: ", this ) );
        OptionSpinBox* spinbox = new OptionSpinBox( this, "MAX_SUGGESTIONS" ) ;
        gridLayout->addWidget( spinbox);
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 50 );
        spinbox->setToolTip( "Maximum number of suggestions in suggestion menu.\n 0 means no limit." );
        addOptionWidget( spinbox );

        // options
        gridLayout->addWidget( new QLabel( "Highlight color: ", this ), 1, 0 );
        OptionColorDisplay *colordisplay = new OptionColorDisplay( this, "AUTOSPELL_COLOR" );
        gridLayout->addWidget( colordisplay  );
        colordisplay->setToolTip( "Highlight color for misspelled words" );
        addOptionWidget( colordisplay );

        gridLayout->addWidget( new QLabel( "Highlight font format: ", this ), 2, 0 );
        OptionFontInfo* fontinfo =  new OptionFontInfo( this, "AUTOSPELL_FONT_FORMAT" );
        gridLayout->addWidget( fontinfo );
        fontinfo->setToolTip( "Font format for misspelled words" );
        addOptionWidget( fontinfo );

    }
}
