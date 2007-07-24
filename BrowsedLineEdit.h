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
  \file BrowsedLineEdit.h
  \brief combined QLineEdit + associated browse button
  line edit is also customized to have faster popup menu
  result of the file dialog associated to the button is stored into the 
  CustomLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#ifndef _BrowsedLineEdit_h_
#define _BrowsedLineEdit_h_

/*! 
  \class BrowsedLineEdit
  \brief combined QLineEdit + associated browse button
  line edit is also customized to have faster popup menu
  result of the file dialog associated to the button is stored into the 
  CustomLineEdit object
*/

#include <QWidget>
#include <string>

#include "Counter.h"
#include "Exception.h"
#include "CustomFileDialog.h"

class CustomLineEdit;

class BrowsedLineEdit: public QWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor;
  BrowsedLineEdit( QWidget* parent );
    
  //! retrieve line editor
  CustomLineEdit& editor( void ) const
  { 
    Exception::checkPointer( line_edit_, DESCRIPTION( "line_edit_ not initialized" ) );
    return *line_edit_; 
  }
    
  //! set view mode
  void setMode( const QFileDialog::FileMode& mode ) 
  { mode_ = mode; }
  
  //! retrieve view mode
  QFileDialog::FileMode mode( void ) const
  { return mode_; }
  
  //! retrieve working directory
  std::string workDirectory( void ) const
  { return work_directory_; }
  
  public slots:
  
  //! set working directory
  void setWorkDirectory( const std::string& directory );

  //! set line_edit_file
  void setFile( const std::string& file );
  
  private slots:

  //! opens a file dialog, stores result in line edit 
  void _browse( void );
  
  private:
  
  //! line editor widget
  CustomLineEdit *line_edit_;
  
  //! default working directory
  std::string work_directory_;
  
  //! File dialog mode
  QFileDialog::FileMode mode_;
  
};

#endif
