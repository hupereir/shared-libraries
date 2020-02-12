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

#include "SpellItemDialog.h"

#include "CppUtil.h"
#include "TreeView.h"

#include <QHeaderView>
#include <QLabel>

namespace SpellCheck
{

    //_______________________________________________
    SpellItemDialog::SpellItemDialog( QWidget* parent ):
        CustomDialog( parent )
    {

        // add label
        auto label = new QLabel( tr( "Select the items that should appear in the menu:" ), this );
        label->setWordWrap( true );
        mainLayout().addWidget( label );

        // add list
        mainLayout().addWidget( list_ = new TreeView( this ) );
        list_->setSortingEnabled( false );
        list_->setModel( &model_ );
        list_->header()->hide();

        // connections
        connect( list_, &QAbstractItemView::clicked, this, &SpellItemDialog::_selectItem );

    }

    //_______________________________________________
    void SpellItemDialog::setItems( const QOrderedSet<QString>& items )
    { model_.set( Base::makeT<SpellItemModel::List>( items ) ); }

    //_______________________________________________
    void SpellItemDialog::setDisabledItems( const QString& items )
    { model_.setDisabledItems( QSet<QString>::fromList( items.split( QLatin1Char(' ') ) ) ); }

    //_______________________________________________
    QString SpellItemDialog::disabledItems() const
    { return QStringList( model_.disabledItems().toList() ).join( QStringLiteral(" ") ); }

    //_______________________________________________
    void SpellItemDialog::_selectItem( const QModelIndex& index )
    {
        Debug::Throw( QStringLiteral("SpellItemDialog::_selectItem.\n") );

        if( !index.isValid() ) return;

        // get string and list of disabled items
        auto disabledItems( model_.disabledItems() );
        const QString value( model_.get( index ) );

        // toggle
        if( disabledItems.contains( value ) ) disabledItems.remove( value );
        else disabledItems.insert( value );

        // replace in model
        model_.setDisabledItems( disabledItems );

        return;
    }
}
