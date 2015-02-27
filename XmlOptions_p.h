#ifndef XmlOptions_p_h
#define XmlOptions_p_h

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

#include "Options.h"
#include "File.h"
#include "XmlError.h"

//____________________________________________________________________
class XmlOptionsSingleton
{
    public:

    //* constructor
    XmlOptionsSingleton( void )
    { options_.installDefaultOptions(); }

    //* file
    const File& file( void ) const
    { return file_; }

    //* set file
    bool setFile( const File& file )
    {
        if( file_ == file ) return false;
        file_ = file;
        return true;
    }

    //* true if options have changed with respect to current
    /*! read-only options are ignored, as well as default values and default flags */
    bool differs( const Options& other ) const
    { return _differs( options_, other ) || _differs( other, options_ ); }

    //* options
    Options options_;

    //* error
    XmlError error_;

    private:

    //* return true if two sets of options differ
    bool _differs( const Options&, const Options& ) const;

    //* file
    File file_;

};

#endif
