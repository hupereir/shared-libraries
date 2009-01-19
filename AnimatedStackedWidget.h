
#ifndef _AnimatedStackedWidget_h_
#define _AnimatedStackedWidget_h_

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
  \file AnimatedStackedWidget.h
  \brief animated stacked widget
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QPaintEvent>
#include <QTimeLine>
#include <QTimerEvent>
#include <QStackedWidget>

#include "Counter.h"

class Fader: public QWidget, public Counter
{

  Q_OBJECT
  
  public:
  
  //! constructor
  Fader( QWidget* parent );

  //! start
  void start( QSize, QWidget*, QWidget* );
  
  //! timeline
  QTimeLine& timeLine( void )
  { return time_line_; }
  
  signals:
  
  void finished( void );
  
  protected:
  
  //! paint event
  virtual void paintEvent( QPaintEvent* );
  
  private:
  
  //! timeline
  QTimeLine time_line_;
    
  //! current widget pixmap
  QPixmap first_pixmap_;
  
  //! second pixmap
  QPixmap second_pixmap_;
  
};
  

//! tabbed dialog
/*! a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class AnimatedStackedWidget: public QStackedWidget, public Counter
{

  Q_OBJECT

  public:

  //! creator
  AnimatedStackedWidget( QWidget *parent );

  //! destructor
  virtual ~AnimatedStackedWidget();

  public slots:
  
  //! current index
  void setCurrentIndex( int );
    
  //! current widget
  void setCurrentWidget( QWidget* );
  
  protected slots:
  
  //! animation finished
  void _animationFinished( void );
  
  private:
  
  Fader& _fader( void ) const
  { return *fader_; }
  
  //! current widget
  QWidget* widget_; 
  
  //! fader
  Fader* fader_;
  
};


#endif
