#ifndef ApplicationId_h
#define ApplicationId_h

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

/*!
   \file    ApplicationId.h
   \brief   application unique Identifier
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QDomDocument>
#include <QDomElement>
#include <QString>

#include "Counter.h"

namespace SERVER
{


  /*! \brief
    application identifier.
    Stores application name and user
    to uniquely identify an application.
  */
  class ApplicationId:public Counter
  {
    public:

    //! constructor
    ApplicationId( const QString& name = "", const QString& user = "", const QString& display = "" );

    //! constructor
    ApplicationId( const QDomElement& );

    //! dom element
    QDomElement domElement( QDomDocument& ) const;

    //! name
    const QString& name( void ) const
    { return name_; }

    //! name
    void setName( const QString& value )
    { name_ = value; }

    //! short name
    QString userName( void ) const;

    //! short name
    QString display( void ) const;

    //! user
    const QString& user( void ) const
    { return user_; }

    //! user
    void setUser( const QString& value )
    { user_ = value; }

    //! equal to operator
    bool operator == (const ApplicationId& app ) const
    {
      return (
        name() == app.name() &&
        user() == app.user() );
    }

    //! equal to operator
    bool operator != (const ApplicationId& app ) const
    { return !( app == *this ); }

    //! lower than to operator
    bool operator < (const ApplicationId& app ) const
    {
      if ( name() != app.name() ) return name() < app.name();
      else if( user() != app.user() ) return user() < app.user();
      return false;
    }

    //! returns true if user and name makes sense
    bool isValid( void ) const
    { return !(name().isEmpty() || user().isEmpty() ); }

    private:

    //! application name
    QString name_;

    //! application user
    QString user_;

  };
};

#endif
