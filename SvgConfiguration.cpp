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

#include "SvgConfiguration.h"

#include "CustomDialog.h"
#include "Debug.h"
#include "GridLayout.h"
#include "OptionListBox.h"
#include "OptionCheckBox.h"
#include "OptionComboBox.h"
#include "OptionSpinBox.h"
#include "XmlOptions.h"

#include <QLabel>

namespace SVG
{

    //______________________________________________________________________
    SvgConfiguration::SvgConfiguration( QWidget* parent ):
        QWidget( parent ),
        Counter( "SVG::SvgConfiguration" )
    {

        Debug::Throw( "SvgConfiguration::SvgConfiguration.\n" );

        setLayout( new QVBoxLayout() );
        layout()->setSpacing(2);
        layout()->setMargin(0);

        QWidget* box;
        layout()->addWidget( box = new QWidget( this ) );
        box->setLayout( new QVBoxLayout() );
        box->layout()->setSpacing(5);
        box->layout()->setMargin(0);

        box->layout()->addWidget( svgCheckBox_ = new OptionCheckBox( tr( "Use svg background" ), box, "USE_SVG" ));
        svgCheckBox_->setToolTip( tr( "Use svg to paint background" ) );
        addOptionWidget( svgCheckBox_ );

        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        hLayout->setSpacing(5);
        box->layout()->addItem( hLayout );

        // plasma interface
        #if defined(Q_OS_LINUX)
        QWidget* plasmaBox;
        layout()->addWidget( plasmaBox = new QWidget( this ) );
        plasmaBox->setLayout( new QVBoxLayout() );
        plasmaBox->layout()->setSpacing(5);
        plasmaBox->layout()->setMargin(0);

        plasmaBox->layout()->addWidget( plasmaCheckBox_ = new OptionCheckBox( tr( "Use plasma interface (KDE only)" ), plasmaBox, "SVG_USE_PLASMA_INTERFACE" ) );
        plasmaCheckBox_->setToolTip( tr( "If checked, the KDE plasma theme is used for this application theme" ) );
        addOptionWidget( plasmaCheckBox_ );

        QWidget* plasmaOptionBox;
        plasmaBox->layout()->addWidget( plasmaOptionBox = new QWidget( plasmaBox ) );
        plasmaOptionBox->setLayout( new QVBoxLayout() );
        plasmaOptionBox->layout()->setSpacing(5);
        plasmaOptionBox->layout()->setMargin(0);

        OptionCheckBox* checkbox;
        plasmaOptionBox->layout()->addWidget( checkbox = new OptionCheckBox( tr( "Draw overlay" ), plasmaOptionBox, "SVG_DRAW_OVERLAY" ) );
        checkbox->setToolTip( tr( "If checked, overlay pictures, if any, found in the SVG file, are drawn on top of the background" ) );
        addOptionWidget( checkbox );

        hLayout = new QHBoxLayout();
        hLayout->setMargin(0);
        hLayout->setSpacing(5);
        plasmaOptionBox->layout()->addItem(hLayout);
        hLayout->addWidget( new QLabel( tr( "Plasma background image path:" ), plasmaOptionBox ) );

        OptionComboBox* plasmaImagePath;
        hLayout->addWidget( plasmaImagePath = new OptionComboBox( plasmaOptionBox, "SVG_PLASMA_IMAGE_PATH" ) );
        plasmaImagePath->setUseValue( false );
        plasmaImagePath->addItems( QStringList()
            << "dialogs/background"
            << "widgets/background"
            << "widgets/translucentbackground" );
        plasmaImagePath->setToolTip( tr( "Relative path of the svg file used for the background" ) );
        addOptionWidget( plasmaImagePath );
        hLayout->addStretch(1);
        #endif

        // SVG file
        layout()->addWidget( fileListBox_ = new QWidget( this ) );
        fileListBox_->setLayout( new QVBoxLayout() );
        fileListBox_->layout()->setSpacing(5);
        fileListBox_->layout()->setMargin(0);

        fileListBox_->layout()->addWidget( new QLabel( tr( "SVG Files:" ), fileListBox_ ) );

        OptionListBox *listbox = new OptionListBox( fileListBox_, "SVG_BACKGROUND" );
        listbox->setBrowsable( true );
        listbox->setToolTip( tr( "Pathname to load background svg" ) );
        fileListBox_->layout()->addWidget( listbox );
        addOptionWidget( listbox );

        // initiali setup and connections
        svgCheckBox_->setChecked( false );
        fileListBox_->setEnabled( false );

        #if defined(Q_OS_LINUX)
        plasmaCheckBox_->setChecked( false );
        plasmaOptionBox->setEnabled( false );
        plasmaBox->setEnabled( false );
        connect( plasmaCheckBox_, SIGNAL( toggled( bool ) ), SLOT( _enableSvgFileList( void ) ) );
        connect( plasmaCheckBox_, SIGNAL( toggled( bool ) ), plasmaOptionBox, SLOT( setEnabled( bool ) ) );
        connect( svgCheckBox_, SIGNAL( toggled( bool ) ), plasmaBox, SLOT( setEnabled( bool ) ) );
        #endif

        connect( svgCheckBox_, SIGNAL( toggled( bool ) ), SLOT( _enableSvgFileList( void ) ) );


    }

    //______________________________________________________________________
    void SvgConfiguration::_enableSvgFileList( void )
    {
        bool enabled( svgCheckBox_->isChecked() );
        #if defined(Q_OS_LINUX)
        enabled &= !plasmaCheckBox_->isChecked();
        #endif
        fileListBox_->setEnabled( enabled );
    }

}
