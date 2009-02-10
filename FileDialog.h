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
    
  //! file mode
  FileDialog& setAcceptMode( const QFileDialog::AcceptMode mode )
  { 
    accept_mode_ = mode; 
    return *this;
  }
    
  //! file mode
  FileDialog& setFileMode( const QFileDialog::FileMode mode )
  { 
    file_mode_ = mode; 
    return *this;
  }
  
  //! caption
  FileDialog& setCaption( const QString& caption )
  { 
    caption_ = caption; 
    return *this;
  }
  
  //! working directory
  FileDialog& setWorkingDirectory( const QString& directory )
  { 
    working_directory_ = directory;
    return *this; 
  }
  
  //! filter
  FileDialog& setFilter( const QString& filter )
  { 
    filter_ = filter;
    return *this;
  }
  
  //! options
  FileDialog& setOptions( const QFileDialog::Options& options )
  { 
    options_ = options;
    return *this;
  }
  
  //! get file
  QString getFile( void );
  
  //! get list of files
  QStringList getFiles( void );
  
  private:
  
  //! reference to statically scoped working directory
  static QString& _workingDirectory();
  
  //! accept mode
  QFileDialog::AcceptMode accept_mode_;
  
  //! file mode
  QFileDialog::FileMode file_mode_;

  //! caption
  QString caption_;
  
  //! working directory
  QString working_directory_;
  
  //! filter
  QString filter_;
  
  //! options
  QFileDialog::Options options_;
  
};

#endif
