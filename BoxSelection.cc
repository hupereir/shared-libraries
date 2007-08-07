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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file BoxSelection.cc
  \brief Box selection
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "BoxSelection.h"
#include "CustomTextEdit.h"

using namespace std;

//________________________________________________________________________
BoxSelection::BoxSelection( CustomTextEdit* parent ):
  Counter( "BoxSelection" ),
  parent_( parent ),
  state_( EMPTY )
{ Debug::Throw( 0, "BoxSelection::BoxSelection.\n" ); }

//________________________________________________________________________
bool BoxSelection::start( const QPoint& point )
{
  Debug::Throw( 0, "BoxSelection::start.\n" );
  if( state_ != EMPTY ) return false;
  
  start_ = point;
  state_ = STARTED;
  return true;
}
 
//________________________________________________________________________
bool BoxSelection::update( const QPoint& point )
{ 
  Debug::Throw( 0, "BoxSelection::update.\n" );
  if( state_ != STARTED ) return false;
  
  end_ = point;
  return true;
}

//________________________________________________________________________
bool BoxSelection::finish( const QPoint& point )
{ 
  Debug::Throw( 0, "BoxSelection::finish.\n" );
  if( state_ != STARTED ) return false;
  
  state_ = FINISHED;
  end_ = point;
  return true;
}

//________________________________________________________________________
bool BoxSelection::clear( void )
{ 
  Debug::Throw( 0, "BoxSelection::clear.\n" );
  if( state_ != FINISHED ) return false;
  state_ = EMPTY;
  return true;
}
