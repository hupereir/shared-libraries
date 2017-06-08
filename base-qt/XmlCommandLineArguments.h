#ifndef XmlCommandLineArguments_h
#define XmlCommandLineArguments_h

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

#include "CommandLineArguments.h"

#include <QDomDocument>
#include <QDomElement>
#include <QString>

//! Xml interface to time manipulation object
class XmlCommandLineArguments:public CommandLineArguments
{

    public:

    //! empty creator
    explicit XmlCommandLineArguments( void )
    {}

    //! creator from DOM
    explicit XmlCommandLineArguments( const QDomElement& );

    //! create from CommandLineArguments
    explicit XmlCommandLineArguments( const CommandLineArguments& stamp ):
        CommandLineArguments( stamp )
    {}

    //! get DOM element
    QDomElement domElement( const QString&, QDomDocument& ) const;

};

#endif
