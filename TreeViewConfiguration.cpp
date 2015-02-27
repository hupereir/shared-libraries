
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

#include "TreeViewConfiguration.h"

#include "Debug.h"
#include "XmlOptions.h"

#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QToolTip>

//____________________________________________________________________________
TreeViewConfiguration::TreeViewConfiguration( QWidget *parent, QTreeView *target, const QString& optionName ):
    QGroupBox( parent ),
    OptionWidget( optionName, this )
{

    Debug::Throw( "TreeViewConfiguration::TreeViewConfiguration.\n" );

    // check target
    Q_CHECK_PTR( target );

    // create vbox layout
    setLayout( new QVBoxLayout() );
    layout()->setMargin(5);
    layout()->setSpacing( 2 );

    // size
    QCheckBox *checkbox;

    // retrieve columns
    QHeaderView* header( target->header() );
    Q_CHECK_PTR( header );
    for( int index=0; index < header->count(); index++ )
    {

        // retrieve column name
        QString columnName( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( columnName.isNull() || columnName.isEmpty() )
        { columnName = QString( tr( "Column %1" ) ).arg( index+1 ); }

        // add checkbox
        checkbox = new QCheckBox( columnName, this );
        layout()->addWidget( checkbox );
        checkbox_ << checkbox;

        // add tooltip
        checkbox->setToolTip( QString( tr( "Show/hide column '%1'" ) ).arg( columnName )  );

    }

}

//____________________________________________________________________________
void TreeViewConfiguration::read( void )
{
    Debug::Throw( "TreeViewConfiguration::read.\n" );

    // set check button state according to the backup mask
    unsigned int mask( XmlOptions::get().get<unsigned int>( optionName() ) );
    for( int index = 0; index < checkbox_.size(); index++ )
    {
        checkbox_[index]->setChecked( mask & (1<<index) );
        if( !_connected() )
        { connect( checkbox_[index], SIGNAL(toggled(bool)), this, SIGNAL(modified())); }
    }

    _setConnected();

    return;

}

//____________________________________________________________________________
void TreeViewConfiguration::write( void ) const
{
    Debug::Throw( "TreeViewConfiguration::write.\n" );

    unsigned int mask(0);
    for( int index = 0; index < checkbox_.size(); index++ )
    { mask |= (checkbox_[index]->isChecked() << index); }

    XmlOptions::get().set<unsigned int>( optionName(), mask );

    return;

}
