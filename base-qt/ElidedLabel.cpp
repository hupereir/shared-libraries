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

#include "Debug.h"
#include "ElidedLabel.h"

#include <QApplication>
#include <QClipboard>
#include <QRegularExpression>
#include <QStringList>

//___________________________________________________
ElidedLabel::ElidedLabel(  QWidget* parent ):
    QLabel( parent ),
    Counter( "ElidedLabel" )
{ setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ); }

//___________________________________________________
ElidedLabel::ElidedLabel( const QString& text, QWidget* parent ):
    ElidedLabel( parent )
{ setText( text ); }

//___________________________________________________
void ElidedLabel::setText( const QString& text )
{
    fullText_ = text;
    updateElidedText();
}

//___________________________________________________
void ElidedLabel::clear()
{
    Debug::Throw( "ElidedLabel::clear.\n" );
    fullText_.clear();
    QLabel::clear();
}

//___________________________________________________
QSize ElidedLabel::minimumSizeHint() const
{ return QSize( -1, QLabel::minimumSizeHint().height() ); }

//___________________________________________________
QSize ElidedLabel::sizeHint() const
{ return QSize( fontMetrics().width(fullText_), QLabel::sizeHint().height() ); }

//___________________________________________________
void ElidedLabel::mouseReleaseEvent( QMouseEvent *event )
{
    if(
        QApplication::clipboard()->supportsSelection() &&
        textInteractionFlags() != Qt::NoTextInteraction &&
        event->button() == Qt::LeftButton &&
        !fullText_.isEmpty() &&
        hasSelectedText() )
    {
        // Expand "..." when selecting with the mouse
        QString text = selectedText();
        const QChar ellipsis(0x2026); // from qtextengine.cpp
        const int dotsPos = text.indexOf(ellipsis);

        if (dotsPos > -1)
        {

            // Ex: abcde...yz, selecting de...y  (selectionStart=3)
            // charsBeforeSelection = selectionStart = 2 (ab)
            // charsAfterSelection = 1 (z)
            // final selection length= 26 - 2 - 1 = 23
            const int start = selectionStart();
            int charsAfterSelection = QLabel::text().length() - start - selectedText().length();
            text = fullText_;

            // Strip markup tags
            if (textFormat() == Qt::RichText )
            {

                static const QRegularExpression regExp( QStringLiteral("<[^>]*>") );
                text.replace( regExp, "" );

                // account for stripped characters
                charsAfterSelection -= fullText_.length() - text.length();

            }

            text.remove( 0, selectionStart() );
            text.truncate( text.length() - start - charsAfterSelection );

        }

        // update clipboard
        QApplication::clipboard()->setText( text, QClipboard::Selection );

    } else {

        QLabel::mouseReleaseEvent( event );

    }

}

//___________________________________________________
void ElidedLabel::resizeEvent( QResizeEvent* e )
{
    QLabel::resizeEvent( e );
    updateElidedText();
}

//___________________________________________________
void ElidedLabel::updateElidedText()
{
    if( fullText_.isEmpty() )
    {
        QLabel::clear();
        return;
    }

    static const QRegularExpression regexp( QStringLiteral("(<a href=.+?>)|(</a>)") );

    auto fontMetrics( this->fontMetrics() );
    int labelWidth = size().width();

    QStringList elidedLines;
    bool elided = false;

    for( const auto& fullLine:fullText_.split( QLatin1Char('\n') ) )
    {

        // attempt at dealing with rich text
        QString prefix;
        QString suffix;
        QString line( fullLine );
        switch( elideMode_ )
        {
            case Qt::ElideLeft:
            {
                QRegularExpressionMatch match;
                if( fullLine.indexOf( regexp, 0, &match ) == 0 )
                {
                    prefix = fullLine.left( match.capturedLength() );
                    line = fullLine.mid( match.capturedLength() );
                }
                break;
            }

            case Qt::ElideRight:
            {
                QRegularExpressionMatch match;
                int position( fullLine.lastIndexOf( regexp, -1, &match ) );
                if( position >= 0 && position + match.capturedLength() == fullLine.length() )
                {
                    suffix = fullLine.mid( position );
                    line = fullLine.left( position );
                }
                break;
            }

            default: break;
        }

        int lineWidth = fontMetrics.width(line);
        if (lineWidth > labelWidth)
        {
            elided = true;
            elidedLines.append( prefix + fontMetrics.elidedText(line, elideMode_, labelWidth ) + suffix );

        } else {

            elidedLines.append( fullLine );

        }
    }

    if( elided )
    {

        QLabel::setText(elidedLines.join("\n"));
        setToolTip( fullText_ );

    } else {

        QLabel::setText( fullText_ );
        setToolTip(QString());

    }

}
