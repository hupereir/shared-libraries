#ifndef AnimatedLineEditor_h
#define AnimatedLineEditor_h

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
  \file AnimatedLineEditor.h
  \brief Animated text editor
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "LineEditor.h"

class TransitionWidget;

//! Animated text editor
/*! uses fading when calling setPlainText, setHTML or clear */
class AnimatedLineEditor: public LineEditor
{
    
  Q_OBJECT
  
  public:
  
  //! constructor
  AnimatedLineEditor( QWidget* parent = 0 );
  
  //! destrutor
  virtual ~AnimatedLineEditor( void );

  public slots:
  
  //! clear
  virtual void clear( void );
  
  //! set text
  virtual void setText( const QString& );

  protected:
  
  //! transition widget
  TransitionWidget& _transitionWidget( void ) const
  { return *transition_widget_; }

  private:
  
  //! transition widget
  TransitionWidget* transition_widget_;
  
  
};

#endif
