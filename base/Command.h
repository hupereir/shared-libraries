#ifndef Command_h
#define Command_h

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

#include "Counter.h"

#include <QString>
#include <QStringList>

namespace Base
{
    class Command final: private Base::Counter<Command>
    {

        public:

        #if QT_VERSION < 0x040800
        //* constructor
        explicit Command( std::initializer_list<QString>&& );
        #endif

        //* constructor
        explicit Command( const QStringList& other = QStringList() ):
            Counter( "Command" ),
            values_( other )
        {}

        //* constructor
        explicit Command( QStringList&& other ):
            Counter( "Command" ),
            values_( std::move( other ) )
        {}

        //* constructor
        explicit Command( const QString& in ):
            Counter( "Command" ),
            values_( _parse( in ) )
        {}

        //*@name accessors
        //@{

        const QStringList& get() const { return values_; }

        //* run
        bool run( const QString& = QString() ) const;

        //@}

        //*@name modifiers
        //@{

        //* streamers
        template<class T>
            Command& operator<< ( const T& t )
        {
            values_ << t;
            return *this;
        }

        //@}

        private:

        //* parse command
        /**
        parse command so that first string in the list
        is the command name and following strings are all arguments
        first argument must start with a "-"
        */
        static QStringList _parse( const QString& );

        //* values
        QStringList values_;

    };

}

#endif
