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
  \file CustomProcess.cpp
  \brief customized process to store stdout/stderr in full/flushed streams
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "CustomProcess.h"
#include "Debug.h"

using namespace std;

//____________________________________________________
CustomProcess::CustomProcess( QObject* parent ):
  QProcess( parent ),
  Counter( "CustomProcess" )
{}

//____________________________________________________
CustomProcess::~CustomProcess( void )
{ if( state() != QProcess::NotRunning ) kill(); }

//____________________________________________________
void CustomProcess::start( const string& arguments, OpenMode mode )
{
  Debug::Throw() << "CustomProcess::start - " << arguments << endl;
  return start( QString( arguments.c_str() ).split( QRegExp("\\s"), QString::SkipEmptyParts ), mode );
}

//____________________________________________________
void CustomProcess::start( const QStringList& arguments, OpenMode mode )
{
  
  if( arguments.empty() ) return;
  
  QString program( arguments.front() );
  QStringList local_args( arguments );
  local_args.pop_front();
  Debug::Throw(0) << "CustomProcess::start - (qstringlist)"
    << " program: " << qPrintable( program ) 
    << " arguments: " << qPrintable( local_args.join( " " ) )
    << endl;
  
  if( local_args.empty() ) return QProcess::start( program, mode );
  else return QProcess::start( program, local_args, mode );
  
}

//______________________________________________________________
void CustomProcess::setAutoDelete( void )
{ 
  Debug::Throw( "CustomProcess::setAutoDelete.\n" );
  connect( this, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( deleteLater() ) ); 
}

//______________________________________________________________
string CustomProcess::errorMessage( ProcessError error )
{ 
  switch( error )
  {
    
    case FailedToStart: return "Process failed to start";
    case Crashed: return "Process crashed";
    case Timedout: return "Process timed out";
    case WriteError: return "Process ended due to write error";
    case ReadError: return "Process ended du to read error";  
    default: return "Process ended with unknown error";
  }
}
