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
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
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

namespace TRANSPARENCY
{

    //________________________________________________________________________
    TransparencyConfiguration::TransparencyConfiguration( QWidget* parent ):
        QWidget( parent ),
        Counter( "TRANSPARENCY::TransparencyConfiguration" )
    {
        Debug::Throw( "TransparencyConfiguration::TransparencyConfiguration.\n" );

        setLayout( new QVBoxLayout() );
        layout()->setSpacing(5);
        layout()->setMargin(0);

        // generic objects
        QGroupBox *box;
        OptionColorDisplay* color_display;
        OptionSlider* slider;
        OptionSpinBox* spinbox;
        OptionCheckBox* checkbox;

        // general
        layout()->addWidget( box = new QGroupBox( "General", this ) );
        box->setLayout( new QVBoxLayout() );
        box->layout()->setSpacing(5);
        box->layout()->setMargin(5);

        // enable/disable transparency
        box->layout()->addWidget( checkbox = new OptionCheckBox( "Use transparency", box, "TRANSPARENT" ) );
        checkbox->setToolTip( "Enable/disable transparent background" );
        addOptionWidget( checkbox );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setSpacing(5);
        hLayout->setMargin(0);
        box->layout()->addItem( hLayout );

        box->layout()->addWidget( checkbox = new OptionCheckBox( "Inverse colors", box, "TRANSPARENCY_INVERSE_COLORS" ) );
        addOptionWidget( checkbox );

        // colors
        layout()->addWidget( box = new QGroupBox( "Colors", this ) );

        GridLayout* gridLayout = new GridLayout();
        gridLayout->setSpacing(5);
        gridLayout->setMargin(5);
        gridLayout->setMaxCount( 2 );
        gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
        box->setLayout( gridLayout );

        // foreground color
        gridLayout->addWidget( new QLabel("Foreground color:", box ) );
        gridLayout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_FOREGROUND_COLOR" ) );
        color_display->setToolTip( "Text/display foreground color" );
        addOptionWidget( color_display );

        // shadow color
        gridLayout->addWidget( new QLabel( "Shadow color:", box ) );
        gridLayout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_SHADOW_COLOR" ) );
        color_display->setToolTip( "Text/display shadow color" );
        addOptionWidget( color_display );

        // tint color
        gridLayout->addWidget( new QLabel( "Tint color:", box ) );
        gridLayout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_TINT_COLOR" ) );
        color_display->setToolTip( "Transparent background tint color" );
        addOptionWidget( color_display );

        // highlight color
        gridLayout->addWidget( new QLabel( "Highlight color:", box ) );
        gridLayout->addWidget( color_display = new OptionColorDisplay( box, "TRANSPARENCY_HIGHLIGHT_COLOR" ) );
        color_display->setToolTip( "Highlight color when entering widget" );
        addOptionWidget( color_display );

        // shadow offset
        gridLayout->addWidget( new QLabel( "Shadow offset: ", box ) );
        gridLayout->addWidget( spinbox = new OptionSpinBox( box, "TRANSPARENCY_SHADOW_OFFSET" ) );
        spinbox->setMinimum( 0 );
        spinbox->setMaximum( 10 );
        spinbox->setToolTip(
            "Offset between text shadow and text.\n"
            "0 means no shadow." );
        addOptionWidget( spinbox );

        // foreground intensity
        gridLayout->addWidget( new QLabel("Foreground intensity:", box ) );
        gridLayout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_FOREGROUND_INTENSITY" ) );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setToolTip("Foreground color intensity" );
        addOptionWidget( slider );

        // tint intensity
        gridLayout->addWidget( new QLabel( "Tint intensity:", box ) );
        gridLayout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_TINT_INTENSITY" ) );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setToolTip( "Transparent background tint intensity" );
        addOptionWidget( slider );

        // tint intensity
        gridLayout->addWidget( new QLabel( "Highlight intensity:", box ) );
        gridLayout->addWidget( slider = new OptionSlider( box, "TRANSPARENCY_HIGHLIGHT_INTENSITY" ) );
        slider->setScale( 1.0/2.55 );
        slider->setRange( 0, 100 );
        slider->setToolTip( "Transparent background highlight intensity" );
        addOptionWidget( slider );

        return;
    }
}
