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

#include "ColorDisplay.h"

#include "BaseIconNames.h"
#include "ColorGrabButton.h"
#include "Debug.h"
#include "IconEngine.h"
#include "InformationDialog.h"

#include <QApplication>
#include <QCursor>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QPainter>
#include <QToolButton>

//______________________________________________
ColorDisplay::ColorDisplay( QWidget* parent ):
    QWidget( parent ),
    Counter( "ColorDisplay" )
{
    Debug::Throw( "ColorDisplay::ColorDisplay.\n" );

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(2);
    setLayout( layout );

    editor_ = new Editor( this );
    editor_->setHasClearButton( false );
    editor_->setAlignment( Qt::AlignCenter );
    layout->addWidget( editor_, 1 );
    connect( editor_, SIGNAL(returnPressed()), SLOT(_selectColorFromText()) );

    // browse button
    {
        QToolButton *button;
        layout->addWidget( button = new QToolButton( this ), 0 );
        button->setAutoRaise( true );
        button->setIcon( IconEngine::get( IconNames::Open ) );
        button->setToolTip( tr( "Select color from dialog" ) );
        connect( button, SIGNAL(clicked()), SLOT(_selectColorFromDialog()) );
    }

    // grab button
    {
        ColorGrabButton* button;
        layout->addWidget( button = new ColorGrabButton( this ), 0 );
        button->setAutoRaise( true );
        button->setIcon( IconEngine::get( IconNames::SelectColor ) );
        button->setToolTip( tr( "Grab color from screen" ) );
        connect( button, SIGNAL(colorSelected(QColor)), SLOT(setColor(QColor)) );
    }
}

//________________________________________________________
void ColorDisplay::_selectColorFromDialog( void )
{
    Debug::Throw( "ColorDisplay::_selectColor.\n" );
    QColor color( QColorDialog::getColor( editor_->color(), this ) );
    if( color.isValid() ) setColor( color );
}

//________________________________________________________
void ColorDisplay::_selectColorFromText( void )
{
    Debug::Throw( "ColorDisplay::_selectColorFromText.\n" );
    editor_->setColor( editor_->text() );
}

//_______________________________________________
QColor ColorDisplay::Editor::color( void ) const
{
    Debug::Throw( "ColorDisplay::Editor::color.\n" );
    QString text( AnimatedLineEditor::text() );
    return text.isEmpty() ? QColor():QColor( text );
}

//_________________________________________________________
void ColorDisplay::Editor::setColor( QColor color )
{
    Debug::Throw() << "ColorDisplay::Editor::setColor - " << color.name() << endl;

    // check color validity
    if( color.isValid() ) setText( color.name() );
    else {

        QString buffer;
        if( !text().isEmpty() ) buffer = QString( tr( "Invalid color: %1" ) ).arg( text() );
        else buffer = QString( tr( "Invalid color" ) );

        InformationDialog( this, buffer ).exec();

        // update text
        clear();

        // reassign color
        color = QLineEdit().palette().color( QPalette::Window );

    }

    // retrieve palette, change background/foreground
    QPalette palette( this->palette() );
    palette.setColor( QPalette::Active, QPalette::Base, color );
    palette.setColor( QPalette::Active, QPalette::Text, (color.value() >= 175) ? Qt::black:Qt::white );

    palette.setColor( QPalette::Inactive, QPalette::Base, color );
    palette.setColor( QPalette::Inactive, QPalette::Text, (color.value() >= 175) ? Qt::black:Qt::white );

    // update
    setPalette(palette);
    update();

}
