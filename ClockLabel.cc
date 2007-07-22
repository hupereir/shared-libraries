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
  \file ClockLabel.cc
  \brief a self-updated label displaying current date and time 
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "ClockLabel.h"
#include "Debug.h"

using namespace std;

//__________________________________________________________
//! timestamp string format
static const TimeStamp::Format format( TimeStamp::JOB_TAG );

//__________________________________________________________
ClockTimer::ClockTimer( QWidget *parent, int time_out ):
  QTimer( parent ),
  Counter( "ClockTimer" )
{
  Debug::Throw( "ClockTimer::ClockTimer.\n" );
  connect( this, SIGNAL( timeout() ), this, SLOT( _checkCurrentTime() ) );
  start( 1000*time_out );
}

//__________________________________________________________
void ClockTimer::_checkCurrentTime( void )
{

  Debug::Throw( "ClockTimer::_CheckCurrentTime.\n" );
  TimeStamp new_time( TimeStamp::now() );
  
  // check time hour and/or minute differ
  if( 
    time_.isValid() && 
    new_time.hours() == time_.hours() && 
    new_time.minutes() == time_.minutes() && 
    new_time.seconds() == time_.seconds() ) 
  {
    time_ = new_time;
    return;
  }
  
  // update stored time
  Debug::Throw( "ClockTimer::_CheckCurrentTime - updating.\n" );
  time_ = new_time;
  
  // emit time changed signal
  emit timeChanged( time_.string(format ).c_str() );
  return;
  
}

//________________________________________________________________
ClockLabel::ClockLabel( QWidget* parent ):
  QLabel( parent ),
  Counter( "ClockLabel" )
{
    
  Debug::Throw( "ClockLabel::ClockLabel.\n" );
  
  // create static clock timer, updated every 10 seconds
  connect( &timer_, SIGNAL( timeChanged( const QString& ) ), this, SLOT( setText( const QString& ) ) );
  setText( TimeStamp::now().string( format ).c_str() );
  
}
