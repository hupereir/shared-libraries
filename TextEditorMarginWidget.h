#ifndef TextEditorMarginWidget_h
#define TextEditorMarginWidget_h

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
  \file TextEditorMarginWidget.h
  \brief widget used to draw margins in TextEditors
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QEvent>
#include <QPaintEvent>
#include <QWidget>

#include "Counter.h"

class TextEditor;

// margin widget
class TextEditorMarginWidget: public QWidget, public Counter
{
  
  Q_OBJECT
  
  public:
  
  //! constructor
  TextEditorMarginWidget( TextEditor* );
  
  //! destructor
  virtual ~TextEditorMarginWidget( void )
  {}
  
  //! vertical line
  const bool& drawVerticalLine( void ) const
  { return draw_vertical_line_; }
  
  public slots:
  
  //! dirty
  void setDirty( void );
  
  private slots:
  
  //! configuration
  void _updateConfiguration( void );
  
  protected:
    
  //! paint event
  virtual void paintEvent( QPaintEvent* );
  
  //! parent editor
  TextEditor& _editor( void ) const
  { return *editor_; }

  private:

  //! parent editor
  TextEditor* editor_;
  
  //! dirty flag
  bool dirty_;

  //! draw vertical line
  bool draw_vertical_line_;
  
};

#endif
