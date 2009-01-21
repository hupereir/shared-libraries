#ifndef AnimatedTextEditor_h
#define AnimatedTextEditor_h

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
  \file AnimatedTextEditor.h
  \brief Customized QTextEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "TextEditor.h"

class TransitionWidget;

class AnimatedTextEditor: public TextEditor
{
    
  public:
  
  //! constructor
  AnimatedTextEditor( QWidget* parent = 0 );
  
  //! destrutor
  virtual ~AnimatedTextEditor( void );
      
  //! set text
  virtual void setPlainText( const QString& );
  
  //! set text
  virtual void setHtml( const QString& );

  private:
  
  //! transition widget
  TransitionWidget& _transitionWidget( void ) const
  { return *transition_widget_; }
  
  //! transition widget
  TransitionWidget* transition_widget_;
  
  
};

#endif
