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
****************************************************************************/

#include "PrinterOptionWidget.h"

#include "BaseIcons.h"
#include "IconEngine.h"
#include "PrintPreviewDialog.h"

#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QLayout>
#include <QtGui/QRadioButton>

//________________________________________________________________
PrinterOptionWidget::PrinterOptionWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "PRINT::PrinterOptionWidget" ),
    helper_( 0 ),
    orientation_( QPrinter::Portrait ),
    pageMode_( BasePrintHelper::SinglePage )
{

    setWindowTitle( "Pages" );
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    layout->addLayout( hLayout );

    QGroupBox* groupBox;
    {
        hLayout->addWidget( groupBox = new QGroupBox( "Orientation", this ) );
        QVBoxLayout* vLayout = new QVBoxLayout();
        groupBox->setLayout( vLayout );

        QRadioButton* radioButton;
        QButtonGroup* buttonGroup = new QButtonGroup( this );
        vLayout->addWidget( radioButton = new QRadioButton( "Portrait", groupBox ) );
        orientationButtons_.insert( radioButton, QPrinter::Portrait );
        buttonGroup->addButton( radioButton );

        radioButton->setChecked( true );

        vLayout->addWidget( radioButton = new QRadioButton( "Landscape", groupBox ) );
        orientationButtons_.insert( radioButton, QPrinter::Landscape );
        buttonGroup->addButton( radioButton );

        connect( buttonGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), SLOT( _setOrientation( QAbstractButton* ) ) );

        vLayout->addStretch();
    }

    {
        hLayout->addWidget( groupBox = new QGroupBox( "Pages Per Sheet", this ) );
        QVBoxLayout* vLayout = new QVBoxLayout();
        groupBox->setLayout( vLayout );

        QRadioButton* radioButton;
        QButtonGroup* buttonGroup = new QButtonGroup( this );
        vLayout->addWidget( radioButton = new QRadioButton( "One page per sheet", groupBox ) );
        pageModeButtons_.insert( radioButton, BasePrintHelper::SinglePage );
        buttonGroup->addButton( radioButton );
        radioButton->setChecked( true );

        vLayout->addWidget( radioButton = new QRadioButton( "Two pages per sheet", groupBox ) );
        pageModeButtons_.insert( radioButton, BasePrintHelper::TwoPages );
        buttonGroup->addButton( radioButton );

        vLayout->addWidget( radioButton = new QRadioButton( "Four pages per sheet", groupBox ) );
        pageModeButtons_.insert( radioButton, BasePrintHelper::FourPages );
        buttonGroup->addButton( radioButton );

        connect( buttonGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), SLOT( _setPageMode( QAbstractButton* ) ) );

        vLayout->addStretch();
    }

    hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    layout->addLayout( hLayout );
    hLayout->addStretch( 1 );
    hLayout->addWidget( previewButton_ = new QPushButton( IconEngine::get( ICONS::PRINT_PREVIEW ), "Preview", this ) );
    connect( previewButton_, SIGNAL( clicked( void ) ), SLOT( _preview( void ) ) );

    previewButton_->setEnabled( false );

}

//_________________________________________________________________
void PrinterOptionWidget::_setOrientation( QAbstractButton* button )
{
    orientation_ = orientationButtons_[button];
    emit orientationChanged( orientation() );
}

//_________________________________________________________________
void PrinterOptionWidget::_setPageMode( QAbstractButton* button )
{
    pageMode_ = pageModeButtons_[button];
    emit pageModeChanged( pageMode() );
}

//_________________________________________________________________
void PrinterOptionWidget::_preview( void )
{

    Debug::Throw( "PrinterOptionWidget::_preview" );
    if( !helper_ ) return;

    // create dialog, connect and execute
    PrintPreviewDialog dialog( this );
    dialog.setWindowTitle( "Print Preview - elogbook" );
    dialog.setHelper( helper_ );
    dialog.hideMenu();
    dialog.exec();

}
