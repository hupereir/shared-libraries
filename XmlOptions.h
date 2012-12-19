#ifndef XmlOptions_h
#define XmlOptions_h

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

#include "File.h"
#include "Options.h"
#include "XmlError.h"

//! forward declaration
class XmlOptionsSingleton;
class XmlDocument;

//! Option file parser based on xml
class XmlOptions
{

    public:

    //!@name accessors
    //@{

    //! options
    static Options& get( void );

    //! file
    static const File& file();

    //! error
    static const XmlError& error();

    //@}

    //!@name modifiers
    //@{

    //! file
    static void setFile( const QByteArray& array )
    { setFile( File( array ) ); }

    //! file
    static void setFile( const File& );

    //! error
    static void setError( const XmlError& );

    //! read options
    static bool read( void );

    //! write options
    static bool write( void );

    //@}

    protected:

    //! read
    static bool _read( const XmlDocument&, Options& );

    private:

    //! singleton
    static XmlOptionsSingleton singleton_;

};

#endif
