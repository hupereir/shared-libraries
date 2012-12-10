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
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include "BaseFileIconProvider.h"
#include "IconEngine.h"
#include "Util.h"

//__________________________________________________________________________
const QIcon& BaseFileIconProvider::icon( const BaseFileInfo& fileInfo )
{

    // get type
    int type( fileInfo.type() );

    // special case for home directory
    if( type & BaseFileInfo::Folder && fileInfo.file() == Util::home() )
    {

        return IconEngine::get( "user-home.png" );

    } else return IconEngine::get( QString() );

}
