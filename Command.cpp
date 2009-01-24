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
#include "CustomProcess.h"
#include "Debug.h"

using namespace std;

//____________________________________________  
void Command::run( const QString& path ) const
{ 

  CustomProcess *p = new CustomProcess();
  p->setAutoDelete();
  if( !path.isEmpty() ) p->setWorkingDirectory( path );
  p->start( *this );
  return;
  
}

//_________________________________________________________
QStringList Command::_parse( const QString &in ) const
{
  
  Debug::Throw( "Command::parse.\n" );

  // first split using '"' as separator to get "single sting" arguments that contain strings
  QStringList local = in.split( "\"", QString::KeepEmptyParts );
  QStringList out;
  
  // split strings that are not enclosed into quotes using white spaces as separator
  bool split( true );
  for( QStringList::const_iterator iter = local.begin(); iter != local.end(); iter++ )
  { 
    if( !iter->isEmpty() )
    {
      if( split ) out << iter->split( QRegExp( "\\s+" ), QString::SkipEmptyParts );
      else out << *iter;
    }
    
    split = !split;
  }

  // print output
  if( Debug::level() >= 1 )
  {
    for( QStringList::const_iterator iter = out.begin(); iter != out.end(); iter++ )
    { Debug::Throw() << "Command::parse - \"" << qPrintable( *iter ) << "\"" << endl; }
  }
  
  return out;
  
}

