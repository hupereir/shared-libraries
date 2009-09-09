#ifndef KeyModifier_h
#define KeyModifier_h

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
  \file    KeyModifier.h
  \brief   key state of given modifier
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include <Qt>

class KeyModifier
{

  public:

  //! constructor
  KeyModifier( const Qt::Key& );

  //! state
  enum State
  {
    OFF,
    ON,
    UNKNOWN
  };

  //! get state
  State state( void ) const;

  private:

  //! key
  Qt::Key key_;

};

#endif
