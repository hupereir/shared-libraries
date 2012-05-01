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
   \file SvgConfiguration.cpp
   \brief   Svg common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

#include "CustomDialog.h"
#include "Debug.h"
#include "GridLayout.h"
#include "OptionListBox.h"
#include "OptionCheckBox.h"
#include "OptionComboBox.h"
#include "OptionSpinBox.h"
#include "SvgConfiguration.h"
#include "XmlOptions.h"



namespace SVG
{
  //___________________________________________
  SvgConfiguration::SvgConfiguration( QWidget* parent ):
    QWidget( parent ),
    Counter( "SvgConfiguration" )
  {

    Debug::Throw( "SvgConfiguration::SvgConfiguration.\n" );

    setLayout( new QVBoxLayout() );
    layout()->setSpacing(2);
    layout()->setMargin(0);

    QGroupBox* box;
    layout()->addWidget( box = new QGroupBox( "General", this ) );
    box->setLayout( new QVBoxLayout() );
    box->layout()->setSpacing(5);
    box->layout()->setMargin(5);

    OptionCheckBox* svg_checkbox;
    box->layout()->addWidget( svg_checkbox = new OptionCheckBox( "Use svg background", box, "USE_SVG" ));
    svg_checkbox->setToolTip( "Use svg to paint background" );
    addOptionWidget( svg_checkbox );

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    box->layout()->addItem( hLayout );

    OptionSpinBox* spinbox;
    hLayout->addWidget( new QLabel( "Offset: ", box ) );
    hLayout->addWidget( spinbox = new OptionSpinBox( box, "SVG_OFFSET" ) );
    hLayout->addStretch();
    spinbox->setMinimum( -16 );
    spinbox->setMaximum( 16 );
    spinbox->setToolTip(
      "Offset used to draw svg.\n"
      "Positive offset will make the SVG larger than the\n"
      "actual window size, thus shinking its edges\n" );
    addOptionWidget( spinbox );

    // plasma interface
    #if !defined(Q_WS_WIN)
    QGroupBox* plasma_box;
    layout()->addWidget( plasma_box = new QGroupBox( "Plasma Interface (KDE only)", this ) );
    plasma_box->setLayout( new QVBoxLayout() );
    plasma_box->layout()->setSpacing(5);
    plasma_box->layout()->setMargin(5);

    OptionCheckBox* plasma_checkbox;
    plasma_box->layout()->addWidget( plasma_checkbox = new OptionCheckBox( "Use plasma interface", plasma_box, "SVG_USE_PLASMA_INTERFACE" ) );
    plasma_checkbox->setToolTip( "If checked, the KDE plasma theme is used for this application theme" );
    addOptionWidget( plasma_checkbox );

    OptionCheckBox* checkbox;
    plasma_box->layout()->addWidget( checkbox = new OptionCheckBox( "Draw overlay", plasma_box, "SVG_DRAW_OVERLAY" ) );
    checkbox->setToolTip( "If checked, overlay pictures, if any, found in the SVG file, are drawn on top of the background" );
    addOptionWidget( checkbox );

    hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    plasma_box->layout()->addItem(hLayout);
    hLayout->addWidget( new QLabel( "Plasma background image path: ", plasma_box ) );

    OptionComboBox* plasmaImagePath;
    hLayout->addWidget( plasmaImagePath = new OptionComboBox( plasma_box, "SVG_PLASMA_IMAGE_PATH" ) );
    plasmaImagePath->addItems( QStringList()
      << "dialogs/background"
      << "widgets/background"
      << "widgets/translucentbackground" );
    plasmaImagePath->setToolTip( "Relative path of the svg file used for the background" );
    addOptionWidget( plasmaImagePath );
    #endif

    // SVG file
    QGroupBox* fileListBox;
    layout()->addWidget( fileListBox = new QGroupBox( "Svg file", this ) );
    fileListBox->setLayout( new QVBoxLayout() );
    fileListBox->layout()->setSpacing(5);
    fileListBox->layout()->setMargin(5);

    OptionListBox *listbox = new OptionListBox( fileListBox, "SVG_BACKGROUND" );
    listbox->setBrowsable( true );
    listbox->setToolTip( "Pathname to load background svg" );
    fileListBox->layout()->addWidget( listbox );
    addOptionWidget( listbox );

    // initiali setup and connections
    svg_checkbox->setChecked( false );
    fileListBox->setEnabled( false );

    #if !defined(Q_WS_WIN)
    plasma_checkbox->setChecked( false );
    plasma_box->setEnabled( false );
    connect( plasma_checkbox, SIGNAL( toggled( bool ) ), fileListBox, SLOT( setDisabled( bool ) ) );
    connect( svg_checkbox, SIGNAL( toggled( bool ) ), plasma_box, SLOT( setEnabled( bool ) ) );
    #endif

    connect( svg_checkbox, SIGNAL( toggled( bool ) ), fileListBox, SLOT( setEnabled( bool ) ) );


  }

};
