#ifndef TransitionWidget_h
#define TransitionWidget_h

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
  \file TransitionWidget.cpp
  \brief widget used for smooth transition between two widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPaintEvent>
#include <QPixmap>
#include <QTimeLine>
#include <QWidget>

#include "Counter.h"

class TransitionWidget: public QWidget, public Counter
{

  Q_OBJECT
  
  public:
  
  //! constructor
  TransitionWidget( QWidget* parent = 0 );

  //! resize
  virtual void resize( const QSize& );

  //! set starting widget
  void setStartWidget( QWidget* );

  //! set starting widget
  void setEndWidget( QWidget* );

  //! start
  void start( QSize, QWidget*, QWidget* );
 
  //! start
  void start( void );
 
  //! timeline
  QTimeLine& timeLine( void )
  { return time_line_; }
  
  //! modes
  enum Mode
  {
    NONE = 0,
    FADE_FIRST = 1<<0,
    FADE_SECOND = 1<<1,
    FADE_BOTH = FADE_FIRST | FADE_SECOND
  };
  
  //! mode
  void setMode( const unsigned int mode )
  { mode_ = mode; }
  
  protected:
  
  //! paint event
  virtual void paintEvent( QPaintEvent* );
  
  private slots:
  
  //! configuration
  void _updateConfiguration( void );
  
  private:
  
  //! used to copy pixmap from widget
  class Pixmap: public QPixmap, public Counter
  {
    public:
    
    //! constructor
    Pixmap( void );
    
    //! constructor
    Pixmap( QSize size );
    
    //! copy pixmap
    void fromWidget( QWidget* );
    
  };
  
  //! mode
  unsigned int mode_; 
  
  //! timeline
  QTimeLine time_line_;
    
  //! current widget pixmap
  Pixmap first_;
  
  //! second pixmap
  Pixmap second_;
  
};
  
#endif
