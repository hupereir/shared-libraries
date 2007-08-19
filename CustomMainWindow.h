#ifndef CustomMainWindow_h
#define CustomMainWindow_h

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
  \file CustomMainWindow.h
  \brief customized QMainWindow
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QMainWindow>

#include "Debug.h"  
#include "Exception.h"  
#include "XmlOptions.h"  

/*!
  \class CustomMainWindow
  \brief customized QDialog
*/
class CustomMainWindow: public QMainWindow
{
  
  Q_OBJECT

  public:
  
  //! constructor
  CustomMainWindow( QWidget *parent, Qt::WFlags wflags = 0);
  
  //! destructor
  virtual ~CustomMainWindow( void )
  { Debug::Throw( "CustomMainWindow::~CustomMainWindow.\n" ); }
  
  private slots:
  
  //! update configuration
  void _updateConfiguration( void );
  
};

#endif 
    
