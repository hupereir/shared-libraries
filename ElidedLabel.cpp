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
#include "ElidedLabel.moc"

#include <QStringList>

//___________________________________________________
ElidedLabel::ElidedLabel( const QString& text, QWidget* parent ):
    QLabel( parent ),
    Counter( "ElidedLabel" ),
    elideMode_( Qt::ElideLeft )
{

    Debug::Throw( "ElidedLabel::ElidedLabel.\n" );

    // set size policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    // set text
    setText( text );

}

//___________________________________________________
ElidedLabel::ElidedLabel(  QWidget* parent ):
    QLabel( parent ),
    Counter( "ElidedLabel" ),
    elideMode_( Qt::ElideLeft )
{

    Debug::Throw( "ElidedLabel::ElidedLabel.\n" );

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

    foreach( const QString& line, fullText_.split( '\n' ) )
    {
        int lineWidth = fm.width(line);
        if (lineWidth > labelWidth)
        {
            elided = true;
            elidedLines << fm.elidedText(line, elideMode_, labelWidth );
        } else {

            elidedLines << line;

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
