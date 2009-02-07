#ifndef _BrowsedLineEditor_h_
#define _BrowsedLineEditor_h_

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
  \file BrowsedLineEditor.h
  \brief combined QLineEdit + associated browse button
  line edit is also customized to have faster popup menu
  result of the file dialog associated to the button is stored into the 
  LineEditor object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/


#include <assert.h>
#include <QWidget>
#include <string>

#include "Counter.h"
#include "CustomFileDialog.h"

class LineEditor;

/*! 
  \class BrowsedLineEditor
  \brief combined QLineEdit + associated browse button
  line edit is also customized to have faster popup menu
  result of the file dialog associated to the button is stored into the 
  LineEditor object
*/
class BrowsedLineEditor: public QWidget, public Counter
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  typedef LineEditor Editor;
    
  //! constructor;
  BrowsedLineEditor( QWidget* parent );
    
  //! retrieve line editor
  Editor& editor( void ) const
  { 
    assert( line_edit_ );
    return *line_edit_; 
  }
    
  //! file mode
  void setFileMode( const QFileDialog::FileMode& mode ) 
  { file_mode_ = mode; }
  
  //! file mode
  QFileDialog::FileMode fileMode( void ) const
  { return file_mode_; }

  //! view mode
  void setViewMode( const QFileDialog::ViewMode& mode ) 
  { view_mode_ = mode; }
  
  //! view mode
  QFileDialog::ViewMode viewMode( void ) const
  { return view_mode_; }
  
  //! set file dialog
  void setFileDialog( CustomFileDialog* dialog )
  { file_dialog_ = dialog; }
  
  //! retrieve working directory
  const QString& workDirectory( void ) const
  { return work_directory_; }
  
  public slots:
  
  //! set working directory
  void setWorkDirectory( const QString& directory );

  //! set line_edit_file
  void setFile( const QString& file );
  
  private slots:

  //! opens a file dialog, stores result in line edit 
  void _browse( void );
  
  private:
  
  //! line editor widget
  Editor *line_edit_;
  
  //! default working directory
  QString work_directory_;

  //! pointer to file dialog
  CustomFileDialog* file_dialog_;
  
  //! File dialog mode
  QFileDialog::FileMode file_mode_;
  
  //! File dialog view mode
  QFileDialog::ViewMode view_mode_;
  
};

#endif
