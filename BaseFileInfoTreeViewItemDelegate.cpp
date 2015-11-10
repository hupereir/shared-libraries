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

#include "BaseFileInfoTreeViewItemDelegate.h"

#include "BaseFileIconProvider.h"
#include "BaseFileInfoModel.h"

#include <QAbstractItemView>
#include <QStyleOptionViewItemV4>

//______________________________________________________
BaseFileInfoTreeViewItemDelegate::BaseFileInfoTreeViewItemDelegate( QObject* parent ):
    TreeViewItemDelegate( parent )
{}

//______________________________________________________
void BaseFileInfoTreeViewItemDelegate::paint(
    QPainter* painter,
    const QStyleOptionViewItem& option, const QModelIndex& index ) const
{

    const QStyleOptionViewItemV4 *optionV4 = qstyleoption_cast<const QStyleOptionViewItemV4*>( &option );
    if( !optionV4 || optionV4->icon.isNull() ) return TreeViewItemDelegate::paint( painter, option, index );

    // get model from parent
    QAbstractItemView* itemView( qobject_cast<QAbstractItemView*>(parent()) );
    if( !( itemView && itemView->model() ) ) return TreeViewItemDelegate::paint( painter, option, index );

    QVariant fileTypeVariant( itemView->model()->data( index, Base::FileTypeRole ) );
    if( fileTypeVariant.canConvert( QVariant::Int ) )
    {
        const int type( fileTypeVariant.toInt() );

        // get pixmap
        QPixmap pixmap( optionV4->icon.pixmap( itemView->iconSize() ) );
        if( type & BaseFileInfo::Link ) pixmap = BaseFileIconProvider::linked( pixmap );
        if( type & BaseFileInfo::Hidden ) pixmap = BaseFileIconProvider::hidden( pixmap );
        if( type & BaseFileInfo::Clipped ) pixmap = BaseFileIconProvider::clipped( pixmap );

        QStyleOptionViewItemV4 copy( *optionV4 );
        copy.icon = pixmap;

        return TreeViewItemDelegate::paint( painter, copy, index );

    } else {

        return TreeViewItemDelegate::paint( painter, option, index );

    }
}
