#ifndef XmlOption_h
#define XmlOption_h

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

#include "base_qt_export.h"
#include "Option.h"

#include <QDomDocument>
#include <QDomElement>
#include <QString>

namespace Base
{
    //* some XML definitions specific to Option management
    namespace Xml
    {

        static const QString SpecialOption = QStringLiteral("SpecialOption");
        static const QString Option = QStringLiteral("Option");
        static const QString Options = QStringLiteral("Options");
        static const QString Value = QStringLiteral("Value");

    }

}

//* Xml implementation of the Option object
class BASE_QT_EXPORT XmlOption:public Option
{

    public:

    //* constructor
    explicit XmlOption()
    {}

    //* constructor
    explicit XmlOption( const QString& name, const Option& option ):
        Option( option ),
        name_( name )
    {}

    //* constructor from DOM node
    explicit XmlOption( const QDomElement& );

    //* create dom element
    QDomElement domElement( QDomDocument& ) const;

    //* name
    void setName( const QString& value )
    { name_ = value; }

    //* name
    const QString& name() const
    { return name_; }

    private:

    //* option name
    QString name_;

};

#endif
