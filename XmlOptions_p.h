#ifndef XmlOptions_p_h
#define XmlOptions_p_h

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

#include "Options.h"
#include "File.h"
#include "XmlError.h"

//____________________________________________________________________
class XmlOptionsSingleton
{
    public:

    //! constructor
    XmlOptionsSingleton( void )
    { options_.installDefaultOptions(); }

    //! file
    const File& file( void ) const
    { return file_; }

    //! set file
    bool setFile( const File& file )
    {
        if( file_ == file ) return false;
        file_ = file;
        return true;
    }

    //! create backup
    void backup( void )
    { backup_ = options_; }

    //! true if options (or file) have been modified since last read/write
    bool modified( void ) const;

    //! options
    Options options_;

    //! backup
    Options backup_;

    //! error
    XmlError error_;

    private:

    //! return true if two sets of options differ
    bool _differs( const Options&, const Options& ) const;

    //! file
    File file_;

};

#endif
