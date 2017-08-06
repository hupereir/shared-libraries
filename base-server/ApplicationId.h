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
    class ApplicationId final:private Base::Counter<ApplicationId>
    {
        public:

        //* constructor
        explicit ApplicationId( const QString& = QString(), QString = QString(), QString = QString() );

        //*@name accessors
        //@{

        //* name
        const QString& name() const
        { return name_; }

        //* short name
        QString userName() const;

        //* short name
        QString display() const;

        //* user
        const QString& user() const
        { return user_; }

        //* pid
        qint64 processId() const
        { return pid_; }

        //* returns true if user and name makes sense
        bool isValid() const
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

    //* equal to operator
    inline bool operator == (const ApplicationId& first, const ApplicationId& second)
    { return first.name() == second.name() && first.user() == second.user(); }

    //* hash
    inline uint qHash( const ApplicationId& id )
    { return qHash( id.name() + id.user() ); }

}

#endif
