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
*******************************************************************************/

#include "IconViewItem.h"

#include <QApplication>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QTextLayout>

//____________________________________________________________________
/*!
   copied from kdelibs/kdecore/text/kstringhandler.cpp
   Copyright (C) 1999 Ian Zepp (icszepp@islc.net)
   Copyright (C) 2006 by Dominic Battre <dominic@battre.de>
   Copyright (C) 2006 by Martin Pool <mbp@canonical.com>
*/
QString preProcessWrap(const QString &text)
{

    // break
    const QChar brk(0x200b);

    // word join
    const QChar join(0x2060);

    QString result;
    result.reserve(text.length());

    for (int i = 0; i < text.length(); i++)
    {

        const QChar c = text[i];
        const bool openingParenthesis( (c == QLatin1Char('(') || c == QLatin1Char('{') || c == QLatin1Char('[')) );
        const bool closingParenthesis( (c == QLatin1Char(')') || c == QLatin1Char('}') || c == QLatin1Char(']')) );
        const bool singleQuote( (c == QLatin1Char('\'') ) );
        const bool breakAfter( (closingParenthesis || c.isPunct() || c.isSymbol()) );
        const bool nextIsSpace( (i == (text.length() - 1) || text[i + 1].isSpace()) );
        const bool prevIsSpace( (i == 0 || text[i - 1].isSpace() || result[result.length() - 1] == brk) );

        // Provide a breaking opportunity before opening parenthesis
        if( openingParenthesis && !prevIsSpace ) result += brk;

        // Provide a word joiner before the single quote
        if( singleQuote && !prevIsSpace ) result += join;

        result += c;

        if( breakAfter && !openingParenthesis && !nextIsSpace && !singleQuote )
        { result += brk; }

    }

    return result;
}

//____________________________________________________________________
int IconViewItem::margin( 5 );
int IconViewItem::spacing( 5 );
int IconViewItem::maxTextWidth( 100 );

//____________________________________________________________________
QRect IconViewItem::boundingRect( void ) const
{
    if( dirty_ ) const_cast<IconViewItem*>( this )->_updateBoundingRect();
    return boundingRect_;
}

//____________________________________________________________________
void IconViewItem::paint( QPainter* painter, const QStyleOption* option, QWidget* widget ) const
{

    QRectF boundingRect( this->boundingRect() );
    QRectF textRect( boundingRect.adjusted( 0, margin, 0, -margin ) );

    // draw selection
    widget->style()->drawPrimitive( QStyle::PE_PanelItemViewItem, option, painter, widget );

    if( !pixmap_.isNull() )
    {
        painter->drawPixmap( QPointF( (boundingRect.width() - pixmap_.width())/2, margin ), pixmap_ );
        textRect.adjust( 0, pixmap_.height() + spacing, 0, 0 );
    }

    if( !text_.isEmpty() )
    {

        const QString text( preProcessWrap( text_ ) );

        const int maxWidth( qMax( maxTextWidth, pixmap_.width() ) );
        QTextOption textOption(Qt::AlignHCenter);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

        qreal height(0);
        QTextLine line;
        QTextLayout layout( text, static_cast<const QStyleOptionViewItem*>(option)->font );
        layout.setTextOption(textOption);
        layout.beginLayout();
        while( ( line = layout.createLine() ).isValid())
        {

            line.setLineWidth(maxWidth);
            line.naturalTextWidth();
            line.setPosition(QPointF(0, height));
            height += line.height();

        }

        layout.endLayout();

        // save old pen
        const QPen oldPen( painter->pen() );

        // set correct foreground color and render text
        painter->setPen( option->palette.color(
            option->state & QStyle::State_Active ? QPalette::Active:QPalette::Inactive,
            option->state&QStyle::State_Selected ? QPalette::HighlightedText:QPalette::Text ) );
        layout.draw( painter, textRect.topLeft() + QPointF( 0.5*(textRect.width()-layout.boundingRect().width()), 0 ), QVector<QTextLayout::FormatRange>(), textRect );

        // restore old pen
        painter->setPen( oldPen );

    }

}

//____________________________________________________________________
void IconViewItem::_updateBoundingRect( void )
{
    boundingRect_ = QRect( 0, 0, 2*margin, 2*margin );

    // calculate pixmap size
    QSize pixmapSize( pixmap_.size() );

    // calculate text size
    QSize textSize;
    if( !text_.isEmpty() )
    {

        const QString text( preProcessWrap( text_ ) );
        const int maxWidth( qMax( maxTextWidth, pixmapSize.width() ) );
        int maxLineWidth( 0 );

        QTextOption textOption(Qt::AlignHCenter);
        textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        qreal height(0);

        QTextLine line;
        QTextLayout layout( text, QApplication::font() );
        layout.setTextOption(textOption);
        layout.beginLayout();
        while( ( line = layout.createLine() ).isValid())
        {

            line.setLineWidth(maxWidth);
            maxLineWidth = qMax( maxLineWidth, (int) line.naturalTextWidth() );
            line.setPosition( QPointF( 0, height ) );
            height += line.height();

        }

        layout.endLayout();
        textSize = QSize( maxLineWidth, layout.boundingRect().size().toSize().height() );

    }

    if( !( pixmap_.isNull() || text_.isEmpty() ) )
    {

        boundingRect_.adjust( 0, 0, qMax( pixmapSize.width(), textSize.width() ), spacing + pixmapSize.height() + textSize.height() );

    } else if( !pixmap_.isNull() ) {

        boundingRect_.adjust( 0, 0, pixmapSize.width(), pixmapSize.height() );

    } else if( !text_.isEmpty() ) {

        boundingRect_.adjust( 0, 0, textSize.width(), textSize.height() );

    }

    dirty_ = false;
}
