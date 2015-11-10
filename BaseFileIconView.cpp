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

#include "BaseFileIconView.h"

#include "BaseFileInfoModel.h"
#include "BaseFileIconProvider.h"

//______________________________________________________________
BaseFileIconView::BaseFileIconView( QWidget* parent ):
    IconView( parent )
{ Debug::Throw( "BaseFileIconView::BaseFileIconView.\n" ); }

//______________________________________________________________
void BaseFileIconView::_updateItem( IconViewItem& item, const QModelIndex& index ) const
{

    // base class
    IconView::_updateItem( item, index );

    // get pixmap
    QPixmap pixmap( item.pixmap() );
    if( pixmap.isNull() ) return;

    // check type role and customize pixmap
    QVariant fileTypeVariant( model()->data( index, Base::FileTypeRole ) );
    if( fileTypeVariant.canConvert( QVariant::Int ) )
    {
        const int type( fileTypeVariant.toInt() );

        if( type & BaseFileInfo::Link ) pixmap = BaseFileIconProvider::linked( pixmap );
        if( type & BaseFileInfo::Hidden ) pixmap = BaseFileIconProvider::hidden( pixmap );
        if( type & BaseFileInfo::Clipped ) pixmap = BaseFileIconProvider::clipped( pixmap );

    }

    item.setPixmap( pixmap );

}
