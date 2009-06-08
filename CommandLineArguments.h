#ifndef CommandLineArguments_h
#define CommandLineArguments_h

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
  \file CommandLineArguments.h
  \brief stores command line arguments
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QStringList>

#include "Counter.h"
#include "Debug.h"

/*! \brief 
  parse command line argument.
  first argument is ignored (application name)
  the raw argument list can be accessed as well as
  tagged [using - or --] arguments and associated options
*/
class CommandLineArguments: public QStringList, public Counter
{

  public:
  
  //! constructor
  CommandLineArguments( int argc = 0, char* argv[] = 0 );  
  
  //! constructor
  CommandLineArguments( const QStringList& );
  
  //! return arg counts
  int argc( void ) const
  { return size(); }
  
  //! return argv
  char** argv( void ) const;
  
};

#endif
