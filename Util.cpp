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
   \file    Util.cpp
   \brief   some facilities used everywhere
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QProcess>
#include <QStringList>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include "CustomProcess.h"
#include "Util.h"
#include "Str.h"
#include "Debug.h"

/*! \brief
  static 'long' string size 
  for c_like system string access
*/

static const int LONGSTR = 256;

using namespace std;

//____________________________________________  
string Util::timeStamp( void )
{
  time_t now( time(0) );
  struct tm time_struct( *localtime(&now) );  
  
  ostringstream o;
  o << time_struct.tm_mday << "-"
    << time_struct.tm_mon << "-"
    << time_struct.tm_year << "_"
    << time_struct.tm_hour << "-"
    << time_struct.tm_min << "-"
    << time_struct.tm_sec;
  return o.str();
}

//____________________________________________  
string Util::timeString( void )
{
  time_t now( time(0) );
  struct tm time_struct( *localtime(&now) );  
  
  ostringstream o;
  if( time_struct.tm_mday < 10 ) o << "0";  o << time_struct.tm_mday << "/";
  if( time_struct.tm_mon +1 < 10 ) o << "0";  o << time_struct.tm_mon+1 << "/";
  o << time_struct.tm_year+1900 << " ";
  if( time_struct.tm_hour < 10 ) o << "0";  o << time_struct.tm_hour << ":";
  if( time_struct.tm_min < 10 ) o << "0";   o << time_struct.tm_min << ":";
  if( time_struct.tm_sec < 10 ) o << "0";   o << time_struct.tm_sec;
  return o.str();
}

//____________________________________________  
bool Util::run( const string& value )
{
  Debug::Throw( "Util::run" );
  return run( QString( value.c_str() ).split( QRegExp("\\s"), QString::SkipEmptyParts ) );
}
  
//____________________________________________  
bool Util::run( QStringList arguments )
{
  Debug::Throw( "Util::run" );
  if( arguments.empty() ) return false;
  
  // dump arguments for debugging
  if( Debug::level() > 0 )
  {
    for( QStringList::iterator iter = arguments.begin(); iter != arguments.end(); iter++ )
    { Debug::Throw() << "Util::run - argument: \"" << qPrintable( *iter ) << "\"" << endl; }  
  }
  
  QString program( arguments.front() );
  arguments.pop_front();
  if( arguments.empty() ) return QProcess::startDetached( program );
  else return QProcess::startDetached( program, arguments );
 
}     

//____________________________________________  
void Util::runAt( const string& path, const string& value )
{ return runAt( path, QString( value.c_str() ).split( QRegExp("\\s"), QString::SkipEmptyParts ) ); }

//____________________________________________  
void Util::runAt( const string& path, QStringList arguments )
{ 

  // dump arguments for debugging
  if( Debug::level() > 0 )
  {
    for( QStringList::iterator iter = arguments.begin(); iter != arguments.end(); iter++ )
    { Debug::Throw(0) << "Util::runAt - argument: \"" << qPrintable( *iter ) << "\"" << endl; }  
  }
  
  CustomProcess *p = new CustomProcess();
  p->setAutoDelete();
  p->setWorkingDirectory( path.c_str() );
  p->start( arguments );
  return;
}     

//______________________________________________________________________
list< unsigned int > Util::allChildren( const unsigned int pid_ )
{
  list< unsigned int > vout;
  string command = string( "ps -o pid -o ppid -u ")+user();

  FILE *tmp = popen( command.c_str(), "r" );
  
  static const int linesize( 128 );
  char* buf = new char[linesize];
  while( fgets( buf, linesize, tmp ) ) {
    
    if( !( buf && strlen( buf ) ) ) continue;

    unsigned int pid, ppid;
    istringstream i( buf ); i >> pid >> ppid;
    if( i.rdstate() & std::ios::failbit ) continue;
    if( ppid == pid_ ) {
      vout.push_back( pid );
      list< unsigned int > vsub = allChildren( pid );
      for( list<unsigned int>::iterator it=vsub.begin(); it != vsub.end(); it++ ) vout.push_back( *it );
      vsub.clear();
    }
  }

  delete[] buf;
   pclose( tmp );
  
  return vout;
}
   
//______________________________________________________________________
string Util::env( const string& val, const string& default_value )
{
  
  QStringList environment( QProcess::systemEnvironment() );
  for( QStringList::iterator iter = environment.begin(); iter != environment.end(); iter++ )
  {
    int position( iter->indexOf( "=" ) );
    if( position <= 0 ) continue;
    
    QString var( iter->left( position ) );
    if( var == QString( val.c_str() ) ) return qPrintable( iter->mid( position+1 ) );
  }
  
  return default_value;
  
}  
    
//______________________________________________________________________
string Util::user( void )
{ 
  Debug::Throw( "Util::user.\n" ); 
  #ifdef Q_WS_WIN32
  return env( "USERNAME", "unknown user" ); 
  #else 
  // first try look for USERNAME environment variable
  // if failed, try USER
  // if failed, return unknown.
  return env( "USERNAME", env( "USER", "unknown user" ) ); 
  #endif
}  
   
//______________________________________________________________________
string Util::domain( void )
{
  Debug::Throw( "Util::domain.\n" ); 
  
  #ifdef Q_WS_X11
  
  // use build-in unix function
  char *buf = new char[ LONGSTR ];
  if( !buf ) return "";
  getdomainname( buf, LONGSTR );
  string out( buf );
  delete[] buf;
  return out;
  
  #else
  
  // use system environment.
  // should work for windows
  return env( "USERDOMAIN","localdomain");
  
  #endif
  
}  

//______________________________________________________________________
string Util::home( void )
{
  Debug::Throw( "Util::home.\n" );
  
  #ifdef Q_WS_WIN32
  
  // use drive+path for windows
  string drive( env( "HOMEDRIVE","C:" ) );
  string home( env( "HOMEPATH","/"));
  return drive+home;
  #else 
  
  // use system environment
  return env( "HOME","." ); 
  
  #endif
  
}

//______________________________________________________________________
string Util::tmp( void )
{
  Debug::Throw( "Util::tmp.\n" );
  
  #ifdef Q_WS_WIN32
  
  // use system environment
  return env( "TEMP", env( "TMP", "C:/tmp" ) );
  
  #else 
  
  // use hard-coded linux /tmp path
  return "/tmp"; 
  
  #endif
  
}
//______________________________________________________________________
string Util::host( bool short_name )
{

  // use system environment
  // it does not work for windows
  Debug::Throw( "Util::host.\n" ); 
  #ifdef Q_WS_WIN32
  string out( env( "COMPUTERNAME", "unknown" ) );
  #else
  string out( env( "HOSTNAME", "unknown" ) );
  #endif
  
  if( ! short_name ) return out;

  size_t pos( out.find( "." ) ); 
  if( pos == string::npos ) return out;
  return out.substr( 0, pos );
  
}
 
//______________________________________________________________________
int Util::pid( void )
{ return getpid(); }
     
//______________________________________________________________________
string Util::workingDirectory( void )
{ return env( "PWD", "." ); }

//______________________________________________________________________
string Util::convertTime( float value )
{
  int sec = int(value);
  int week = sec/(7*24*60*60);  sec%=(7*24*60*60);
  int day = sec/(24*60*60);     sec%=(24*60*60); 
  int hour = sec/(60*60);       sec%=(60*60); 
  int min  = sec/60;            sec%=60;
  ostringstream out;
  
  if( week ) out << ">" << week << " week" << ( week>1 ? "s":"");
  else if( day ) out << ">"  << day << " day" << ( day>1 ? "s":"");
  else {
    if( hour ) out << hour << "h ";
    if( min ) out << min << "m ";
    if( sec ) out << sec;
    else out << "0";
    out << "s";
  }
  
  return out.str();
}
