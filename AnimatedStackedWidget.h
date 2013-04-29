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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

/*!
  \file AnimatedStackedWidget.h
  \brief animated stacked widget
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QStackedWidget>

#include "Counter.h"

class TransitionWidget;

//! animated stacked widget
class AnimatedStackedWidget: public QStackedWidget, public Counter
{

  Q_OBJECT

  public:

  //! creator
  AnimatedStackedWidget( QWidget *parent );

  //! destructor
  virtual ~AnimatedStackedWidget();

  //! transition widget
  TransitionWidget& transitionWidget( void ) const
  { return *transition_widget_; }

  public slots:

  //! current index
  void setCurrentIndex( int );

  //! current widget
  void setCurrentWidget( QWidget* );

  protected slots:

  //! animation finished
  void _animationFinished( void );

  private:

  //! transitionWidget
  TransitionWidget* transition_widget_;

};


#endif
