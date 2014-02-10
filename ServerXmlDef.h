#ifndef ServerXmlDef_h
#define ServerXmlDef_h

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

#include <QString>

//! Some Xml definitions
namespace Server
{

    namespace Xml
    {
        const QString Transmission( "_t" );
        const QString Command( "c" );
        const QString Id( "i" );
        const QString User( "u" );
        const QString Name( "n" );
        const QString Type( "t" );
        const QString Pid( "p" );
        const QString Arguments( "a" );
    }

}
#endif
