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

#include "Option.h"

//________________________________________________________
QTextStream& operator >> ( QTextStream& in, bool& value )
{
    short unsigned int local;
    in >> local;
    value = (bool)( local );
    return in;
}

//________________________________________________________
Option::Option():
    Counter( "Option" ),
    flags_( Recordable ),
    defaultFlags_( Recordable )
{}

//________________________________________________________
Option::Option( const char* value, Flags flags ):
    Counter( "Option" ),
    flags_( flags ),
    defaultFlags_( Recordable )
{
    Q_CHECK_PTR( value );
    value_ = value;
}

//________________________________________________________
Option::Option( const QByteArray& value, Flags flags ):
    Counter( "Option" ),
    value_( value ),
    flags_( flags ),
    defaultFlags_( Recordable )
{}

//________________________________________________________
Option::Option( const QString& value, Flags flags ):
    Counter( "Option" ),
    value_( value.toUtf8() ),
    flags_( flags ),
    defaultFlags_( Recordable )
{}

//________________________________________________________
Option::Option( const QByteArray& value, const QString& comments, Flags flags ):
    Counter( "Option" ),
    value_( value ),
    comments_( comments ),
    flags_( flags ),
    defaultFlags_( Recordable )
{}
