#ifndef XmlOptions_h
#define XmlOptions_h

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

#include "File.h"
#include "Options.h"
#include "base_qt_export.h"

//* forward declaration
class XmlDocument;
class XmlError;

namespace Private
{
    class XmlOptionsSingleton;
}

//* Option file parser based on xml
class BASE_QT_EXPORT XmlOptions
{

    public:

    //*@name accessors
    //@{

    //* options
    static Options& get();

    //* file
    static const File& file();

    //* error
    static const XmlError& error();

    //@}

    //*@name modifiers
    //@{

    //* file
    static void setFile( const QByteArray& array )
    { setFile( File( array ) ); }

    //* file
    static void setFile( const File& );

    //* read options
    static bool read();

    //* write options
    static bool write();

    //@}

    protected:

    //* read
    static bool _read( const XmlDocument&, Options& );

    private:

    //* singleton
    static Private::XmlOptionsSingleton& _singleton();

};

#endif
