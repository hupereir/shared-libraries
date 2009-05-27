// $Id$
#ifndef _ImageFileDialog_h_
#define _ImageFileDialog_h_
 
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
  \file ImageFileDialog.h
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <cassert>

#include <QCheckBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>

#include "File.h"  

/*!
  \class ImageFileDialog
  \brief customized file selection dialog, to store last selected directory
*/
class ImageFileDialog: public QFileDialog
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! creator
  ImageFileDialog( QWidget* parent );  

  //! select file
  void selectFile( const QString& filename )
  {
    emit _currentChanged( filename );
    QFileDialog::selectFile( filename );
  }

  private slots:
  
  //! update current
  void _currentChanged( const QString& path );

  //! display current image
  void _preview( void );

  protected:
  
  virtual void showEvent( QShowEvent* e )
  {
    QFileDialog::showEvent( e );
    _preview(); 
  }
  
  private slots:
  
  //! stores working directory when changed
  void _saveWorkingDirectory( const QString& directory );

  private:
  
  //! automatic preview checkbox
  QCheckBox& _automaticPreviewCheckbox( void ) const
  { 
    assert( automatic_preview_ );
    return *automatic_preview_;
  }
  
  //! automatic preview checkbox
  QCheckBox* automatic_preview_;
  
  //! current file
  QString current_path_;

  class Label: public QLabel, public Counter
  {
    public:
    
    //! constructor
    Label( QWidget* parent );
    
    protected:
    
    //! drag enter event
    virtual void dragEnterEvent( QDragEnterEvent* );

    //! drop event
    virtual void dropEvent( QDropEvent* );
        
  };
    
  //! preview label
  Label* preview_;
  
};

#endif
