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
  \file ScrollObject.cpp
  \brief customized Tree View
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include <QEvent>
#include <QKeyEvent>
#include <QScrollBar>
#include <QWheelEvent>

#include "ScrollObject.h"
#include "Singleton.h"
#include "XmlOptions.h"

using namespace std;
using namespace Qt;

//_______________________________________________
ScrollObject::ScrollObject( QAbstractScrollArea* parent ):
  QObject( parent ),
  Counter( "ScrollObject" ),
  enabled_( true ),
  mode_(0),
  auto_repeat_( false ),
  target_( parent )
{
  Debug::Throw( "ScrollObject::ScrollObject.\n" );   

  assert( parent );
  
  // configuration
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
  
  // connect time line
  _timeLine().setCurveShape( QTimeLine::LinearCurve );
  connect( &_timeLine(), SIGNAL( frameChanged( int ) ), this, SLOT( _scroll( int ) ) ); 
  connect( &_timeLine(), SIGNAL( finished( void ) ), this, SLOT( _finished( void )) ); 
  
  targets_.insert( &_target() );
  targets_.insert( _target().viewport() );
  targets_.insert( _target().verticalScrollBar() );
  targets_.insert( _target().horizontalScrollBar() );
  
  // install filters
  for( ObjectSet::iterator iter = targets_.begin(); iter != targets_.end(); iter++ )
  { (*iter)->installEventFilter(this); }
  
}

//_______________________________________________
ScrollObject::~ScrollObject( void )
{ Debug::Throw( "ScrollObject::~ScrollObject.\n" );  }

 
//_______________________________________________
bool ScrollObject::eventFilter( QObject* object, QEvent* event)
{
  
  // check enability
  if( !isEnabled() ) return false;
  if( targets_.find( object ) == targets_.end() ) return false;
  
  // check event type
  switch( event->type() )
  {
    
    case QEvent::KeyPress:
    return keyPressEvent( dynamic_cast<QKeyEvent*>( event ) );
    
    case QEvent::KeyRelease:
    return keyReleaseEvent( static_cast<QKeyEvent*>( event ) );
    
    case QEvent::Wheel:
    {
      Qt::Orientation orientation( object == _target().horizontalScrollBar() ? Qt::Horizontal : Qt::Vertical );
      return wheelEvent( static_cast<QWheelEvent*>( event), orientation );
    }
    
    case QEvent::MouseButtonPress:
    return mousePressEvent( static_cast<QMouseEvent*>( event ) );
    
    default: return false;
  }
  
}

//_______________________________________________
bool ScrollObject::keyPressEvent( QKeyEvent* event )
{
   
  // check key against page up or page down
  if( event->modifiers() != Qt::NoModifier ) return false;
  if( event->key() == Qt::Key_PageUp ) { _setAutoRepeat( true ); return _previousPage( Qt::Vertical ); }
  if( event->key() == Qt::Key_PageDown ) { _setAutoRepeat( true ); return _nextPage( Qt::Vertical ); }
  return false;
  
}

//_______________________________________________
bool ScrollObject::keyReleaseEvent( QKeyEvent* event )
{
  _setAutoRepeat( false );
  return false;
}

//_______________________________________________
bool ScrollObject::wheelEvent( QWheelEvent* event, Qt::Orientation orientation )
{

  // check key against page up or page down
  if( event->modifiers() != Qt::NoModifier ) return false;
  if( _timeLine().state() == QTimeLine::Running ) _setAutoRepeat( true );
  
  // factor 3 here is not understood. It was added to mimic at best 
  // the scale when animation is turned off. Need to check against Qt sources
  return _singleStep( (3*event->delta())/120, orientation );  

}
  
//_______________________________________________
bool ScrollObject::mousePressEvent( QMouseEvent* )
{
   
  _timeLine().stop();
  _setAutoRepeat( false );
  return false;
  
}

//_____________________________________________________
QPoint ScrollObject::_current( void ) const
{
  return QPoint( 
    _target().horizontalScrollBar() ? _target().horizontalScrollBar()->value():0,
    _target().verticalScrollBar() ? _target().verticalScrollBar()->value():0 );
}

//_____________________________________________________
bool ScrollObject::_setCurrent( QPoint position ) const
{
  
  bool accepted( false );
  if( (mode_&Qt::Horizontal) && _target().horizontalScrollBar() ) 
  {
    if( position.x() < _target().horizontalScrollBar()->minimum() ) position.setX( _target().horizontalScrollBar()->minimum() );
    else if( position.x() > _target().verticalScrollBar()->maximum() ) position.setX( _target().horizontalScrollBar()->maximum() );
    else accepted = true;
    _target().horizontalScrollBar()->setValue( position.x() );
  }
  
  if( (mode_&Qt::Vertical) && _target().verticalScrollBar() ) 
  {
    if( position.y() < _target().verticalScrollBar()->minimum() ) position.setY( _target().verticalScrollBar()->minimum() );
    else if( position.y() > _target().verticalScrollBar()->maximum() ) position.setY( _target().verticalScrollBar()->maximum() );
    else accepted = true;
    _target().verticalScrollBar()->setValue( position.y() );
  }
  
  return accepted;
  
}

//_____________________________________________________________________
void ScrollObject::_scroll( int frame )
{

  // calculate current point  
  if( !_setCurrent( QPoint(
    _start().x() + _step().x()*frame,
    _start().y() + _step().y()*frame ) ) )
  { _timeLine().stop(); }
  
}

//_____________________________________________________________________
void ScrollObject::_finished( void )
{ 
  Debug::Throw( "ScrollObject::_finished.\n" ); 
  if( _autoRepeat() )
  {
    _setAutoRepeat( false );
    _setStart( _current() );
    _timeLine().start();
  }
  
}

//_____________________________________________________________________
void ScrollObject::_updateConfiguration( void )
{
  Debug::Throw( "ScrollObject::_updateConfiguration.\n" );
        
  // smooth scrolling
  setEnabled(  XmlOptions::get().get<bool>( "SMOOTH_SCROLLING_ENABLED" ) );
  _timeLine().setDuration( XmlOptions::get().get<int>( "SMOOTH_SCROLLING_DURATION" ) );
  _timeLine().setFrameRange( 0, XmlOptions::get().get<int>( "ANIMATION_FRAMES" ) );
  
}

//_____________________________________________________________________
bool ScrollObject::_singleStep( int delta, unsigned int orientation )
{ 

  if( !delta ) return false;
  
  QPoint offset(0,0);
  bool accepted = false;
  if( (orientation & Qt::Horizontal) && _target().horizontalScrollBar() )
  { 
    accepted = true; 
    offset.setX( -delta*_target().horizontalScrollBar()->singleStep() );
  } 
  
  if( (orientation & Qt::Vertical) && _target().verticalScrollBar() )
  { 
    accepted = true;
    offset.setY( -delta*_target().verticalScrollBar()->singleStep() );
  }
  
  return accepted && _scrollBy( offset );
  
}

//_____________________________________________________________________
bool ScrollObject::_pageStep( int delta, unsigned int mode )
{ 

  if( !delta ) return false;
  
  QPoint offset(0,0);
  bool accepted = false;
  if( ( mode & Qt::Horizontal ) && _target().horizontalScrollBar() )
  { 
    accepted = true; 
    offset.setX( -delta*_target().horizontalScrollBar()->pageStep() );
  }
  
  if( ( mode & Qt::Vertical ) && _target().verticalScrollBar() )
  { 
    accepted = true;
    offset.setY( -delta*_target().verticalScrollBar()->pageStep() );
  }
  
  return accepted && _scrollBy( offset );
  
}

//_____________________________________________________________________
bool ScrollObject::_previousPage( unsigned int mode )
{ return _pageStep(1, mode ); }

//_____________________________________________________________________
bool ScrollObject::_nextPage( unsigned int mode )
{ return _pageStep(-1, mode); }

//_____________________________________________________________________
bool ScrollObject::_scrollBy( QPoint value )
{
  
  // set mode based on offset values
  mode_ = 0;
  if( value.x() ) mode_ |= Qt::Horizontal;
  if( value.y() ) mode_ |= Qt::Vertical;
  
  _setStart( _current() );
  _setStep( QPointF( 
    qreal( value.x() )/_timeLine().endFrame(),
    qreal( value.y() )/_timeLine().endFrame()
    ) );

  if( _timeLine().state() == QTimeLine::NotRunning ) _timeLine().start();
  else if( !_autoRepeat() ) {
    _timeLine().stop();
    _timeLine().start();
  }
  
  return true;
}
