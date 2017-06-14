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

#include "TransparencyConfiguration.h"

#include "CompositeEngine.h"
#include "Debug.h"
#include "GridLayout.h"
#include "OptionCheckBox.h"
#include "OptionColorDisplay.h"
#include "OptionSlider.h"
#include "OptionSpinBox.h"

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

namespace Transparency
{

    //________________________________________________________________________
    TransparencyConfiguration::TransparencyConfiguration( QWidget* parent, Flags flags ):
        QWidget( parent ),
        OptionWidgetList( this ),
        Counter( "Transparency::TransparencyConfiguration" )
    {
        Debug::Throw( "TransparencyConfiguration::TransparencyConfiguration.\n" );

        if( (flags&Foreground) && (flags&Background) )
        {
            setLayout( new QVBoxLayout );
            layout()->setSpacing(5);
            layout()->setMargin(0);
        }

        // generic widget
        QWidget* box;

        // foreground
        if( flags&Foreground )
        {

            if( flags&Background )
            {
                box = new QGroupBox( "Foreground", this );
                layout()->addWidget( box );
            } else box = this;

            _foregroundConfiguration( box );
        }

        // background
        if( flags&Background )
        {
            if( flags&Foreground )
            {
                box = new QGroupBox( "Background", this );
                layout()->addWidget( box );
            } else box = this;

            _backgroundConfiguration( box );
        }

        return;
    }

    //________________________________________________________________________
    void TransparencyConfiguration::_foregroundConfiguration( QWidget* parent )
    {

        OptionColorDisplay* colorDisplay;
        OptionSlider* slider;
        OptionSpinBox* spinbox;
        OptionCheckBox* checkbox;

        GridLayout* gridLayout = new GridLayout;
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, (Qt::Alignment)(Qt::AlignRight|Qt::AlignVCenter));
        parent->setLayout( gridLayout );

        // foreground color
        gridLayout->addWidget( new QLabel( tr( "Foreground Color:" ), parent ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( parent, "TRANSPARENCY_FOREGROUND_COLOR" ) );
        colorDisplay->setToolTip( tr( "Text/display foreground color" ) );
        addOptionWidget( colorDisplay );

        // shadow color
        gridLayout->addWidget( new QLabel( tr( "Highlight color:" ), parent ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( parent, "TRANSPARENCY_SHADOW_COLOR" ) );
        colorDisplay->setToolTip( tr( "Text/display highlight color" ) );
        addOptionWidget( colorDisplay );

        // foreground intensity
        gridLayout->addWidget( new QLabel( tr( "Foreground intensity:" ), parent ) );
        gridLayout->addWidget( slider = new OptionSlider( parent, "TRANSPARENCY_FOREGROUND_INTENSITY" ) );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setSuffix( tr( "%" ) );
        slider->setToolTip( tr( "Foreground color intensity" ) );
        addOptionWidget( slider );

        // foreground highlight offset
        gridLayout->addWidget( new QLabel( tr( "Highlight size: " ), parent ) );
        gridLayout->addWidget( spinbox = new OptionSpinBox( parent, "TRANSPARENCY_SHADOW_OFFSET" ) );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );
        spinbox->setSuffix( tr( "px" ) );
        spinbox->setToolTip(
            tr( "Offset between text shadow and text.\n"
            "0 means no shadow." ) );
        addOptionWidget( spinbox );

        // inverse colors
        gridLayout->addWidget( checkbox = new OptionCheckBox( tr( "Inverse colors" ), parent, "TRANSPARENCY_INVERSE_COLORS" ), 4, 1, 1, 1 );
        addOptionWidget( checkbox );
        checkbox->setToolTip( tr( "Inverse foreground and highlight color, typically for dark themes" ) );

    }

    //________________________________________________________________________
    void TransparencyConfiguration::_backgroundConfiguration( QWidget* parent )
    {

        OptionColorDisplay* colorDisplay;
        OptionSlider* slider;

        GridLayout* gridLayout = new GridLayout;
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        parent->setLayout( gridLayout );

        // background color
        gridLayout->addWidget( new QLabel( tr( "Background color:" ), parent ) );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( parent, "TRANSPARENCY_TINT_COLOR" ) );
        colorDisplay->setToolTip( tr( "Transparent background tint color" ) );
        addOptionWidget( colorDisplay );

        // background intensity
        gridLayout->addWidget( new QLabel( tr( "Background intensity:" ), parent ) );
        gridLayout->addWidget( slider = new OptionSlider( parent, "TRANSPARENCY_TINT_INTENSITY" ) );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setSuffix( tr( "%" ) );
        slider->setToolTip( tr( "Transparent background tint intensity" ) );
        addOptionWidget( slider );

    }

}
