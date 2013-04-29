// $Id$

/***********************************************************************
*
* Copyright 2013 Hugo PEREIRA DA COSTA <hugo.pereira@free.fr>
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of 
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#include "Counter.h"
#include "CounterMap.h"

//____________________________________________________________
Counter::Counter( const QString& name ):
count_( 0 )
{
    count_ = CounterMap::get().counter( name );
    CounterMap::get().increment( *count_ );
    return;
}

//____________________________________________________________
Counter::Counter( const Counter& counter ):
    count_( counter.count_ )
{
    CounterMap::get().increment( *count_ );
    return;
}

//____________________________________________________________
Counter::~Counter( void )
{ if( count_ ) CounterMap::get().decrement( *count_ ); }

//____________________________________________________________
int Counter::count( void ) const
{ return (count_) ? *count_:0; }
