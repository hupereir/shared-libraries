#ifndef CounterMap_h
#define CounterMap_h

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QMap>

//! thread-safe Object counter storage map
class CounterMap: public QMap<QString, int>
{

    public:

    //! singleton
    static CounterMap& get( void );

    //!  get counter for a given name
    /*!
    if the name is found, returns adress of the existing counter
    creates new counter otherwise and returns adress
    */
    int* counter( const QString& name )
    {
        iterator iter = find( name );
        if( iter == end() ) return &( insert( name, 0 ).value() );
        else return &(iter.value());
    }

    //! increment
    void increment( int& counter )
    {
        QMutexLocker locker( &mutex() );
        counter++;
    }

    //! increment
    void decrement( int& counter )
    {
        QMutexLocker locker( &mutex() );
        counter--;
    }

    //! mutex
    QMutex& mutex( void )
    { return mutex_; }

    private:

    //! constructor
    CounterMap( void )
    {}

    //! mutex
    QMutex mutex_;

};

#endif
