#ifndef AnimatedLabel_h
#define AnimatedLabel_h

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

#include "Counter.h"
#include <QLabel>

class TransitionWidget;

class AnimatedLabel: public QLabel, public Counter
{

  Q_OBJECT

  public:

  //* constructor
  AnimatedLabel( QWidget* = nullptr );

  //* destrutor
  virtual ~AnimatedLabel( void ) = default;

  //* transition widget
  TransitionWidget& transitionWidget( void ) const
  { return *transitionWidget_; }

  public Q_SLOTS:

  //* set text
  virtual void setText( const QString & );

  //* clear
  virtual void clear( void );

  private:

  //* transition widget
  TransitionWidget* transitionWidget_ = nullptr;


};

#endif
