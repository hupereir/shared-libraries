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
   \file    TimeStamp.cpp
   \brief   time manipulation object
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <time.h>
#include <sstream>

#include "Debug.h"
#include "Exception.h"
#include "Str.h"
#include "TimeStamp.h"
#include "Util.h"

using namespace std;

const string TimeStamp::days_[] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat" 
  }; 

const string TimeStamp::months_[] = {
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec" };

//_______________________________
const string TimeStamp::STAMP_UNKNOWN( "unknown" ); 

//_____________________________________
TimeStamp TimeStamp::now( void )
{
  Debug::Throw( "TimeStamp::now.\n" );
  TimeStamp out;
  out.time_ = time(0);
  out.tm_   = *localtime(&out.time_);
  out.is_valid_ = ( out.time_>=0 );   
  return out;
}

//___________________________________
string TimeStamp::string( TimeStamp::Format format ) const
{
  if( !(is_valid_ ) ) return STAMP_UNKNOWN;
  
  ostringstream o;
  
  switch (format) {
    
    case DATE:
    if( tm_.tm_mday < 10 ) o << "0";
    o << tm_.tm_mday << "/";
    if( tm_.tm_mon+1 < 10 ) o << "0"; 
    o << tm_.tm_mon+1 << "/";
    o << tm_.tm_year+1900;
    break;
    
    case DATE_US:
    o << tm_.tm_year+1900 << "/";
    if( tm_.tm_mon+1 < 10 ) o << "0"; 
    o << tm_.tm_mon+1 << "/";
    if( tm_.tm_mday < 10 ) o << "0";
    o << tm_.tm_mday;
    break;
    
    case TIME:
    if( tm_.tm_hour < 10 ) o << "0";
    o << tm_.tm_hour << ":";
    if( tm_.tm_min < 10 ) o << "0";
    o << tm_.tm_min;
    break;
    
    case SHORT:
    o << this->string( DATE ) << " " << this->string( TIME );
    break;
      
    case SHORT_US:
    o << this->string( DATE_US ) << " " << this->string( TIME );
    break;
    
    
    case LONG:
    if( tm_.tm_mday < 10 ) o << "0"; 
    o << tm_.tm_mday << " ";
    o << months_[tm_.tm_mon] << " ";
    o << tm_.tm_year+1900 << " ";
  
    o << this->string( TIME );
    o << ":";
    if( tm_.tm_sec < 10 ) o << "0"; 
    o << tm_.tm_sec << " ";
  
    o << "(" << days_[tm_.tm_wday] << ")";    
    break;
  
    case DATE_TAG:
    if( tm_.tm_mday < 10 ) o << "0";
    o << tm_.tm_mday << "_";
    if( tm_.tm_mon+1 < 10 ) o << "0";
    o << tm_.tm_mon+1 << "_";
    if( (tm_.tm_year%100)<10) o << "0"; 
    o << tm_.tm_year%100;
    break;
    
    case JOB_TAG:
    o << months_[tm_.tm_mon] << " ";
    if( tm_.tm_mday < 10 ) o << " ";
    o << tm_.tm_mday << " ";
    if( tm_.tm_hour < 10 ) o << "0";
    o << tm_.tm_hour << ":";
    if( tm_.tm_min < 10 ) o << "0";
    o << tm_.tm_min;
    break;
    
    default:
    throw runtime_error( DESCRIPTION( "unrecognized format" ) );
  
  }
  
  return o.str();
}
