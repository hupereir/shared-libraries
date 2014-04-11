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

#include "PathHistoryConfiguration.h"
#include "PathHistoryConfiguration.moc"

#include "OptionCheckBox.h"
#include "OptionSpinBox.h"

#include <QLabel>
#include <QLayout>

//___________________________________________
PathHistoryConfiguration::PathHistoryConfiguration( QWidget* parent ):
    QWidget( parent ),
    OptionWidgetList( this ),
    Counter( "PathHistoryConfiguration" )
{
    Debug::Throw( "PathHistoryConfiguration::PathHistoryConfiguration.\n" );

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );
    layout->setMargin(0);
    layout->setSpacing(5);

    OptionCheckBox* checkbox;
    layout->addWidget( checkbox = new OptionCheckBox( tr( "Save/restore history across sessions" ), this, "SAVE_HISTORY" ) );
    addOptionWidget( checkbox );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    layout->addLayout( hLayout );

    hLayout->addSpacing( _checkBoxSpacing() );

    QLabel* label;
    OptionSpinBox* spinbox;
    hLayout->addWidget( label = new QLabel( tr( "Maximum history size" ), this ) );
    hLayout->addWidget( spinbox = new OptionSpinBox( this, "HISTORY_SIZE" ) );
    spinbox->setMinimum(0);
    spinbox->setMaximum(100);
    spinbox->setSpecialValueText( tr( " Unlimited" ) );
    addOptionWidget( spinbox );
    label->setBuddy( spinbox );

    label->setEnabled( false );
    spinbox->setEnabled( false );

    connect( checkbox, SIGNAL(toggled(bool)), label, SLOT(setEnabled(bool)) );
    connect( checkbox, SIGNAL(toggled(bool)), spinbox, SLOT(setEnabled(bool)) );

}
