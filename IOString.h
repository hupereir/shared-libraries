#ifndef IOString_h
#define IOString_h

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

/*!
   \file IOString.h
   \brief read bytes from an IODevice and store into a string
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QIODevice>
#include <QString>

/*!
   \class IOString
   \brief read bytes from an IODevice and store into a string
*/

class IOString: public QString
{

  public:

  //! constructor
  IOString( QIODevice& device );

};

#endif
