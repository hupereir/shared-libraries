#ifndef BoxSelection_h
#define BoxSelection_h

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
  \file BoxSelection.h
  \brief Box selection
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPoint>
#include <QRect>
#include "Counter.h"

class CustomTextEdit;

//! handles box selection
class BoxSelection: public Counter
{
 
  public:
  
  //! state 
  enum State
  {
    EMPTY,
    STARTED,
    FINISHED
  };
  
  //! constructor
  BoxSelection( CustomTextEdit* parent );
 
  //!@ accessors
  //@{
  
  //! start point
  const QPoint &begin() const
  { return begin_; }
  
  //! end point
  const QPoint &end() const
  { return end_; }

  //! end point
  const QPoint &newEnd() const
  { return new_end_; }
  
  //@}
  
  //!@name modifiers
  //@{
  
  //! start
  bool start( QPoint point );
  
  //! draw
  QRect update( QPoint point );
  
  //! finish
  QRect finish( QPoint point );
  
  //! clear
  bool clear( void );
  
  //! state
  const State& state( void ) const
  { return state_; }
  
  //@}
  
  private:
  
  //! update repaiint rect
  void _updateRect( void );
  
  //! parent editor
  CustomTextEdit* parent_;
  
  //! state
  State state_;
  
  //! start point
  QPoint begin_;
  
  //! end point
  QPoint end_;
  
  //! new end point (while updating)
  QPoint new_end_;
  
  //! max rectangle
  QRect rect_;
  
};

#endif
