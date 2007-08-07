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
  \file BoxSelection.cc
  \brief Box selection
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "BoxSelection.h"
#include "CustomTextEdit.h"

#include <QPainter>
#include <QScrollBar>

using namespace std;

//________________________________________________________________________
BoxSelection::BoxSelection( CustomTextEdit* parent ):
  Counter( "BoxSelection" ),
  parent_( parent ),
  state_( EMPTY )
{ Debug::Throw( "BoxSelection::BoxSelection.\n" ); }  
  
//________________________________________________________________________
bool BoxSelection::start( QPoint point )
{
  Debug::Throw( "BoxSelection::start.\n" );
  if( state_ != EMPTY ) return false;
  
  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );
  
  begin_ = point;
  end_ = point;
  new_end_ = point;
  state_ = STARTED;
  return true;
}
 
//________________________________________________________________________
QRect BoxSelection::update( QPoint point )
{ 
  Debug::Throw( "BoxSelection::update.\n" );
  if( state_ != STARTED ) return QRect();
  
  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );
  
  end_ = new_end_; 
  new_end_ = point;  
  _updateRect();
  
  return rect_;
}

//________________________________________________________________________
QRect BoxSelection::finish( QPoint point )
{ 
  Debug::Throw( "BoxSelection::finish.\n" );
  if( state_ != STARTED ) return QRect();
  
  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );
  
  end_ = new_end_; 
  new_end_ = point;
  _updateRect();

  state_ = FINISHED;
  return rect_;
}

//________________________________________________________________________
bool BoxSelection::clear( void )
{ 
  Debug::Throw( "BoxSelection::clear.\n" );
  if( state_ != FINISHED ) return false;
  state_ = EMPTY;
  return true;
}

//________________________________________________________________________
void BoxSelection::_updateRect( void )
{
  
  // this can probably be made smarter when considering update
  // from previous call
  int x_min( min( begin_.x(), min( end_.x(), new_end_.x() ) ) );
  int x_max( max( begin_.x(), max( end_.x(), new_end_.x() ) ) );
  
  int y_min( min( begin_.y(), min( end_.y(), new_end_.y() ) ) );
  int y_max( max( begin_.y(), max( end_.y(), new_end_.y() ) ) );
  
  rect_ = QRect( QPoint( x_min, y_min ), QPoint( x_max, y_max ) );
  return;
  
}
