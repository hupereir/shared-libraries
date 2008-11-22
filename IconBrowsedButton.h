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
  \file IconBrowsedButton.h
  \brief icon browser. The icon is displayed as the button label
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#ifndef IconBrowsedButton_h
#define IconBrowsedButton_h

#include <QIconSet>

#include "CustomToolButton.h"
#include "Counter.h"  
#include "Debug.h"  
#include "File.h"  
#include "Util.h"  

/*!
  \class IconBrowsedButton
  \brief customized tool button to display tooltip in a dedicated label
*/
class IconBrowsedButton: public CustomToolButton
{

  //! Qt meta object macros
  Q_OBJECT

  public:
  
  //! no icon filename
  static const std::string NO_ICON;
      
  //! creator
  IconBrowsedButton( 
    QWidget* parent,   
    const File& file = NO_ICON
  );
  
  //! destructor
  ~IconBrowsedButton( void ) 
  {}
  
  //! iconFile
  virtual const File& file( void ) const
  { return file_; }
  
  //! iconFile
  virtual bool setFile( const File& file, const bool& check );

  protected:
  
  //! drag enter event
  virtual void dragEnterEvent( QDragEnterEvent* );
  
  //! drop event
  virtual void dropEvent( QDropEvent* );

  protected slots:
      
  //! opens file dialog to browse icon file
  virtual void _browse( void );
  
  private:
      
  //! empty pixmap
  QPixmap no_icon_pixmap_;
    
  //! icon file
  File file_;
  
};

#endif
