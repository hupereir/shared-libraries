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

#include "BasePrintHelper.h"

#include <QtCore/QTextStream>
#include <QtGui/QButtonGroup>
#include <QtGui/QTextDocument>
#include <QtGui/QGroupBox>
#include <QtGui/QLayout>
#include <QtGui/QRadioButton>

//__________________________________________________________________________
void BasePrintHelper::setupPage( QPrinter* printer )
{

    Debug::Throw( "BasePrintHelper::setupPage.\n" );

    sheetNumber_ = 0;
    pageNumber_ = 0;
    now_ = TimeStamp::now();

    // set printer orientation
    printer->setOrientation( orientation_ );

    // get font
    const QFont font( QTextDocument().defaultFont() );
    const QFontMetrics metrics( font, printer );
    const int leading( metrics.leading() );
    const QRect printerRect( printer->pageRect() );
    const int margin = 1.5*( metrics.height() + leading );

    QRect fullPageRect;
    qreal scale;
    switch( pageMode_ )
    {
        default:
        case SinglePage:
        {
            scale = 1;
            fullPageRect = QRect( QPoint(0,0), printerRect.size() );
            break;
        }

        case TwoPages:
        {
            scale = qreal( printerRect.height() - margin )/(2*printerRect.width() );
            fullPageRect = QRect( 0, 0, printerRect.width(), printerRect.width()/scale );

            // change orientation and define viewports
            printer->setOrientation( orientation_ == QPrinter::Portrait ? QPrinter::Landscape:QPrinter::Portrait );
            const QRect viewport = QRect( 0, 0, scale*printer->pageRect().width(), scale*printer->pageRect().height() );
            pages_.clear();
            pages_.push_back( viewport );
            pages_.push_back( viewport.translated( scale*fullPageRect.width() + margin, 0 ) );
            break;
        }

        case FourPages:
        {
            scale = qreal( printerRect.width() - margin )/(2*printerRect.width() );
            fullPageRect = QRect( 0, 0, printerRect.width(), qreal(printerRect.height()-margin)/(2*scale) );

            // define viewports
            const QRect viewport = QRect( 0, 0, scale*printer->pageRect().width(), scale*printer->pageRect().height() );
            pages_.clear();
            pages_.push_back( viewport );
            pages_.push_back( viewport.translated( scale*fullPageRect.width() + margin, 0 ) );
            pages_.push_back( viewport.translated( 0, scale*fullPageRect.height() + margin ) );
            pages_.push_back( viewport.translated( scale*fullPageRect.width() + margin, scale*fullPageRect.height() + margin ) );
            break;
        }

    }

    // calculate header, footers and printable pageRect based on fullPageRect
    const int headerHeight( ( metrics.height() + leading ) * 1.5 );
    const int footerHeight( ( metrics.height() + leading ) * 1.5 );
    headerRect_ = QRect( 0, 0, fullPageRect.width(), headerHeight );
    footerRect_ = QRect( 0, fullPageRect.height() - footerHeight, fullPageRect.width(), footerHeight );
    pageRect_ = QRect( headerRect_.bottomLeft(), footerRect_.topRight() );
    pageRect_.adjust( 0, headerRect_.height()/2, 0, -footerRect_.height()/2 );

    return;

}

//__________________________________________________________________________
void BasePrintHelper::_newPage( QPrinter* printer, QPainter* painter )
{

    Debug::Throw( "BasePrintHelper::_newPage.\n" );

    // increment printer page
    switch( pageMode_ )
    {

        default:
        case SinglePage:
        if( pageNumber_ > 0 ) printer->newPage();
        sheetNumber_++;
        break;

        case TwoPages:
        if( pageNumber_ > 0 && pageNumber_%2 == 0 ) printer->newPage();
        if( pageNumber_%2 == 0 ) sheetNumber_++;
        painter->setViewport( pages_[pageNumber_%2] );
        break;

        case FourPages:
        if( pageNumber_ > 0 && pageNumber_%4 == 0 ) printer->newPage();
        if( pageNumber_%4 == 0 ) sheetNumber_++;
        painter->setViewport( pages_[pageNumber_%4] );
        break;
    }

    // increment page number
    pageNumber_++;

    // setup painter
    painter->save();
    painter->setPen( QColor( "#888888" ) );

    // header
    painter->drawLine( headerRect_.bottomLeft()+QPoint(0,1), headerRect_.bottomRight()+QPoint(0,1) );
    painter->drawText( headerRect_, Qt::AlignVCenter|Qt::AlignLeft, TimeStamp::now().toString( TimeStamp::DATE ) );
    painter->drawText( headerRect_, Qt::AlignVCenter|Qt::AlignRight, QString().setNum( pageNumber_ ) );

    if( !file_.isEmpty() )
    { painter->drawText( headerRect_, Qt::AlignCenter, file_.localName() ); }

    // footer
    painter->drawLine( footerRect_.topLeft()-QPoint(0,1), footerRect_.topRight()-QPoint(0,1) );
    painter->drawText( footerRect_, Qt::AlignVCenter|Qt::AlignRight, QString().setNum( pageNumber_ ) );
    if( !file_.isEmpty() )
    { painter->drawText( footerRect_, Qt::AlignVCenter|Qt::AlignLeft, file_ ); }

    // restore
    painter->restore();

    emit pageCountChanged( sheetNumber_ );

}

//________________________________________________________________
namespace PRINT
{

    //________________________________________________________________
    PrinterOptionWidget::PrinterOptionWidget( QWidget* parent ):
        QWidget( parent ),
        Counter( "PRINT::PrinterOptionWidget" )
    {

        setWindowTitle( "Pages" );
        QHBoxLayout* layout = new QHBoxLayout();
        setLayout( layout );

        QGroupBox* groupBox;
        {
            layout->addWidget( groupBox = new QGroupBox( "Orientation", this ) );
            QVBoxLayout* vLayout = new QVBoxLayout();
            groupBox->setLayout( vLayout );

            QRadioButton* radioButton;
            QButtonGroup* buttonGroup = new QButtonGroup( this );
            vLayout->addWidget( radioButton = new QRadioButton( "Portrait", groupBox ) );
            buttonGroup->addButton( radioButton, 0 );
            radioButton->setChecked( true );

            vLayout->addWidget( radioButton = new QRadioButton( "Landscape", groupBox ) );
            buttonGroup->addButton( radioButton, 1 );

            connect( buttonGroup, SIGNAL( buttonClicked( int ) ), SLOT( _setOrientation( int ) ) );

            vLayout->addStretch();
        }

        {
            layout->addWidget( groupBox = new QGroupBox( "Pages Per Sheet", this ) );
            QVBoxLayout* vLayout = new QVBoxLayout();
            groupBox->setLayout( vLayout );

            QRadioButton* radioButton;
            QButtonGroup* buttonGroup = new QButtonGroup( this );
            vLayout->addWidget( radioButton = new QRadioButton( "One page per sheet", groupBox ) );
            buttonGroup->addButton( radioButton, 0 );
            radioButton->setChecked( true );

            vLayout->addWidget( radioButton = new QRadioButton( "Two pages per sheet", groupBox ) );
            buttonGroup->addButton( radioButton, 1 );

            vLayout->addWidget( radioButton = new QRadioButton( "Four pages per sheet", groupBox ) );
            buttonGroup->addButton( radioButton, 2 );

            connect( buttonGroup, SIGNAL( buttonClicked( int ) ), SLOT( _setPageMode( int ) ) );

            vLayout->addStretch();
        }


    }


    //_________________________________________________________________
    void PrinterOptionWidget::_setOrientation( int value )
    {
        if( value == 0 ) emit orientationChanged( QPrinter::Portrait );
        else if( value == 1 ) emit orientationChanged( QPrinter::Landscape );
    }

    //_________________________________________________________________
    void PrinterOptionWidget::_setPageMode( int value )
    {
        if( value == 0 ) emit pageModeChanged( BasePrintHelper::SinglePage );
        else if( value == 1 ) emit pageModeChanged( BasePrintHelper::TwoPages );
        else if( value == 2 ) emit pageModeChanged( BasePrintHelper::FourPages );
    }


}
