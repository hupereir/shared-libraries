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
   \file    Command.cpp
   \brief   retrieve commands from Options and format so that they match QProcess
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Command.h"
#include "Debug.h"

//_________________________________________________________
QStringList Command::parse( void ) const
{
  
  Debug::Throw( "Command::parse.\n" );
  int position( indexOf( "-" ) );
  if( position < 0 )
  { return QStringList() << trimmed(); }
    
  // split everything that is after the first dash, using white spaces as separator
  // and assign them as arguments.
  // note that this is not universal: if the arguments have filenames that includes white 
  // spaces, they will be also split.
  QStringList tmp( mid( position ).split( QRegExp("\\s+" ), SkipEmptyParts ) );
  return QStringList() << left( position ).trimmed() << tmp;

}
