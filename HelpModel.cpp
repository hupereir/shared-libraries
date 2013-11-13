
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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "HelpModel.h"
#include "HelpModel.moc"

#include <QMimeData>
#include <QStringList>

namespace BASE
{

    //__________________________________________________________________
    Qt::ItemFlags HelpModel::flags(const QModelIndex &index) const
    {
        if( !index.isValid() ) return Qt::ItemFlags(0);
        Qt::ItemFlags flags( Qt::ItemIsEnabled | Qt::ItemIsSelectable );
        return flags;
    }

    //__________________________________________________________________
    QVariant HelpModel::data( const QModelIndex& index, int role ) const
    {

        // check index, role and column
        if( !index.isValid() ) return QVariant();
        if( role != Qt::DisplayRole ) return QVariant();
        if( index.column() != LABEL ) return QVariant();
        return QString( get(index).label() );

    }

}
