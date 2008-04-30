#ifndef BlockHighlight_h
#define BlockHighlight_h

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
  \file BlockHighlight.h
  \brief handles current block highlighting
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QBasicTimer>
#include <QTimerEvent>

#include "Counter.h"
#include "Debug.h"

class CustomTextEdit;

//! handles current block highlighting
class BlockHighlight: public QObject, public Counter
{
    
  Q_OBJECT
  
  public:
  
  //! constructor
  BlockHighlight( CustomTextEdit* parent );
  
  //! destructor
  virtual ~BlockHighlight( void )
  { Debug::Throw( "BlockHighlight::~BlockHighlight.\n" ); }
  
  //! enable/disable
  void setEnabled( const bool& value )
  { enabled_ = value; }
    
  //! enable/disable
  const bool& isEnabled( void ) const
  { return enabled_; }
  
  //! clear highlighted block
  void clear( void );
  
  signals:
  
  //! emmited when block highlight was changed
  void highlightChanged( void );
  
  public slots:
  
  //! highlight
  void highlight( void );
  
  protected:
  
  //! delayed highlighting
  virtual void timerEvent( QTimerEvent* event );
  
  private:
  
  //! parent editor
  CustomTextEdit* parent_;
  
  //! associated timer
  QBasicTimer timer_;
  
  //! true if enabled
  bool enabled_;
  
  //! true when cleared
  bool cleared_;
  
};

#endif
