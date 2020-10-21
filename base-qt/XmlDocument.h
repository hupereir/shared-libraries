#ifndef XmlDocument_h
#define XmlDocument_h

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
#include "Counter.h"
#include "XmlError.h"

#include <QByteArray>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>

class BASE_QT_EXPORT XmlDocument final: private Base::Counter<XmlDocument>
{

    public:

    //* constructor
    explicit XmlDocument();

    //*@name accessors
    //@{

    //* immutable accessor
    const QDomDocument& get() const { return document_; }

    //* elements
    QDomNodeList elementsByTagName( const QString& value ) const
    { return document_.elementsByTagName( value ); }

    //* content
    QByteArray toByteArray() const
    { return document_.toByteArray(); }

    //* xml error
    const XmlError& error() const
    { return error_; }

    //@}

    //*@name modifiers
    //@{

    //* mutable accessor
    QDomDocument& get() { return document_; }

    //* set content
    template<class T>
        bool setContent( T&& t )
    { return setContent( std::forward<T>(t), error_ ); }

    //* set content
    template<class T>
        bool setContent( T&& t, XmlError& error )
    {
        error.clear();
        return document_.setContent( std::forward<T>(t), &error.error(), &error.line(), &error.column() );
    }

    //* create element
    QDomElement createElement( const QString& value )
    { return document_.createElement( value ); }

    //* replace child
    void replaceChild( QDomElement& );

    //@}

    private:

    //* document
    QDomDocument document_;

    //* document node name
    const QString topNodeTagName_;

    //* error
    XmlError error_;

};

#endif
