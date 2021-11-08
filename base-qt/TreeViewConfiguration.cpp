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
#include "QtUtil.h"
#include "XmlOptions.h"


#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QToolTip>

//____________________________________________________________________________
TreeViewConfiguration::TreeViewConfiguration( QWidget *parent, QTreeView *target, const QString& optionName ):
    QGroupBox( parent ),
    OptionWidget( optionName )
{

    Debug::Throw( QStringLiteral("TreeViewConfiguration::TreeViewConfiguration.\n") );

    // create vbox layout
    setLayout( new QVBoxLayout );
    QtUtil::setMargin(layout(), 5);
    layout()->setSpacing( 2 );

    // size
    QCheckBox *checkbox;

    // retrieve columns
    auto header( target->header() );
    for( int index=0; index < header->count(); index++ )
    {

        // retrieve column name
        auto columnName( header->model()->headerData( index, Qt::Horizontal, Qt::DisplayRole ).toString() );
        if( columnName.isNull() || columnName.isEmpty() )
        { columnName = tr( "Column %1" ).arg( index+1 ); }

        // add checkbox
        checkbox = new QCheckBox( columnName, this );
        layout()->addWidget( checkbox );
        checkbox_.append( checkbox );

        // add tooltip
        checkbox->setToolTip( tr( "Show/hide column '%1'" ).arg( columnName )  );

    }

}

//____________________________________________________________________________
void TreeViewConfiguration::read( const Options& options )
{
    Debug::Throw( QStringLiteral("TreeViewConfiguration::read.\n") );

    // set check button state according to the backup mask
    int mask( options.get<int>( optionName() ) );
    for( int index = 0; index < checkbox_.size(); index++ )
    {
        checkbox_[index]->setChecked( mask & (1<<index) );
        if( !_connected() )
        { connect( checkbox_[index], &QAbstractButton::toggled, this, &TreeViewConfiguration::modified); }
    }

    _setConnected();

    return;

}

//____________________________________________________________________________
void TreeViewConfiguration::write( Options& options ) const
{
    Debug::Throw( QStringLiteral("TreeViewConfiguration::write.\n") );

    int mask(0);
    for( int index = 0; index < checkbox_.size(); index++ )
    { mask |= (checkbox_[index]->isChecked() << index); }

    options.set<int>( optionName(), mask );

    return;

}
