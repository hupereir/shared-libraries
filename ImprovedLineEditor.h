#ifndef _ImprovedLineEditor_h_
#define _ImprovedLineEditor_h_

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
  \file ImprovedLineEditor.h
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>
#include <QString>
#include <QMouseEvent>
#include <QPaintEvent>

#include "Debug.h"
#include "Counter.h"
#include "LineEditor.h"

/*!
   \class ImprovedLineEditor
   \brief customized line edit to have faster popup menu
*/
class ImprovedLineEditor: public LineEditor
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor;
  ImprovedLineEditor( QWidget* parent );

  protected:
  
  //! paint
  void paintEvent( QPaintEvent* );

  //! mouse move
  void mouseMoveEvent( QMouseEvent* );
  
  //! mouse move
  void mousePressEvent( QMouseEvent* );

  //! mouse move
  void mouseReleaseEvent( QMouseEvent* );
  
  private:
  
  //! clear pixmap
  QIcon clear_icon_;
  
};

#endif
