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
#include "QtUtil.h"


#include <QApplication>
#include <QCursor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>
#include <QToolButton>

//______________________________________________
ColorDisplay::ColorDisplay( QWidget* parent ):
    QWidget( parent ),
    Counter( QStringLiteral("ColorDisplay") )
{
    Debug::Throw( QStringLiteral("ColorDisplay::ColorDisplay.\n") );

    QHBoxLayout *layout = new QHBoxLayout;
    QtUtil::setMargin(layout, 0);
    layout->setSpacing(2);
    setLayout( layout );

    editor_ = new Editor( this );
    editor_->setShowClearButton( false );
    editor_->setAlignment( Qt::AlignCenter );
    layout->addWidget( editor_, 1 );
    connect( editor_, &QLineEdit::returnPressed, this, &ColorDisplay::_selectColorFromText );

    // browse button
    {
        QToolButton *button;
        layout->addWidget( button = new QToolButton( this ), 0 );
        button->setAutoRaise( true );
        button->setIcon( IconEngine::get( IconNames::Open ) );
        button->setToolTip( tr( "Select color from dialog" ) );
        connect( button, &QAbstractButton::clicked, this, &ColorDisplay::_selectColorFromDialog );
    }

    // grab button
    {
        ColorGrabButton* button;
        layout->addWidget( button = new ColorGrabButton( this ), 0 );
        button->setAutoRaise( true );
        button->setIcon( IconEngine::get( IconNames::SelectColor ) );
        button->setToolTip( tr( "Grab color from screen" ) );
        connect( button, &ColorGrabButton::colorSelected, this, &ColorDisplay::setColor );
    }
}

//________________________________________________________
void ColorDisplay::_selectColorFromDialog()
{
    Debug::Throw( QStringLiteral("ColorDisplay::_selectColor.\n") );
    QColor color( QColorDialog::getColor( editor_->color(), this ) );
    if( color.isValid() ) setColor( color );
}

//________________________________________________________
void ColorDisplay::_selectColorFromText()
{
    Debug::Throw( QStringLiteral("ColorDisplay::_selectColorFromText.\n") );
    editor_->setColor( editor_->text() );
}

//_______________________________________________
QColor ColorDisplay::Editor::color() const
{
    Debug::Throw( QStringLiteral("ColorDisplay::Editor::color.\n") );
    QString text( LineEditor::text() );
    return text.isEmpty() ? QColor():QColor( text );
}

//_________________________________________________________
void ColorDisplay::Editor::setColor( QColor color )
{
    Debug::Throw() << "ColorDisplay::Editor::setColor - " << color.name() << Qt::endl;

    // check color validity
    if( color.isValid() ) setText( color.name() );
    else {

        QString buffer;
        if( !text().isEmpty() ) buffer = tr( "Invalid color: %1" ).arg( text() );
        else buffer = tr( "Invalid color" );

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
