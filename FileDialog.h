// $Id$
#ifndef _FileDialog_h_
#define _FileDialog_h_
 
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
  \file FileDialog.h
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QFileDialog>
#include <QObject>
#include <QString>
#include <QStringList>

#include "Counter.h"  

/*!
  \class FileDialog
  \brief customized file selection dialog, to store last selected directory
*/
class FileDialog: public QObject, public Counter
{

  public:
  
  //! creator
  FileDialog( QWidget* parent );  
   
  //! destructor
  virtual ~FileDialog( void )
  {}
  
  //! open mode
  virtual FileDialog& setAcceptMode( const QFileDialog::AcceptMode mode )
  { 
    accept_mode_ = mode; 
    return *this;
  }
    
  //! file mode
  virtual FileDialog& setFileMode( const QFileDialog::FileMode mode )
  { 
    file_mode_ = mode; 
    return *this;
  }
  
  //! caption
  virtual FileDialog& setCaption( const QString& caption )
  { 
    caption_ = caption; 
    return *this;
  }
  
  //! default file/directory
  /*! it is used as a starting selection when file dialog is opened */
  virtual FileDialog& selectFile( const QString& file );
  
  //! filter
  virtual FileDialog& setFilter( const QString& filter )
  { 
    filter_ = filter;
    return *this;
  }
  
  //! options
  virtual FileDialog& setOptions( const QFileDialog::Options& options )
  { 
    options_ = options;
    return *this;
  }
  
  //! get file
  virtual QString getFile( void );
  
  //! get list of files
  virtual QStringList getFiles( void );
  
  //! reference to statically scoped working directory
  virtual const QString& workingDirectory() const
  { return _workingDirectory(); }
  
  protected:
  
  //! reference to statically scoped working directory
  static QString& _workingDirectory();
  
  //! accept mode
  virtual const QFileDialog::AcceptMode& _acceptMode( void ) const
  { return accept_mode_; }
  
  //! file mode 
  virtual const QFileDialog::FileMode& _fileMode( void ) const
  { return file_mode_; }
  
  //! caption
  virtual const QString& _caption( void ) const
  { return caption_; }
  
  //! selected file
  /*! it is used as a starting selection when file dialog is opened */
  virtual const QString& _selectedFile( void ) const
  { return selected_file_; }
  
  //! filter
  virtual const QString& _filter( void ) const
  { return filter_; }
  
  //! options
  virtual const QFileDialog::Options& _options( void ) const
  { return options_; }
  
  //! options
  virtual QFileDialog::Options& _options( void )
  { return options_; }
 
  private:
  
  //! accept mode
  QFileDialog::AcceptMode accept_mode_;
  
  //! file mode
  QFileDialog::FileMode file_mode_;

  //! caption
  QString caption_;
  
  //! default file
  QString selected_file_;
  
  //! filter
  QString filter_;
  
  //! options
  QFileDialog::Options options_;
  
  //! needed to allow ImageFileDialog to acces working directory
  friend class ImageFileDialog;
  
};

#endif
