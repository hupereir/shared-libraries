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

#include "IconCacheDialog.h"

#include "BaseIcons.h"
#include "CustomPixmap.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "AnimatedTreeView.h"

#include <QtGui/QPushButton>
#include <QtGui/QLayout>

//__________________________________________________________________________
IconCacheDialog::IconCacheDialog( QWidget* parent ):
    CustomDialog( parent, OkButton, Qt::Window )
{

    Debug::Throw( "IconCacheDialog::IconCacheDialog.\n" );
    setWindowTitle( "Icon Cache" );
    setOptionName( "ICON_CACHE_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(5);

    // insert main vertical box
    list_ = new AnimatedTreeView( this );
    mainLayout().addWidget( list_ );
    list_->setModel( &model_ );
    list_->setIconSize( IconSize( IconSize::Large ) );

    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( IconEngine::get( ICONS::RELOAD ),"&Update", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( update() ) );
    button->setAutoDefault( false );

}

//__________________________________________________________________________
void IconCacheDialog::update( void )
{

    Debug::Throw( "IconCacheDialog::update.\n" );

    // retrieve cache
    const BASE::IconCache& cache( IconEngine::cache() );
    IconCacheModel::List modelList;
    for( BASE::IconCache::const_iterator iter = cache.begin(); iter != cache.end(); ++iter )
    { modelList << BASE::IconCache::Pair( iter.key(), iter.value() ); }

    model_.set( modelList );

    list_->resizeColumnToContents( IconCacheModel::Icon );

}
