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
#include <QFileDialog>
#include <QWidget>


#include "Counter.h"

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
    
  //! open mode
  void setAcceptMode( const QFileDialog::AcceptMode mode )
  { accept_mode_ = mode; }

  //! file mode
  void setFileMode( const QFileDialog::FileMode& mode ) 
  { file_mode_ = mode; }
  
  public slots:

  //! set line_edit_file
  void setFile( const QString& file );
  
  protected slots:

  //! opens a file dialog, stores result in line edit 
  virtual void _browse( void );
  
  private:
  
  //! line editor widget
  Editor *line_edit_;
  
  //! accept mode
  QFileDialog::AcceptMode accept_mode_;
  
  //! File dialog mode
  QFileDialog::FileMode file_mode_;
  
};

#endif
