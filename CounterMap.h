#ifndef CounterMap_h
#define CounterMap_h

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
#include <QMutex>
#include <QMutexLocker>
#include <QHash>

//* thread-safe Object counter storage map
class CounterMap: public QHash<QString, int>
{

    public:

    //* singleton
    static CounterMap& get( void );

    //*  get counter for a given name
    /**
    if the name is found, returns adress of the existing counter
    creates new counter otherwise and returns adress
    */
    int* counter( const QString& name )
    {
        iterator iter = find( name );
        if( iter == end() ) return &( insert( name, 0 ).value() );
        else return &(iter.value());
    }

    //* increment
    void increment( int& counter )
    {
        QMutexLocker locker( &mutex_ );
        counter++;
    }

    //* increment
    void decrement( int& counter )
    {
        QMutexLocker locker( &mutex_ );
        counter--;
    }

    //* mutex
    QMutex& mutex( void )
    { return mutex_; }

    private:

    //* constructor
    CounterMap( void )
    {}

    //* mutex
    QMutex mutex_;

};

#endif
