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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "TransparencyConfiguration.h"
#include "TransparencyConfiguration.moc"

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

namespace TRANSPARENCY
{

    //________________________________________________________________________
    TransparencyConfiguration::TransparencyConfiguration( QWidget* parent ):
        QWidget( parent ),
        OptionWidgetList( this ),
        Counter( "TRANSPARENCY::TransparencyConfiguration" )
    {
        Debug::Throw( "TransparencyConfiguration::TransparencyConfiguration.\n" );

        setLayout( new QVBoxLayout() );
        layout()->setSpacing(5);
        layout()->setMargin(0);

        // generic objects
        QWidget *box;
        OptionColorDisplay* colorDisplay;
        OptionSlider* slider;
        OptionSpinBox* spinbox;
        OptionCheckBox* checkbox;
        GridLayout* gridLayout;

        // foreground
        layout()->addWidget( box = new QGroupBox( "Foreground", this ) );

        gridLayout = new GridLayout();
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->setLayout( gridLayout );

        // foreground color
        gridLayout->addWidget( new QLabel( tr( "Foreground Color:" ), box ), 0, 0, 1, 1 );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "TRANSPARENCY_FOREGROUND_COLOR" ), 0, 1, 1, 1 );
        colorDisplay->setToolTip( tr( "Text/display foreground color" ) );
        addOptionWidget( colorDisplay );

        // shadow color
        gridLayout->addWidget( new QLabel( tr( "Highlight color:" ), box ), 1, 0, 1, 1 );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "TRANSPARENCY_SHADOW_COLOR" ), 1, 1, 1, 1 );
        colorDisplay->setToolTip( tr( "Text/display highlight color" ) );
        addOptionWidget( colorDisplay );

        // foreground intensity
        gridLayout->addWidget( new QLabel( tr( "Foreground intensity:" ), box ), 2, 0, 1, 1 );
        gridLayout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_FOREGROUND_INTENSITY" ), 2, 1, 1, 1 );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setSuffix( tr( "%" ) );
        slider->setToolTip( tr( "Foreground color intensity" ) );
        addOptionWidget( slider );

        // foreground highlight offset
        gridLayout->addWidget( new QLabel( tr( "Highlight offset: " ), box ), 3, 0, 1, 1 );
        gridLayout->addWidget( spinbox = new OptionSpinBox( box, "TRANSPARENCY_SHADOW_OFFSET" ), 3, 1, 1, 1 );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );
        spinbox->setSuffix( tr( "px" ) );
        spinbox->setToolTip(
            tr( "Offset between text shadow and text.\n"
            "0 means no shadow." ) );
        addOptionWidget( spinbox );

        // background
        layout()->addWidget( box = new QGroupBox( "Background", this ) );

        gridLayout = new GridLayout();
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->setLayout( gridLayout );

        // background color
        gridLayout->addWidget( new QLabel( tr( "Background color:" ), box ), 0, 0, 1, 1 );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "TRANSPARENCY_TINT_COLOR" ), 0, 1, 1, 1 );
        colorDisplay->setToolTip( tr( "Transparent background tint color" ) );
        addOptionWidget( colorDisplay );

        // highlight color
        gridLayout->addWidget( new QLabel( tr( "Highlight color:" ), box ), 1, 0, 1, 1 );
        gridLayout->addWidget( colorDisplay = new OptionColorDisplay( box, "TRANSPARENCY_HIGHLIGHT_COLOR" ), 1, 1, 1, 1 );
        colorDisplay->setToolTip( tr( "Background highlight color when entering widget" ) );
        addOptionWidget( colorDisplay );

        // background intensity
        gridLayout->addWidget( new QLabel( tr( "Background intensity:" ), box ), 2, 0, 1, 1 );
        gridLayout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_TINT_INTENSITY" ), 2, 1, 1, 1 );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setSuffix( tr( "%" ) );
        slider->setToolTip( tr( "Transparent background tint intensity" ) );
        addOptionWidget( slider );

        // highlight intensity
        gridLayout->addWidget( new QLabel( tr( "Highlight intensity:" ), box ), 3, 0, 1, 1 );
        gridLayout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_HIGHLIGHT_INTENSITY" ), 3, 1, 1, 1 );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setSuffix( tr( "%" ) );
        slider->setToolTip( tr( "Background highlight intensity when entering widget" ) );
        addOptionWidget( slider );

        // inverse colors
        gridLayout->addWidget( checkbox = new OptionCheckBox( tr( "Inverse colors" ), box, "TRANSPARENCY_INVERSE_COLORS" ), 8, 1, 1, 1 );
        addOptionWidget( checkbox );

        return;
    }
}
