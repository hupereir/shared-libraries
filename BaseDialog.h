#ifndef BaseDialog_h
#define BaseDialog_h

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
  \file BaseDialog.h
  \brief customized QDialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QDialog>
#include <QResizeEvent>

#include "WindowSizeWatcher.h"  

/*!
  \class BaseDialog
  \brief customized QDialog
*/
class BaseDialog: public QDialog
{

  public:
  
  //! constructor
  BaseDialog( QWidget *parent = 0, Qt::WFlags flags = 0);
  
  //! restore window size
  virtual QSize minimumSizeHint( void ) const;

  //! restore window size
  virtual QSize sizeHint( void ) const;

  //! center widget on argument widget
  void centerOnDesktop( void );
  
  //! center on parent widget
  void centerOnParent( void )
  { centerOnWidget( parentWidget() ); }
  
  //! center widget on argument widget
  void centerOnWidget( QWidget* );
 
  protected:
    
  //! resize event
  void resizeEvent( QResizeEvent* );
  
  //! size option name
  virtual void _setSizeOptionName( const std::string& name )
  { size_watcher_.setSizeOptionName( name ); }
    
  private:
  
  //! size watch
  WindowSizeWatcher size_watcher_;
      
};

#endif 
    
