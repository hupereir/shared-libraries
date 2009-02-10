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
  \file FileDialog.cpp
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <assert.h>

#include "Debug.h"
#include "FileDialog.h"
#include "Util.h"

using namespace std;

//_______________________________________________________
QString& FileDialog::_workingDirectory( void )
{
  static QString working_directory = Util::workingDirectory();
  return working_directory;
}

//_______________________________________________________
FileDialog::FileDialog( QWidget* parent ):
  QObject( parent ),
  Counter( "FileDialog" ),
  accept_mode_( QFileDialog::AcceptOpen ),
  file_mode_( QFileDialog::ExistingFile ),
  working_directory_( _workingDirectory() ),
  options_(0)
{ Debug::Throw( "FileDialog::FileDialog.\n" ); }

//_______________________________________________________
QString FileDialog::getFile( void )
{
  Debug::Throw( "FileDialog::getFile.\n" );
  QString out;
  if( accept_mode_ == QFileDialog::AcceptOpen )
  {
    if( file_mode_ == QFileDialog::DirectoryOnly || file_mode_ == QFileDialog::Directory )
    {
      
      options_ |= QFileDialog::ShowDirsOnly;
      out = QFileDialog::getExistingDirectory( static_cast<QWidget*>( parent() ), caption_, working_directory_, options_ );
      
    } else {
      
      out = QFileDialog::getOpenFileName( static_cast<QWidget*>( parent() ), caption_, working_directory_, filter_, 0, options_ );
      
    }
    
  } else {
    
    out = QFileDialog::getSaveFileName( static_cast<QWidget*>( parent() ), caption_, working_directory_, filter_, 0, options_ );
    
  }
        
  // store working directory 
  if( !out.isNull() ) _workingDirectory() = QFileInfo( out ).path();
  return out;
  
}

//_______________________________________________________
QStringList FileDialog::getFiles( void )
{

  Debug::Throw( "FileDialog::getFiles.\n" );
  assert( accept_mode_ == QFileDialog::AcceptOpen );
  assert( file_mode_ == QFileDialog::ExistingFiles );
  QStringList out( QFileDialog::getOpenFileNames( static_cast<QWidget*>( parent() ), caption_, working_directory_, filter_, 0, options_ ) );
  if( !(out.empty() || out.front().isNull() ) ) _workingDirectory() = QFileInfo( out.front() ).path(); 
  return out;
}
