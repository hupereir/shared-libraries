#ifndef LineNumberWidget_h
#define LineNumberWidget_h

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
  \file LineNumberWidget.h
  \brief display line number of a text editor
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QTextEdit>
#include <QWidget>

#include "Counter.h"

//___________________________________________________________
class LineNumberWidget: public QWidget, public Counter
{

  Q_OBJECT  

  public:
    
  //! constructor
  LineNumberWidget(QTextEdit*, QWidget* parent);

  //! destructor
  virtual ~LineNumberWidget();
  
  protected:
  
  //! paint
  virtual void paintEvent( QPaintEvent* );
  
  private slots:
  
  void _updateConfiguration( void );
  
  private:
  
  //! editor
  QTextEdit& _editor( void ) const
  { return *editor_; }
  
  //! associated editor
  QTextEdit* editor_;
  
  //! current block highlight color
  QColor highlight_color_;

};

#endif
