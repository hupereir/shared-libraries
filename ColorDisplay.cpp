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
*
*******************************************************************************/

/*!
\file ColorDisplay.cpp
\brief used to display colors and a choose button
\author Hugo Pereira
\version $Revision$
\date $Date$
*/



#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QToolButton>

#include "BaseIcons.h"
#include "ColorGrabButton.h"
#include "ColorDisplay.h"
#include "Debug.h"
#include "IconEngine.h"
#include "InformationDialog.h"



//______________________________________________
const QString ColorDisplay::NONE = "None";

//______________________________________________
ColorDisplay::ColorDisplay( QWidget* parent ):
    QWidget( parent ),
    Counter( "ColorDisplay" ),
    editor_( this )
{
    Debug::Throw( "ColorDisplay::ColorDisplay.\n" );

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    setLayout( layout );

    editor_.setHasClearButton( false );
    editor_.setAlignment( Qt::AlignCenter );
    layout->addWidget( &editor_, 1 );
    connect( &editor_, SIGNAL( returnPressed() ), SLOT( _selectColorFromText() ) );

    // browse button
    QToolButton *button;
    layout->addWidget( button = new QToolButton( this ), 0 );
    button->setAutoRaise( true );
    button->setIcon( IconEngine::get( ICONS::OPEN ) );
    button->setToolTip( "Select color from dialog" );
    connect( button, SIGNAL( clicked( void ) ), SLOT( _selectColorFromDialog( void ) ) );

    // grab button
    layout->addWidget( grabButton_ = new ColorGrabButton( this ), 0 );
    grabButton_->setAutoRaise( true );
    grabButton_->setIcon( IconEngine::get( ICONS::COLOR_PICKER ) );
    grabButton_->setToolTip( "Grap color from screen" );
    connect( grabButton_, SIGNAL( colorSelected( QString ) ), SLOT( setColor( QString ) ) );

}

//________________________________________________________
void ColorDisplay::setColor( QString color )
{
    Debug::Throw( "ColorDisplay::setColor.\n" );
    editor_.setText( color );
    editor_.setColor( color == NONE ? palette().color( QPalette::Window ):QColor(color) );
}

//________________________________________________________
void ColorDisplay::_selectColorFromDialog( void )
{
    Debug::Throw( "ColorDisplay::_selectColor.\n" );
    QColor color( QColorDialog::getColor( editor_.color(), this ) );
    if( color.isValid() ) setColor( color.name() );
}

//________________________________________________________
void ColorDisplay::_selectColorFromText( void )
{
    Debug::Throw( "ColorDisplay::_selectColorFromText.\n" );

    QColor color;
    QString text = editor_.text();

    if( text != NONE )
    {
        color = QColor( text );
        if( !color.isValid() )
        {

            QString buffer;
            QTextStream what( &buffer );
            if( !( text.isNull() || text.isEmpty() ) ) what << "Invalid color: " << text;
            else what << "Invalid color";

            InformationDialog( this,buffer ).exec();
            editor_.setText( NONE );
        }
    }

    if( color.isValid() )
    {

        editor_.setColor( color );
        editor_.setText( color.name() );

    } else editor_.setColor( palette().color( QPalette::Window ) );

}

//_______________________________________________
QColor ColorDisplay::LocalLineEdit::color( void ) const
{
    Debug::Throw( "ColorDisplay::LocalLineEdit::color.\n" );
    QString text( AnimatedLineEditor::text() );
    if( text == ColorDisplay::NONE || text.isEmpty() ) return QColor();
    else return QColor( text );
}

//_________________________________________________________
void ColorDisplay::LocalLineEdit::setColor( QColor color )
{
    Debug::Throw( "ColorDisplay::LocalLineEdit::setColor.\n" );

    // check color validity
    if( !color.isValid() )
    {

        QString buffer;
        QTextStream what( &buffer );
        if( !( text().isNull() || text().isEmpty() ) ) what << "Invalid color: " << text();
        else what << "Invalid color";

        InformationDialog( this, buffer ).exec();

        // update text
        setText( NONE );

        // reassign color
        color = QLineEdit().palette().color( QPalette::Base );

    }

    // retrieve palette, change background/foreground
    QPalette palette( ColorDisplay::LocalLineEdit::palette() );
    palette.setColor( QPalette::Base, color );
    palette.setColor( QPalette::Text, (color.value() >= 175) ? Qt::black:Qt::white );

    // update
    setPalette(palette);
    update();

}
