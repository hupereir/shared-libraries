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

#include "ApplicationId.h"
#include "Debug.h"
#include "ServerXmlDef.h"
#include "Util.h"

namespace Server
{

    //____________________________________________________
    ApplicationId::ApplicationId( const QString& name, QString user, QString display ):
        Counter( "ApplicationId" ),
        name_( name ),
        pid_( 0 )
    {
        Debug::Throw( "ApplicationId::ApplicationId.\n" );
        if( user.isNull() ) user = Util::user();
        if( display.isNull() ) display = Util::env( "DISPLAY", "0.0" ).replace( ":", "" );
        user_ = user + QString("@")+display;
    }

    //____________________________________________________
    ApplicationId::ApplicationId( const QDomElement& element ):
        Counter( "ApplicationId" ),
        pid_( 0 )
    {
        Debug::Throw( "ApplicationId::ApplicationId (dom).\n" );

        // parse attributes
        QDomNamedNodeMap attributes( element.attributes() );
        for( int i=0; i<attributes.count(); i++ )
        {
            QDomAttr attribute( attributes.item( i ).toAttr() );
            if( attribute.isNull() ) continue;
            if( attribute.name() == Xml::User ) setUser( attribute.value() );
            else if( attribute.name() == Xml::Name ) setName( attribute.value() );
            else if( attribute.name() == Xml::Pid ) setProcessId( attribute.value().toLongLong() );
            else Debug::Throw() << "ApplicationId::ApplicationId - unrecognized attribute " << attribute.name() << endl;
        }

    }

    //____________________________________________________
    QDomElement ApplicationId::domElement( QDomDocument& document ) const
    {

        Debug::Throw( "ApplicationId::domElement" );
        QDomElement out( document.createElement( Xml::Id ) );
        if( !name().isEmpty() ) out.setAttribute( Xml::Name, name() );
        if( !user().isEmpty() ) out.setAttribute( Xml::User, user() );
        if( pid_ > 0 ) out.setAttribute( Xml::Pid, QString::number( pid_ ) );
        return out;

    }

    //____________________________________________________
    QString ApplicationId::userName( void ) const
    {
        int position( user_.indexOf( "@" ) );
        return (position < 0 ) ? user_ : user_.left(position);
    }

    //______________________________________________________
    QString ApplicationId::display( void ) const
    {
        int position( user_.indexOf( "@" ) );
        return (position < 0 ) ? "" : user_.mid(position+1 );
    }
}
