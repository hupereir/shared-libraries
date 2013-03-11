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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "SpellItemDialog.h"
#include "TreeView.h"

#include <QHeaderView>
#include <QLabel>

namespace SPELLCHECK
{

    //_______________________________________________
    SpellItemDialog::SpellItemDialog( QWidget* parent ):
        CustomDialog( parent )
    {

        // add label
        QLabel* label = new QLabel( tr( "Select the items that should appear in the menu:" ), this );
        label->setWordWrap( true );
        mainLayout().addWidget( label );

        // add list
        mainLayout().addWidget( list_ = new TreeView( this ) );
        _list().setSortingEnabled( false );
        _list().setModel( &model_ );
        _list().header()->hide();

        // connections
        connect( &_list(), SIGNAL( clicked( const QModelIndex& ) ), SLOT( _selectItem( const QModelIndex& ) ) );

    }

    //_______________________________________________
    void SpellItemDialog::setItems( const QOrderedSet<QString>& items )
    { model_.set( items.toList() ); }

    //_______________________________________________
    void SpellItemDialog::setDisabledItems( const QString& items )
    { model_.setDisabledItems( QSet<QString>::fromList( items.split( " " ) ) ); }

    //_______________________________________________
    QString SpellItemDialog::disabledItems( void ) const
    { return QStringList( QList<QString>::fromSet( model_.disabledItems() ) ).join( " " ); }

    //_______________________________________________
    void SpellItemDialog::_selectItem( const QModelIndex& index )
    {
        Debug::Throw( "SpellItemDialog::_selectItem.\n" );

        if( !index.isValid() ) return;

        // get string and list of disabled items
        SpellItemModel::ItemSet disabledItems( model_.disabledItems() );
        const QString value( model_.get( index ) );

        // toggle
        if( disabledItems.contains( value ) ) disabledItems.remove( value );
        else disabledItems.insert( value );

        // replace in model
        model_.setDisabledItems( disabledItems );

        return;
    }
}
