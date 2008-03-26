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
  \file BrowsedLineEdit.cpp
  \brief combined QLineEdit + associated browse button
  line edit is also customized to have faster popup menu
  result of the file dialog associated to the button is stored into the 
  CustomLineEdit object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QHBoxLayout>
#include <QPushButton>

#include "BrowsedLineEdit.h"
#include "CustomLineEdit.h"
#include "File.h"
#include "QtUtil.h"
#include "Util.h"
#include "Debug.h"

using namespace std;

//____________________________________________________________
BrowsedLineEdit::BrowsedLineEdit( QWidget *parent ):
  QWidget( parent ),
  Counter( "BrowsedLineEdit" ),
  work_directory_( Util::workingDirectory() ),
  file_mode_( QFileDialog::AnyFile ),
  view_mode_( QFileDialog::List )
{
  Debug::Throw( "BrowsedLineEdit::BrowsedLineEdit.\n" );
  
  // insert horizontal layout
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(5);
  setLayout( layout );
  
  // create line editor
  line_edit_ = new CustomLineEdit( this );
  layout->addWidget( line_edit_ );
  
  // create push_button
  QPushButton *button = new QPushButton( "...", this );
  QtUtil::fixSize( button );
  layout->addWidget( button );
  
  // connect push_button
  connect( button, SIGNAL( clicked() ), SLOT( _browse() ) );
}

//____________________________________________________________
void BrowsedLineEdit::setWorkDirectory( const string& directory )
{
  line_edit_->setText( directory.c_str() );
  work_directory_ = directory;
}

//____________________________________________________________
void BrowsedLineEdit::setFile( const string& file )
{ line_edit_->setText( file.c_str() ); } 

//____________________________________________________________
void BrowsedLineEdit::_browse( void )
{ 
  Debug::Throw( "BrowsedLineEdit::_browse.\n" );
    
  // create file dialog
  CustomFileDialog dialog( this );
  dialog.setFileMode( file_mode_ );
  dialog.setViewMode( view_mode_ );
  
  // retrieve text, check if path is valid, assign to FileDialog
  File current_directory( qPrintable( line_edit_->text() ) );
  if( 
    current_directory.isDirectory() ||
    ( current_directory = current_directory.path() ).isDirectory() 
    ) 
  dialog.setDirectory( current_directory.c_str() );
  else dialog.setDirectory( work_directory_.c_str() );
 
  if( dialog.exec() != QDialog::Accepted ) return;

  // retrieve selected files
  QStringList files( dialog.selectedFiles() );
    
  // check file size
  if( files.size() > 1 ) 
  {
    QtUtil::infoDialog( this, "Too many files selected." );
    return;
  }
  
  if( files.size() < 1 )
  {
    QtUtil::infoDialog( this, "No file selected." );
    return;
  }
    
  setFile( qPrintable( files.front() ) );
  
  return; 
}
