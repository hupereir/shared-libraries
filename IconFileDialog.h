// $Id$
#ifndef _IconFileDialog_h_
#define _IconFileDialog_h_
 
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
  \file IconFileDialog.h
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <string>
#include <QCheckBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QLabel>

#include "CustomFileDialog.h"
#include "File.h"  

/*!
  \class IconFileDialog
  \brief customized file selection dialog, to store last selected directory
*/
class IconFileDialog: public CustomFileDialog
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! creator
  IconFileDialog( QWidget* parent );  

  //! select file
  void selectFile( const QString& filename )
  {
    emit _currentChanged( filename );
    CustomFileDialog::selectFile( filename );
  }

  protected:

  //! drag enter event
  virtual void dragEnterEvent( QDragEnterEvent* );

  //! drop event
  virtual void dropEvent( QDropEvent* );
  
  private slots:
  
  //! update current
  void _currentChanged( const QString& path );

  //! display current image
  void _preview( void );

  protected:
  
  virtual void showEvent( QShowEvent* e )
  {
    CustomFileDialog::showEvent( e );
    _preview(); 
  }
  
  private:
  
  //! automatic preview checkbox
  QCheckBox* automatic_preview_;
  
  //! current file
  QString current_path_;
  
  //! preview label
  QLabel* preview_;
  
};

#endif
