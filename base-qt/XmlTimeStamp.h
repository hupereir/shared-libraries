#ifndef XmlTimeStamp_h
#define XmlTimeStamp_h

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

#include "TimeStamp.h"
#include "base_qt_export.h"

#include <QDomDocument>
#include <QDomElement>
#include <QString>

//* Xml interface to time manipulation object
class BASE_QT_EXPORT XmlTimeStamp:public TimeStamp
{

    public:

    //* empty constructor
    explicit XmlTimeStamp()
    {}

    //* constructor from DOM
    explicit XmlTimeStamp( const QDomElement& );

    //* create from timestamp
    explicit XmlTimeStamp( const TimeStamp& stamp ):
        TimeStamp( stamp )
    {}

    //* time_t creator
    explicit XmlTimeStamp( time_t time ):
        TimeStamp( time )
    {}

    //* get DOM element
    QDomElement domElement( const QString&, QDomDocument& ) const;

};

#endif
