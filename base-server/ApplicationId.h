#ifndef ApplicationId_h
#define ApplicationId_h

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

#include <QDataStream>
#include <QString>

namespace Server
{

    //* Stores application name and user to uniquely identify an application.
    class ApplicationId:public Counter
    {
        public:

        //* constructor
        ApplicationId( const QString& = QString(), QString = QString(), QString = QString() );

        //* equal to operator
        bool operator == (const ApplicationId& app ) const
        { return name() == app.name() && user() == app.user(); }

        //* equal to operator
        bool operator != (const ApplicationId& app ) const
        { return !( app == *this ); }

        //* lower than to operator
        bool operator < (const ApplicationId& app ) const
        {
            if ( name() != app.name() ) return name() < app.name();
            else if( user() != app.user() ) return user() < app.user();
            return false;
        }

        //*@name accessors
        //@{

        //* name
        const QString& name( void ) const
        { return name_; }

        //* short name
        QString userName( void ) const;

        //* short name
        QString display( void ) const;

        //* user
        const QString& user( void ) const
        { return user_; }

        //* pid
        qint64 processId( void ) const
        { return pid_; }

        //* returns true if user and name makes sense
        bool isValid( void ) const
        { return !(name().isEmpty() || user().isEmpty() ); }

        //@}

        //*@name modifiers
        //@{

        //* name
        void setName( const QString& value )
        { name_ = value; }

        //* user
        void setUser( const QString& value )
        { user_ = value; }

        //* pid
        void setProcessId( qint64 value )
        { pid_ = value; }

        //@}

        private:

        //* application name
        QString name_;

        //* application user
        QString user_;

        //* process id
        qint64 pid_ = 0;

        //*@name serializer
        //@{
        friend QDataStream& operator << (QDataStream&, const ApplicationId& );
        friend QDataStream& operator >> (QDataStream&, ApplicationId& );
        //@}

    };
};

#endif
