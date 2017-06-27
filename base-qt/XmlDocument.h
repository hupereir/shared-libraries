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

#include "Counter.h"
#include "XmlError.h"

#include <QByteArray>
#include <QFile>
#include <QDomDocument>
#include <QDomElement>

class XmlDocument: public QDomDocument, private Base::Counter<XmlDocument>
{

    public:

    //* constructor
    explicit XmlDocument();

    //*@name accessors
    //@{

    //* xml error
    const XmlError& error() const
    { return error_; }

    //@}

    //*@name modifiers
    //@{

    //* set content
    bool setContent( QFile* file )
    { return setContent( file, error_ ); }

    //* set content
    bool setContent( const QString& content )
    { return setContent( content, error_ ); }

    //* set content
    bool setContent( const QByteArray& content )
    { return setContent( content, error_ ); }

    //* set content
    bool setContent( QFile*, XmlError& );

    //* set content
    bool setContent( QIODevice*, XmlError& );

    //* set content
    bool setContent( const QByteArray&, XmlError& );

    //* set content
    bool setContent( const QString&, XmlError& );

    //* replace child
    void replaceChild( QDomElement& );

    //@}

    private:

    //* document node name
    const QString topNodeTagName_;

    //* error
    XmlError error_;

};

#endif
