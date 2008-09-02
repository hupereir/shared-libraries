#ifndef SelectLineDialog_h
#define SelectLineDialog_h

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
  \file SelectLineDialog.h
  \brief goto_line_number_dialog dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <string>

#include "BaseDialog.h"
#include "Counter.h"
#include "LineEditor.h"
#include "Str.h"

//! find_text dialog for text editor widgets
class SelectLineDialog: public BaseDialog, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor
  SelectLineDialog( QWidget* parent = 0, Qt::WFlags wflags = 0 );
  
  //! retrieve editor
  LineEditor& editor( void ) const
  { return *editor_; }

  signals:
  
  //! emmited when pressing the Ok button
  void lineSelected( int line );
  
  private slots:
  
  //! retrieve line number and emit signal
  void _selectLine( void );
    
  private:
  
  //! line editor for text to find
  LineEditor* editor_;      
    
};
#endif
