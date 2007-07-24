#ifndef MultipleClickCounter_h
#define MultipleClickCounter_h

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
  \file MultipleClickCounter.h
  \brief handles multiple clicks and timeout
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QTimer>

#include "Counter.h"
#include "Debug.h"

//! handles multiple clicks and timeout
class MultipleClickCounter: public QTimer, public Counter
{
  
  Q_OBJECT
  
  public:
  
  //! constructor
  MultipleClickCounter( QObject* parent ):
    QTimer( parent ),
    Counter( "MultipleClickCounter" ),
    count_( 0 )
  { 
    Debug::Throw( "MultipleClickCounter::MultipleClickCounter.\n" ); 
    connect( this, SIGNAL( timeout() ), this, SLOT( _reset() ) );
    setSingleShot( true );
  }
  
  //! destructor
  virtual ~MultipleClickCounter( void )
  { Debug::Throw( "MultipleClickCounter::~MultipleClickCounter.\n" ); }
  
  //! increment counter
  void increment( void )
  { 
    count_++; 
    if( count_ > MAX_COUNT ) count_ == 1;
    if( count_ == 0 ) start( QApplication::doubleClickInterval() );
  }
  
  //! counter
  const unsigned int& counts( void ) const
  { return count_; }
  
  private slots:
  
  //! reset
  void _reset( void )
  { count_ = 0; }
  
  private:
  
  //! max number of clicks
  enum {MAX_COUNT = 4};
  
  //! current number of clicks
  unsigned int count_; 
  
};

#endif
