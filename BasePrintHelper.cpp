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

#include "BasePrintHelper.h"

#include <QTextStream>
#include <QButtonGroup>
#include <QTextDocument>
#include <QGroupBox>
#include <QLayout>
#include <QRadioButton>

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
            pages_ << viewport;
            pages_ << viewport.translated( scale*fullPageRect.width() + margin, 0 );
            break;
        }

        case FourPages:
        {
            scale = qreal( printerRect.width() - margin )/(2*printerRect.width() );
            fullPageRect = QRect( 0, 0, printerRect.width(), qreal(printerRect.height()-margin)/(2*scale) );

            // define viewports
            const QRect viewport = QRect( 0, 0, scale*printer->pageRect().width(), scale*printer->pageRect().height() );
            pages_.clear();
            pages_ << viewport;
            pages_ << viewport.translated( scale*fullPageRect.width() + margin, 0 );
            pages_ << viewport.translated( 0, scale*fullPageRect.height() + margin );
            pages_ << viewport.translated( scale*fullPageRect.width() + margin, scale*fullPageRect.height() + margin );
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
    painter->drawText( headerRect_, Qt::AlignVCenter|Qt::AlignLeft, TimeStamp::now().toString( TimeStamp::Date ) );
    painter->drawText( headerRect_, Qt::AlignVCenter|Qt::AlignRight, QString::number( pageNumber_ ) );

    if( !file_.isEmpty() )
    { painter->drawText( headerRect_, Qt::AlignCenter, file_.localName() ); }

    // footer
    painter->drawLine( footerRect_.topLeft()-QPoint(0,1), footerRect_.topRight()-QPoint(0,1) );
    painter->drawText( footerRect_, Qt::AlignVCenter|Qt::AlignRight, QString::number( pageNumber_ ) );
    if( !file_.isEmpty() )
    { painter->drawText( footerRect_, Qt::AlignVCenter|Qt::AlignLeft, file_ ); }

    // restore
    painter->restore();

    emit pageCountChanged( sheetNumber_ );

}
