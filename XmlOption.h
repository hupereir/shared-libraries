#ifndef XmlOption_h
#define XmlOption_h

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

#include <QDomDocument>
#include <QDomElement>
#include <QString>

#include "Option.h"

namespace Base
{
    //! some XML definitions specific to Option management
    namespace XML
    {

        static const QString SPECIAL_OPTION = "SpecialOption";
        static const QString OPTION = "Option";
        static const QString OPTIONS = "Options";
        static const QString VALUE = "Value";

    }

}

//! Xml implementation of the Option object
class XmlOption:public Option
{

    public:

    //! constructor
    XmlOption( const QString& name, const Option& option ):
        Option( option ),
        name_( name )
    {}

    //! constructor from DOM node
    XmlOption( const QDomElement& element );

    //! create dom element
    QDomElement domElement( QDomDocument& parent ) const;

    //! name
    void setName( const QString& value )
    { name_ = value; }

    //! name
    const QString& name( void ) const
    { return name_; }

    private:

    //! option name
    QString name_;

};

#endif
