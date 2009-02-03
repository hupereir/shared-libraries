#ifndef _ScrollObject_h_
#define _ScrollObject_h_

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
  \file ScrollObject.h
  \brief implements smooth scrolling on scroll area
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include <QAbstractScrollArea>
#include <QTimeLine>

#include <string>
#include <vector>

#include "Counter.h"

//! customized tree view
class ScrollObject: public QObject, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
    
  //! constructor
  ScrollObject( QAbstractScrollArea* );

  //! destructor
  virtual ~ScrollObject( void );
    
  //! event filter
  bool eventFilter( QObject*, QEvent* );
  
  //! enability
  void setEnabled( bool value )
  { enabled_ = value; }
  
  //! enability
  const bool& isEnabled( void ) const
  { return enabled_; }
  
  protected:  
  
  //! keypress event
  bool keyPressEvent( QKeyEvent* );
  
  //! keypress event
  bool keyReleaseEvent( QKeyEvent* );
  
  //! wheel event
  bool wheelEvent( QWheelEvent* );
  
  //! time line
  QTimeLine& _timeLine( void ) 
  { return time_line_; }
  
  //! target
  QAbstractScrollArea& _target( void ) const
  { return *target_; }
  
  //! starting point
  void _setStart( const QPoint& value )
  { start_ = value; }
 
  //! starting point
  const QPoint& _start( void ) const
  { return start_; }

  //! current point
  QPoint _current( void ) const;
 
  //! set current point
  bool _setCurrent( QPoint ) const;
  
  //! end point
  void _setStep( const QPointF& value )
  { step_ = value; }
 
  //! end point
  const QPointF& _step( void ) const
  { return step_; }
  
  //! auto_repeat
  void _setAutoRepeat( bool value )
  { auto_repeat_ = value; }
  
  //! auto_repeat
  bool _autoRepeat( void ) const
  { return auto_repeat_; }
  
  protected slots:

  //! animated scroll
  void _scroll( int );
  
  //! finished
  void _finished( void );
  
  private slots:
  
  void _updateConfiguration( void );
  
  private: 
  
  //! single step
  bool _pageStep( int );
  
  //! previous page
  bool _previousPage( void );
  
  //! next page
  bool _nextPage( void );
  
  //! scroll contents
  bool _scrollBy( QPoint );
  
  //! enability
  bool enabled_;
  
  //! scroll mode
  enum Mode
  {
    NONE = 0,
    HORIZONTAL = 1<<0,
    VERTICAL = 1<<1,
    BOTH = HORIZONTAL|VERTICAL
  };
  
  //! scroll mode
  unsigned int mode_;
  
  //! auto repeat
  bool auto_repeat_;
  
  // target
  QAbstractScrollArea* target_;
  
  // smooth scrolling
  QTimeLine time_line_;
  
  //! starting point
  QPoint start_;
  
  //! step
  QPointF step_;
  
};

#endif
