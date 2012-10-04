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

//! Option file parser based on xml
class XmlOptions
{

    public:

    //! retrieve singleton
    static Options& get( void );

    //! read from file
    static bool read( const QByteArray& array )
    { return read( File( array ) ); }

    //! read from file
    static bool read( File = File() );

    //! write options to file
    static bool write( File = File() );

    //!@name accessors
    //@{

    //! file
    static const File& file()
    { return file_; }

    //! error
    static const XmlError& error()
    { return error_; }

    //@}

    //!@name modifiers
    //@{

    //! file
    static void setFile( const File& );

    //! error
    static void setError( const XmlError& error )
    { error_ = error; }

    private:

    //! file
    static File file_;

    //! error
    static XmlError error_;

};

#endif
