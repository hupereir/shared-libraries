
#ifndef _CustomLineEdit_h_
#define _CustomLineEdit_h_

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
  \file CustomLineEdit.h
  \brief customized QLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QShortcut> 

#include <string>
#include <vector>

#include "Counter.h"

/*!
   \class CustomLineEdit
   \brief customized line edit to have faster popup menu
*/
class CustomLineEdit: public QLineEdit, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor;
  CustomLineEdit( QWidget* parent );
      
  //! enable accelerator
  void enableShortCuts( const bool& value )
  { 
    for( std::vector<QShortcut*>::iterator iter = shortcuts_.begin(); iter != shortcuts_.end(); iter++ )
    { (*iter)->setEnabled( value ); }
  }
  
  signals:
  
  //! signal emmited when the cursor changes position
  void cursorPositionChanged( int position );  
  
  public slots:
  
  //! set text
  void setTextString( const std::string& value )
  { setText( QString( value.c_str() ) ); }
    
  //! changes selection to uppercase
  void lowerCase( void );
    
  //! changes selection to uppercase
  void upperCase( void );
  
  protected:
  
  //! context menu (overloaded)
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  //! overloaded key press event
  virtual void keyPressEvent( QKeyEvent* );
    
  //! overloaded mouse event handler
  virtual void mouseReleaseEvent( QMouseEvent* );
  
  private:
  
  //! retrieve searched string
  std::string _getSelection( void );

  //! accelerator
  std::vector<QShortcut*> shortcuts_;
    
};

#endif
