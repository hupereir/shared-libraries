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

#include "ApplicationId.h"
#include "Debug.h"
#include "Util.h"

namespace Server
{

    //____________________________________________________
    ApplicationId::ApplicationId( const QString& name, QString user, QString display ):
        Counter( "ApplicationId" ),
        name_( name )
    {
        Debug::Throw( "ApplicationId::ApplicationId.\n" );
        if( user.isNull() ) user = Util::user();
        if( display.isNull() ) display = Util::env( "DISPLAY", "0.0" );
        user_ = user + QString("@")+display;
    }

    //____________________________________________________
    QString ApplicationId::userName() const
    {
        int position( user_.indexOf( "@" ) );
        return (position < 0 ) ? user_ : user_.left(position);
    }

    //______________________________________________________
    QString ApplicationId::display() const
    {
        int position( user_.indexOf( "@" ) );
        return (position < 0 ) ? "" : user_.mid(position+1 );
    }

    //______________________________________________________
    QDataStream& operator << (QDataStream& stream, const ApplicationId& id )
    {
        static const quint32 version = 0;
        stream << version << id.name_ << id.user_ << id.pid_;
        return stream;
    }

    //______________________________________________________
    QDataStream& operator >> (QDataStream& stream, ApplicationId& id )
    {
        quint32 version;
        stream >> version;
        if( version == 0 ) stream >> id.name_ >> id.user_ >> id.pid_;
        else Debug::Throw(0) << "Unrecognized ApplicationId version: " << version << endl;
        return stream;
    }
}
