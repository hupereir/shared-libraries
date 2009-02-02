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
  mode_( NONE ),
  target_( parent )
{
  Debug::Throw( "ScrollObject::ScrollObject.\n" );   

  assert( parent );
  
  // configuration
  connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  _updateConfiguration();
  
  // connect time line
  //_timeLine().setCurveShape( QTimeLine::LinearCurve );
  connect( &_timeLine(), SIGNAL( frameChanged( int ) ), this, SLOT( _scroll( int ) ) ); 
  connect( &_timeLine(), SIGNAL( finished( void ) ), this, SLOT( _finished( void )) ); 
  
  // install event filters
  _target().viewport()->installEventFilter(this);
  _target().installEventFilter(this);
  
}

//_______________________________________________
ScrollObject::~ScrollObject( void )
{ Debug::Throw( "ScrollObject::~ScrollObject.\n" );  }

 
//_______________________________________________
bool ScrollObject::eventFilter( QObject* object, QEvent* event)
{
  
  Debug::Throw( "ScrollObject::eventFilter.\n" );

  if( !isEnabled() ) return false;
  
  if( object != &_target() && object != _target().viewport() ) return false;

  // check event type
  switch( event->type() )
  {
    
    case QEvent::KeyPress:
    return keyPressEvent( dynamic_cast<QKeyEvent*>( event ) );
    
    case QEvent::Wheel:
    return wheelEvent( dynamic_cast<QWheelEvent*>( event ) );
    
    default: return false;
  }
  
}

//_______________________________________________
bool ScrollObject::keyPressEvent( QKeyEvent* event )
{
  
  Debug::Throw( 0, "ScrollObject::keyPressEvent.\n" );
  
  // check event
  if( !event ) return false;
  
  // check key against page up or page down
  if( event->modifiers() != Qt::NoModifier ) return false;
  if( event->key() == Qt::Key_PageUp ) return _previousPage();
  if( event->key() == Qt::Key_PageDown ) return _nextPage();
  return false;
  
}

//_______________________________________________
bool ScrollObject::wheelEvent( QWheelEvent* event )
{

  Debug::Throw( "ScrollObject::wheelEvent.\n" );
 
  // check event
  if( !event ) return false;

  // check key against page up or page down
  if( event->modifiers() != Qt::NoModifier ) return false;
  return _singleStep( event->delta() / 8 );  

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
  if( (mode_&HORIZONTAL) && _target().horizontalScrollBar() ) 
  {
    if( position.x() < _target().horizontalScrollBar()->minimum() ) position.setX( _target().horizontalScrollBar()->minimum() );
    else if( position.x() > _target().verticalScrollBar()->maximum() ) position.setX( _target().horizontalScrollBar()->maximum() );
    else accepted = true;
    _target().horizontalScrollBar()->setValue( position.x() );
  }
  
  if( (mode_&VERTICAL) && _target().verticalScrollBar() ) 
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

  // stop if needed
  if( frame >= _timeLine().endFrame() ) { 
    frame = _timeLine().endFrame();
    _timeLine().stop();
  }

  // calculate current point  
  if( !_setCurrent( QPoint(
    _start().x() + _step().x()*frame,
    _start().y() + _step().y()*frame ) ) )
  { _timeLine().stop(); }
  
}

//_____________________________________________________________________
void ScrollObject::_finished( void )
{ Debug::Throw( 0, "ScrollObject::_finished.\n" ); }

//_____________________________________________________________________
void ScrollObject::_updateConfiguration( void )
{
  Debug::Throw( "ScrollObject::_updateConfiguration.\n" );
        
  // smooth scrolling
  setEnabled(  XmlOptions::get().get<bool>( "ENABLE_ANIMATIONS" ) );
  _timeLine().setDuration( XmlOptions::get().get<int>( "ANIMATION_DURATION" ) );
  _timeLine().setFrameRange( 0, XmlOptions::get().get<int>( "ANIMATION_FRAMES" ) );
  
}

//_____________________________________________________________________
bool ScrollObject::_singleStep( int delta )
{ return ( _target().verticalScrollBar() && _scrollBy( QPoint( 0, -delta*_target().verticalScrollBar()->singleStep() ) ) ); }

//_____________________________________________________________________
bool ScrollObject::_previousPage( void )
{ return ( _target().verticalScrollBar() && _scrollBy( QPoint( 0, -_target().verticalScrollBar()->pageStep() ) ) ); }

//_____________________________________________________________________
bool ScrollObject::_nextPage( void )
{ return ( _target().verticalScrollBar() && _scrollBy( QPoint( 0, _target().verticalScrollBar()->pageStep() ) ) ); }

//_____________________________________________________________________
bool ScrollObject::_scrollBy( QPoint value )
{
  mode_ = NONE;
  if( value.x() ) mode_ |= HORIZONTAL;
  if( value.y() ) mode_ |= VERTICAL;
  _setStart( _current() );
  _setStep( QPointF( 
    qreal( value.x() )/_timeLine().endFrame(),
    qreal( value.y() )/_timeLine().endFrame()
    ) );

  if( _timeLine().state() != QTimeLine::NotRunning ) _timeLine().stop();
  _timeLine().start();
  return true;
}
