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
  \file ClockLabel.cpp
  \brief a self-updated label displaying current date and time 
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "ClockLabel.h"
#include "Debug.h"
#include "TransitionWidget.h"

using namespace std;

//__________________________________________________________
//! timestamp string format
static const TimeStamp::Format format( TimeStamp::JOB_TAG );

//__________________________________________________________
ClockTimer::ClockTimer( QWidget *parent ):
  QTimer( parent ),
  Counter( "ClockTimer" )
{
  Debug::Throw( "ClockTimer::ClockTimer.\n" );
  connect( this, SIGNAL( timeout() ), SLOT( _checkCurrentTime() ) );
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
  
  time_ = new_time;
  setInterval( 1000 * interval() );
  
  // emit time changed signal
  emit timeChanged( time_.toString( format ) );
  return;
  
}

//________________________________________________________________
ClockLabel::ClockLabel( QWidget* parent ):
  AnimatedLabel( parent ),
  timer_( this )
{
    
  Debug::Throw( "ClockLabel::ClockLabel.\n" );
  
  // needed for proper background to be set
  transitionWidget().setFlag( TransitionWidget::FROM_PARENT, true );
 
  // create static clock timer, updated every 10 seconds
  connect( &timer_, SIGNAL( timeChanged( const QString& ) ), SLOT( setText( const QString& ) ) );
  
  timer_.setSingleShot( false );
  timer_.setInterval( 1000 * ClockTimer::interval() );
  timer_.start();
  
  setText( TimeStamp::now().toString( format ) );
  
}
