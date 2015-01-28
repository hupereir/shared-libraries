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

#include "Debug.h"
#include "ElidedLabel.h"

#include <QApplication>
#include <QClipboard>
#include <QStringList>

//___________________________________________________
ElidedLabel::ElidedLabel( const QString& text, QWidget* parent ):
    QLabel( parent ),
    Counter( "ElidedLabel" ),
    hRefRegExp_( "(<a href=.+>)|(</a>)" ),
    elideMode_( Qt::ElideLeft )
{

    Debug::Throw( "ElidedLabel::ElidedLabel.\n" );
    hRefRegExp_.setMinimal( true );

    // set size policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    // set text
    setText( text );

}

//___________________________________________________
ElidedLabel::ElidedLabel(  QWidget* parent ):
    QLabel( parent ),
    Counter( "ElidedLabel" ),
    hRefRegExp_( "(<a href=.+>)|(</a>)" ),
    elideMode_( Qt::ElideLeft )
{

    Debug::Throw( "ElidedLabel::ElidedLabel.\n" );
    hRefRegExp_.setMinimal( true );

    // set size policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

}

//___________________________________________________
void ElidedLabel::setText( const QString& text )
{
    fullText_ = text;
    updateElidedText();
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
    #if QT_VERSION >= 0x040700
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

                text.replace(QRegExp("<[^>]*>"), "");

                // account for stripped characters
                charsAfterSelection -= fullText_.length() - text.length();

            }

            text = text.mid( selectionStart(), text.length() - start - charsAfterSelection );

        }

        // update clipboard
        QApplication::clipboard()->setText( text, QClipboard::Selection );

    } else
    #endif
    { QLabel::mouseReleaseEvent( event ); }

}

//___________________________________________________
void ElidedLabel::resizeEvent( QResizeEvent* e )
{
    QLabel::resizeEvent( e );
    updateElidedText();
}

//___________________________________________________
void ElidedLabel::updateElidedText( void )
{
    if( fullText_.isEmpty() ) return;

    QFontMetrics fm(fontMetrics());
    int labelWidth = size().width();

    QStringList elidedLines;
    bool elided = false;

    foreach( const QString& fullLine, fullText_.split( '\n' ) )
    {

        // attempt at dealing with rich text
        QString prefix;
        QString suffix;
        QString line( fullLine );
        switch( elideMode_ )
        {
            case Qt::ElideLeft:
            {
                if( fullLine.indexOf( hRefRegExp_ ) == 0 )
                {
                    prefix = fullLine.left( hRefRegExp_.matchedLength() );
                    line = fullLine.mid( hRefRegExp_.matchedLength() );
                }
                break;
            }

            case Qt::ElideRight:
            {
                int position( fullLine.lastIndexOf( hRefRegExp_ ) );
                if( position >= 0 && position + hRefRegExp_.matchedLength() == fullLine.length() )
                {
                    suffix = fullLine.mid( position );
                    line = fullLine.left( position );
                }
                break;
            }

            default: break;
        }

        int lineWidth = fm.width(line);
        if (lineWidth > labelWidth)
        {
            elided = true;
            elidedLines << prefix + fm.elidedText(line, elideMode_, labelWidth ) + suffix;

        } else {

            elidedLines << fullLine;

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
