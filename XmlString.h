#ifndef XmlString_h
#define XmlString_h

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
\file XmlString.h
\brief Some Xml utilities
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <list>
#include <QString>

/*!
\class XmlString
\brief Some Xml utilities
*/

class XmlString: public QString
{
    
    public:
    
    //! constructor
    XmlString( const QString& value ):
        QString( value )
    {}
        
    //! convert input string from text to xml
    QString toXml( void ) const;

    //! convert input string from xml to text
    QString toText( void ) const;

    private:

    //! text to Xml conversion pair type
    typedef std::pair<QString, QString> Conversion;

    //! text to Xml conversion pair type
    typedef std::list< Conversion > ConversionList;

    //! initialize Xml to text conversion pair list
    void _initConversions( void ) const;

    //! text to Xml conversion pair list
    static ConversionList& _conversions( void );


};

#endif
