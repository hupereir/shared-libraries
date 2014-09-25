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

/*!
\file FontEditor.cpp
\brief three line editors and a browser to select fonts
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "BaseIconNames.h"
#include "FontEditor.h"
#include "FontEditor.moc"
#include "IconEngine.h"

#include <QLayout>
#include <QToolButton>



//______________________________________________
FontEditor::FontEditor( QWidget *parent ):
    QWidget( parent ),
    Counter( "FontEditor" )
{

    QHBoxLayout *layout( new QHBoxLayout() );
    layout->setMargin(0);
    setLayout( layout );

    label_ =  new QLabel( this );
    label_->setAlignment( Qt::AlignCenter );
    label_->setFont( font() );
    label_->setFrameStyle( QFrame::StyledPanel|QFrame::Sunken);
    layout->addWidget( label_, 1 );

    // update palette
    QPalette palette( label_->palette() );
    palette.setBrush( QPalette::Base, palette.brush( QPalette::Window ) );
    label_->setPalette( palette );

    // create push_button
    QToolButton *button( new QToolButton( this ) );
    button->setAutoRaise( true );
    button->setIcon( IconEngine::get( IconNames::Open ) );
    layout->addWidget( button, 0 );

    // connect push_button
    connect( button, SIGNAL(clicked()), SLOT(_selectFont()) );
    setFont( font() );
}
