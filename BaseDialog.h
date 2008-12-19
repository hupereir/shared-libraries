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

  Q_OBJECT
  
  public:
  
  //! dialog centering enumeration
  enum Centering {
    
    //! center widget on mouse pointer
    CENTER_ON_POINTER,
        
    //! center widget on parent widget
    CENTER_ON_PARENT,
        
    //! center widget on desktop
    CENTER_ON_DESKTOP
        
  };
  
  //! constructor
  BaseDialog( QWidget *parent = 0, Qt::WFlags flags = 0);
  
  //! restore window size
  virtual QSize minimumSizeHint( void ) const;

  //! restore window size
  virtual QSize sizeHint( void ) const;

  //! center widget on pointer
  BaseDialog& centerOnPointer( void );
  
  //! center widget on argument widget
  BaseDialog& centerOnDesktop( void );
  
  //! center on parent widget
  BaseDialog& centerOnParent( void )
  { return centerOnWidget( parentWidget() ); }
  
  //! center widget on argument widget
  BaseDialog& centerOnWidget( QWidget* );

  //! size option name
  virtual void setSizeOptionName( const std::string& name )
  { size_watcher_.setSizeOptionName( name ); }

  public slots:
  
  //! uniconify
  BaseDialog& uniconify( void );
 
  protected:

  //! generic event
  virtual bool event( QEvent* );
    
  //! resize event
  void resizeEvent( QResizeEvent* );
      
  //! maximize state prior to minimization
  bool _wasMaximized( void ) const
  { return was_maximized_; }
  
  //! maximize state prior to minimization
  void _setWasMaximized( bool value )
  { was_maximized_ = value; }

  private:
  
  //! size watch
  WindowSizeWatcher size_watcher_;
      
  //! window state prior to minimization
  bool was_maximized_;

};

#endif 
    
