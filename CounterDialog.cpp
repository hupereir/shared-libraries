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
* ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA   02111-1307 USA
*
*
*******************************************************************************/

#include "BaseIcons.h"
#include "CounterMap.h"
#include "CounterDialog.h"
#include "IconEngine.h"
#include "Singleton.h"
#include "AnimatedTreeView.h"

#include <QPushButton>
#include <QLayout>

//__________________________________________________________________________
CounterDialog::CounterDialog( QWidget* parent ):
CustomDialog( parent, OkButton, Qt::Window )
{

    Debug::Throw( "CounterDialog::CounterDialog.\n" );
    setWindowTitle( "Object Counters" );
    setOptionName( "COUNTER_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(10);
    mainLayout().addWidget( list_ = new AnimatedTreeView( this ) );
    list_->setModel( &model_ );

    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( ICONS::RELOAD ), "&Reload", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( update() ) );
    button->setAutoDefault( false );

}

//__________________________________________________________________________
void CounterDialog::update( void )
{

    Debug::Throw( "CounterDialog::update.\n" );

    // retrieve counters
    CounterMap& counters( CounterMap::get() );
    QMutexLocker lock( &counters.mutex() );

    CounterModel::List counterList;
    for( CounterMap::const_iterator iter = counters.begin(); iter != counters.end(); ++iter )
    { counterList << qMakePair( iter.key(), iter.value() ); }

    model_.add( counterList );

    // resize list
    list_->resizeColumnToContents( CounterModel::NAME );

}
