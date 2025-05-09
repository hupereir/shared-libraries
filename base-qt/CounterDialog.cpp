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

#include "CounterDialog.h"
#include "BaseIconNames.h"
#include "CounterMap.h"
#include "IconEngine.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "TreeView.h"

#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <QShortcut>

//__________________________________________________________________________
CounterDialog::CounterDialog( QWidget* parent ):
Dialog( parent, OkButton, Qt::Window )
{

    Debug::Throw( QStringLiteral("CounterDialog::CounterDialog.\n") );
    setWindowTitle( tr( "Object Counters" ) );
    setOptionName( QStringLiteral("COUNTER_DIALOG") );

    QtUtil::setMargin(layout(), 0);
    QtUtil::setMargin(&buttonLayout(), defaultMargin());
    mainLayout().addWidget( list_ = new TreeView( this ) );
    list_->setModel( &model_ );

    QtUtil::setWidgetSides(list_, Qt::TopEdge|Qt::BottomEdge);

    auto button = new QPushButton(IconEngine::get( IconNames::Reload), tr( "Reload" ), this );
    buttonLayout().insertWidget( 1, button );
    connect( button, &QPushButton::clicked, this, [this](bool){ updateCounters(); } );
    button->setAutoDefault( false );

    connect( new QShortcut( QKeySequence::Refresh, this ), &QShortcut::activated, this, &CounterDialog::updateCounters );
}

//__________________________________________________________________________
void CounterDialog::updateCounters()
{

    Debug::Throw( QStringLiteral("CounterDialog::updateCounters.\n") );

    // retrieve counters
    auto&& counters( Base::CounterMap::get() );

    CounterModel::List counterList;
    for( auto&& iter = counters.begin(); iter != counters.end(); ++iter )
    {
        if( *iter.value() )
        { counterList.append( { iter.key(), *iter.value() } ); }
    }

    model_.update( counterList );

    // resize list
    list_->resizeColumnToContents( CounterModel::Name );

}
