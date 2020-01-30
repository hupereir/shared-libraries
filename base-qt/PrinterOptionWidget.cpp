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

#include "PrinterOptionWidget.h"

#include "BaseIconNames.h"
#include "IconEngine.h"
#include "PrintPreviewDialog.h"

#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLayout>
#include <QRadioButton>

//________________________________________________________________
PrinterOptionWidget::PrinterOptionWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "Print::PrinterOptionWidget" )
{

    setWindowTitle( tr( "Pages" ) );
    auto layout = new QVBoxLayout;
    setLayout( layout );

    auto hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    layout->addLayout( hLayout );

    QGroupBox* groupBox;
    {
        hLayout->addWidget( groupBox = new QGroupBox( tr( "Orientation" ), this ) );
        QVBoxLayout* vLayout = new QVBoxLayout;
        groupBox->setLayout( vLayout );

        QRadioButton* radioButton;
        QButtonGroup* buttonGroup = new QButtonGroup( this );
        vLayout->addWidget( radioButton = new QRadioButton( tr( "Portrait" ), groupBox ) );
        orientationButtons_.insert( radioButton, QPrinter::Portrait );
        buttonGroup->addButton( radioButton );

        radioButton->setChecked( true );

        vLayout->addWidget( radioButton = new QRadioButton( tr( "Landscape" ), groupBox ) );
        orientationButtons_.insert( radioButton, QPrinter::Landscape );
        buttonGroup->addButton( radioButton );

        connect( buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), SLOT(_setOrientation(QAbstractButton*)) );

        vLayout->addStretch();
    }

    {
        hLayout->addWidget( groupBox = new QGroupBox( tr( "Pages Per Sheet" ), this ) );
        QVBoxLayout* vLayout = new QVBoxLayout;
        groupBox->setLayout( vLayout );

        QRadioButton* radioButton;
        QButtonGroup* buttonGroup = new QButtonGroup( this );
        vLayout->addWidget( radioButton = new QRadioButton( tr( "One page per sheet" ), groupBox ) );
        pageModeButtons_.insert( radioButton, BasePrintHelper::PageMode::SinglePage );
        buttonGroup->addButton( radioButton );
        radioButton->setChecked( true );

        vLayout->addWidget( radioButton = new QRadioButton( tr( "Two pages per sheet" ), groupBox ) );
        pageModeButtons_.insert( radioButton, BasePrintHelper::PageMode::TwoPages );
        buttonGroup->addButton( radioButton );

        vLayout->addWidget( radioButton = new QRadioButton( tr( "Four pages per sheet" ), groupBox ) );
        pageModeButtons_.insert( radioButton, BasePrintHelper::PageMode::FourPages );
        buttonGroup->addButton( radioButton );

        connect( buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), SLOT(_setPageMode(QAbstractButton*)) );

        vLayout->addStretch();
    }

    hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    layout->addLayout( hLayout );
    hLayout->addStretch( 1 );
    hLayout->addWidget( previewButton_ = new QPushButton( IconEngine::get( IconNames::PrintPreview ), tr( "Preview" ), this ) );
    connect( previewButton_, &QAbstractButton::clicked, this, &PrinterOptionWidget::_preview );

    previewButton_->setEnabled( false );

}

//_________________________________________________________________
void PrinterOptionWidget::setHelper( BasePrintHelper* helper )
{
    helper_ = helper;
    previewButton_->setEnabled( helper_ );

    // update orientation
    for( auto&& iter = orientationButtons_.constBegin(); iter != orientationButtons_.constEnd(); ++iter )
    { if( iter.value() == helper_->orientation() ) { iter.key()->setChecked( true ); break; } }

    // update page mode
    for( auto&& iter = pageModeButtons_.constBegin(); iter != pageModeButtons_.constEnd(); ++iter )
    { if( iter.value() == helper_->pageMode() ) { iter.key()->setChecked( true ); break; } }

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
void PrinterOptionWidget::_preview()
{

    Debug::Throw( "PrinterOptionWidget::_preview" );
    if( !helper_ ) return;

    // create dialog, connect and execute
    PrintPreviewDialog dialog( this );
    dialog.setWindowTitle( tr( "Print Preview" ) );
    dialog.setHelper( helper_ );
    dialog.hideMenu();
    dialog.exec();

}
