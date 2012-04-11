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
/*!
\file PixmapCacheDialog.cpp
\brief displays PixmapCache names and counts
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "BaseIcons.h"
#include "CustomPixmap.h"
#include "PixmapCacheDialog.h"
#include "PixmapEngine.h"
#include "IconSize.h"
#include "AnimatedTreeView.h"

#include <QtGui/QPushButton>
#include <QtGui/QLayout>

//__________________________________________________________________________
PixmapCacheDialog::PixmapCacheDialog( QWidget* parent ):
CustomDialog( parent, OkButton, Qt::Window )
{

    Debug::Throw( "PixmapCacheDialog::PixmapCacheDialog.\n" );
    setWindowTitle( "Pixmap Cache" );
    setOptionName( "ICON_CACHE_DIALOG" );

    layout()->setMargin(0);
    buttonLayout().setMargin(5);

    // insert main vertical box
    list_ = new AnimatedTreeView( this );
    mainLayout().addWidget( list_ );
    list_->setModel( &model_ );
    list_->setIconSize( IconSize( IconSize::Large ) );

    QPushButton *button;
    buttonLayout().insertWidget( 1, button = new QPushButton( PixmapEngine::get( ICONS::RELOAD ),"&Update", this ) );
    connect( button, SIGNAL( clicked() ), SLOT( update() ) );
    button->setAutoDefault( false );

}

//__________________________________________________________________________
void PixmapCacheDialog::update( void )
{

    Debug::Throw( "PixmapCacheDialog::update.\n" );

    // retrieve cache
    const PixmapEngine::Cache& cache( PixmapEngine::cache() );
    PixmapCacheModel::List modelList;
    for( PixmapEngine::Cache::const_iterator iter = cache.begin(); iter != cache.end(); ++iter )
    { modelList.push_back( PixmapEngine::Pair( iter.key(), iter.value() ) ); }

    model_.set( modelList );

    list_->resizeColumnToContents( PixmapCacheModel::Icon );

}
