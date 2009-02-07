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
  \file BrowsedLineEditor.cpp
  \brief combined QLineEdit + associated browse button
  line edit is also customized to have faster popup menu
  result of the file dialog associated to the button is stored into the 
  LineEditor object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QHBoxLayout>
#include <QToolButton>

#include "BaseIcons.h"
#include "BrowsedLineEditor.h"
#include "LineEditor.h"
#include "File.h"
#include "IconEngine.h"
#include "InformationDialog.h"
#include "Util.h"
#include "Debug.h"

using namespace std;

//____________________________________________________________
BrowsedLineEditor::BrowsedLineEditor( QWidget *parent ):
  QWidget( parent ),
  Counter( "BrowsedLineEditor" ),
  work_directory_( Util::workingDirectory() ),
  file_dialog_( 0 ),
  file_mode_( QFileDialog::ExistingFile ),
  view_mode_( QFileDialog::List )
{
  Debug::Throw( "BrowsedLineEditor::BrowsedLineEditor.\n" );
  
  // insert horizontal layout
  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0);
  layout->setSpacing(2);
  setLayout( layout );
  
  // create line editor
  line_edit_ = new Editor( this );
  //line_edit_->setHasClearButton( false );
  layout->addWidget( line_edit_, 1 );
  
  // create push_button
  QToolButton *button = new QToolButton( this );
  button->setIcon( IconEngine::get( ICONS::OPEN ) );
  button->setToolTip( "Browse file system" );
  layout->addWidget( button, 0 );
  
  // connect push_button
  connect( button, SIGNAL( clicked() ), SLOT( _browse() ) );
}

//____________________________________________________________
void BrowsedLineEditor::setWorkDirectory( const QString& directory )
{
  line_edit_->setText( directory );
  work_directory_ = directory;
}

//____________________________________________________________
void BrowsedLineEditor::setFile( const QString& file )
{ line_edit_->setText( file ); } 

//____________________________________________________________
void BrowsedLineEditor::_browse( void )
{ 
  Debug::Throw( "BrowsedLineEditor::_browse.\n" );
    
  // create file dialog
  if( !file_dialog_ ) file_dialog_ = new CustomFileDialog( this );
  file_dialog_->setFileMode( file_mode_ );
  file_dialog_->setViewMode( view_mode_ );
  file_dialog_->setAcceptMode( QFileDialog::AcceptOpen );

  // retrieve text, check if path is valid, assign to FileDialog
  File current_directory( line_edit_->text() );
  if( current_directory.isDirectory() || ( current_directory = current_directory.path() ).isDirectory() ) 
  file_dialog_->setDirectory( current_directory );
  else file_dialog_->setDirectory( work_directory_ );
 
  if( file_dialog_->exec() != QDialog::Accepted ) return;

  // retrieve selected files
  QStringList files( file_dialog_->selectedFiles() );
    
  // check file size
  if( files.size() > 1 ) 
  {
    InformationDialog( this, "Too many files selected." ).exec();
    return;
  }
  
  if( files.size() < 1 )
  {
    InformationDialog( this, "No file selected." ).exec();
    return;
  }
    
  setFile( files.front() );
  
  return; 
}
