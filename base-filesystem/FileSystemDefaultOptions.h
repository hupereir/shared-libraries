#ifndef FileSystemDefaultOptions_h
#define FileSystemDefaultOptions_h

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

#include "XmlOptions.h"

#include "FileSystemModel.h"

//_____________________________________________________
//* default options installer
void installFileSystemOptions()
{
    XmlOptions::get().setAutoDefault( true );
    XmlOptions::get().set<bool>( QStringLiteral("SHOW_HIDDEN_FILES"), false );
    XmlOptions::get().set<bool>( QStringLiteral("SHOW_NAVIGATOR"), false );

    XmlOptions::get().set<int>( QStringLiteral("FILE_SYSTEM_LIST_MASK"), 1<<FileSystemModel::FileName );
    XmlOptions::get().set<int>( QStringLiteral("FILE_SYSTEM_LIST_SORT_COLUMN"), FileSystemModel::FileName );
    XmlOptions::get().set<int>( QStringLiteral("FILE_SYSTEM_LIST_SORT_ORDER"), Qt::AscendingOrder );

    XmlOptions::get().setAutoDefault( false );
}

#endif
