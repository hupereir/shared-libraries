#ifndef AnimatedLabel_h
#define AnimatedLabel_h

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
* software; if not, write to the Free Software , Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
  \file AnimatedLabel.h
  \brief animated QLabel object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLabel>

#include "Counter.h"

class TransitionWidget;

class AnimatedLabel: public QLabel, public Counter
{

  Q_OBJECT

  public:

  //! constructor
  AnimatedLabel( QWidget* parent = 0 );

  //! destrutor
  virtual ~AnimatedLabel( void );

  //! transition widget
  TransitionWidget& transitionWidget( void ) const
  { return *transition_widget_; }

  public slots:

  //! set text
  virtual void setText( const QString & );

  //! clear
  virtual void clear( void );

  private:

  //! transition widget
  TransitionWidget* transition_widget_;


};

#endif
