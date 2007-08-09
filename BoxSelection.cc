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
#include "XmlOptions.h"

#include <QPainter>
#include <QScrollBar>

using namespace std;

//________________________________________________________________________
BoxSelection::BoxSelection( CustomTextEdit* parent ):
  Counter( "BoxSelection" ),
  parent_( parent ),
  color_( parent->palette().color( QPalette::Highlight ) ),
  font_width_( 0 ),
  font_height_( 0 ),
  left_margin_( 0 ),
  top_margin_( 0 ),
  state_( EMPTY )
{ Debug::Throw( "BoxSelection::BoxSelection.\n" ); }  
  
//________________________________________________________________________
void BoxSelection::updateConfiguration( void )
{
  
  Debug::Throw( "BoxSelection::updateConfiguration.\n" );
  color_ = parent_->palette().color( QPalette::Highlight );
  color_.setAlpha( 50 );
  
  // read font from option
  QFont font;
  font.fromString( XmlOptions::get().raw( "FIXED_FONT_NAME" ).c_str() );
  font_width_ = QFontMetrics( font ).width( " " );
  font_height_ = QFontMetrics( font ).lineSpacing();
  
  int right, bottom;
  parent_->getContentsMargins ( &left_margin_, &top_margin_, &right, &bottom );
  
}

//________________________________________________________________________
bool BoxSelection::start( QPoint point )
{
  Debug::Throw( "BoxSelection::start.\n" );
  if( state_ != EMPTY ) return false;
  
  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );
  
  begin_ = point;
  end_ = point;
  end_ = point;
  state_ = STARTED;
  parent_->setTextCursor( parent_->cursorForPosition( end_ ) );
  return true;
}
 
//________________________________________________________________________
bool BoxSelection::update( QPoint point )
{ 
  Debug::Throw( "BoxSelection::update.\n" );
  if( state_ != STARTED ) return false;
  
  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );
  
  end_ = point;
  
  QRect old( rect() );
  _updateRect();
  parent_->viewport()->repaint( old.unite( rect() ) );
  parent_->setTextCursor( parent_->cursorForPosition( end_ ) );
 
  return true;
}

//________________________________________________________________________
bool BoxSelection::finish( QPoint point )
{ 
  Debug::Throw( "BoxSelection::finish.\n" );
  if( state_ != STARTED ) return false;
  
  point.setX( point.x() + parent_->horizontalScrollBar()->value() );
  point.setY( point.y() + parent_->verticalScrollBar()->value() );
  
  end_ = point;
  
  QRect old( rect() );
  _updateRect();
  parent_->viewport()->repaint( old.unite( rect() ) );

  state_ = FINISHED;
  
  _store();
  
  return true;
}

//________________________________________________________________________
bool BoxSelection::clear( void )
{ 
  Debug::Throw( "BoxSelection::clear.\n" );
  if( state_ != FINISHED ) return false;
  state_ = EMPTY;
  parent_->viewport()->repaint();
  return true;
}

//________________________________________________________________________
void BoxSelection::_updateRect( void )
{
  
  // this can probably be made smarter when considering update
  // from previous call
  int x_min( min( begin_.x(), end_.x() ) );
  int x_max( max( begin_.x(), end_.x() ) );
  
  int y_min( min( begin_.y(), end_.y() ) );
  int y_max( max( begin_.y(), end_.y() ) );
  
  QPoint translation( parent_->horizontalScrollBar()->value(), parent_->verticalScrollBar()->value() );
  
  QPoint begin( x_min, y_min );
  QPoint end( x_max, y_max );
  
  // map begin and end point to font width
  QRect cursor_rect = parent_->cursorRect( parent_->cursorForPosition( begin - translation ) );
  int x_offset = (x_min - cursor_rect.topLeft().x())%font_width_;
  int y_offset = (y_min - cursor_rect.topLeft().y())%font_height_;
  begin -= QPoint( x_offset+left_margin_, y_offset+top_margin_ );
  
  cursor_rect = parent_->cursorRect( parent_->cursorForPosition( end - translation ) );
  x_offset = (x_max - cursor_rect.topLeft().x())%font_width_;
  y_offset = (y_max - cursor_rect.topLeft().y())%font_height_;
  end += QPoint( font_width_ - x_offset - left_margin_, font_height_ - y_offset - top_margin_ );
  
  // propagate again to begin and end points
  begin_.setX( begin_.x() < end_.x() ? begin.x():end.x() );
  begin_.setY( begin_.y() < end_.y() ? begin.y():end.y() );

  end_.setX( begin_.x() < end_.x() ? end.x():begin.x());  
  end_.setY( begin_.y() < end_.y() ? end.y():begin.y());  
  
  // compute rect
  rect_ = QRect( begin, end );
  
  return;
  
}

//________________________________________________________________________
void BoxSelection::_store( void )
{
  Debug::Throw( "BoxSelection::_store.\n" );


}
  
