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

#include "BaseFileInfoItemDelegate.h"

#include "BaseFileIconProvider.h"
#include "BaseFileInfoModel.h"

#include <QAbstractItemView>
#include <QStyleOptionViewItemV4>

//______________________________________________________
BaseFileInfoItemDelegate::BaseFileInfoItemDelegate( QObject* parent ):
    TreeViewItemDelegate( parent )
{}

//______________________________________________________
void BaseFileInfoItemDelegate::initStyleOption( QStyleOptionViewItem* option, const QModelIndex& index) const
{
    TreeViewItemDelegate::initStyleOption( option, index );

    // cast option and check icon
    QStyleOptionViewItemV4 *optionV4 = qstyleoption_cast<QStyleOptionViewItemV4*>( option );
    if( !optionV4 || optionV4->icon.isNull() ) return;

//     // item view
//     QAbstractItemView* itemView( qobject_cast<QAbstractItemView*>(parent()) );
//     if( !itemView ) return;

    // check type
    QVariant fileTypeVariant( index.data( Base::FileTypeRole ) );
    if( fileTypeVariant.canConvert( QVariant::Int ) )
    {
        const int type( fileTypeVariant.toInt() );

        // get pixmap
        QPixmap pixmap( optionV4->icon.pixmap( optionV4->decorationSize ) );
        if( type & BaseFileInfo::Link ) pixmap = BaseFileIconProvider::linked( pixmap );
        if( type & BaseFileInfo::Hidden ) pixmap = BaseFileIconProvider::hidden( pixmap );
        if( type & BaseFileInfo::Clipped ) pixmap = BaseFileIconProvider::clipped( pixmap );
        optionV4->icon = pixmap;

    }
    return;

}
