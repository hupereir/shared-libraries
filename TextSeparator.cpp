
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
  \file TextSeparator.cpp
  \brief separators between words
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/



#include "TextSeparator.h"

using namespace std;

//___________________________________
const TextSeparator& TextSeparator::get( void )
{
  static TextSeparator singleton;
  return singleton;
}

//___________________________________
TextSeparator::TextSeparator( void )
{

  // initialize separator
  const QString base_separators( " \t\n" );
  for( int i=0; i< base_separators.size(); i++ )
  base_separators_.insert( base_separators[i] );

  const QString extended_separators( ",;.:?!&+-=*/|-()'`{}[]<>\"\\%" );
  for( int i=0; i< extended_separators.size(); i++ )
  extended_separators_.insert( extended_separators[i] );

  separators_ = base_separators_;
  separators_.insert( extended_separators_.begin(), extended_separators_.end() );

}
