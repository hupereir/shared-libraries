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
   \file    Util.cc
   \brief   some facilities used everywhere
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include "Util.h"
#include "Str.h"
#include "Debug.h"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/types.h>

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
void Util::run( const string& value )
{
  if( !value.size() ) return;
  system( value.c_str() );
  return;
}     

//____________________________________________  
void Util::runAt( const string& path, const string& value )
{
  if( !value.size() ) return;
  
  ostringstream command;
  command << "cd " << path << "; " << value;
  system( command.str().c_str() );
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
   Debug::Throw() << "Util::env.\n";
   if( !getenv( val.c_str() ) ) return default_value;
   return string( getenv( val.c_str() ) );
}  
    
//______________________________________________________________________
string Util::user( void )
{
   struct passwd *pass = getpwuid( getuid() );
   return string(pass->pw_name);
}  
   
//______________________________________________________________________
string Util::domain( void )
{
   char *buf = new char[ LONGSTR ];

   if( !buf ) return "";

   getdomainname( buf, LONGSTR );
   string out( buf );
   delete[] buf;
   return out;
   
}  

//______________________________________________________________________
string Util::host( bool short_name )
{
  struct utsname name;
  uname( &name );
  Debug::Throw() << "Util::GetHost. Got " << name.nodename << endl;
  string out(name.nodename);
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
{
  // Get PWD
  char* buf = new char[LONGSTR];
  if (!getcwd(buf, LONGSTR)) {
    cerr << "Util::GetWorkingDirectory - ERROR: getcwd failed\n";
    return ".";
  }
  
  string out( buf );
  delete[] buf;
  return out;
}

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
