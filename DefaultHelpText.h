#ifndef DefaultHelpText_h
#define DefaultHelpText_h

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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include <QObject>

namespace Base
{
    //! default help text
    static const QString helpText[] =
    {

        //_________________________________________________________
        QObject::tr( "Contact" ),
        QObject::tr( "<h2>Contact</h2>\n"
        "<p>Latest software versions can be downloaded at:</p>"
        "<p><a href=\"http://www.phenix.bnl.gov/WWW/publish/hpereira/software/index.php?page=introduction\">http://www.phenix.bnl.gov/WWW/publish/hpereira/software/index.php?page=introduction</a></p>"
        "<p>For suggestions and bug reports, please contact the author directly at:</p>"
        "<p><a href=\"mailto:hugo.pereira@free.fr\">hugo.pereira@free.fr</a></p>" ),
        0

    };
};

#endif
