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

#include <cassert>

#include "Debug.h"
#include "File.h"
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
  selected_file_( _workingDirectory() ),
  options_(0)
{ Debug::Throw( "FileDialog::FileDialog.\n" ); }

//_______________________________________________________
FileDialog& FileDialog::selectFile( const QString& file )
{
  selected_file_ = file;
  _workingDirectory() = QFileInfo( file ).isDir() ? file: File( file ).path();
  return *this;
}

//_______________________________________________________
QString FileDialog::getFile( void )
{
  Debug::Throw( "FileDialog::getFile.\n" );
  QString out;
  if( _acceptMode() == QFileDialog::AcceptOpen )
  {
    if( _fileMode() == QFileDialog::DirectoryOnly || _fileMode() == QFileDialog::Directory )
    {

      _options() |= QFileDialog::ShowDirsOnly;
      out = QFileDialog::getExistingDirectory( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _options() );

    } else {

      Debug::Throw() << "FileDialog::getFile - parent: " << qobject_cast<QWidget*>( parent() ) << endl;
      Debug::Throw() << "FileDialog::getFile - caption: " << _caption() << endl;
      Debug::Throw() << "FileDialog::getFile - selection: " << _selectedFile() << endl;
      Debug::Throw() << "FileDialog::getFile - filter: " << _filter() << endl;
      Debug::Throw() << "FileDialog::getFile - options: " << _options() << endl;

      out = QFileDialog::getOpenFileName( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, _options() );

    }

  } else {

    Debug::Throw( "FileDialog::getFile - getSaveFileName.\n" );
    out = QFileDialog::getSaveFileName( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, _options() );

  }

  Debug::Throw( "FileDialog::getFile. Done.\n" );

  // store working directory
  if( !out.isNull() ) _workingDirectory() = QFileInfo( out ).path();
  return out;

}

//_______________________________________________________
QStringList FileDialog::getFiles( void )
{

  Debug::Throw( "FileDialog::getFiles.\n" );
  assert( _acceptMode() == QFileDialog::AcceptOpen );
  assert( _fileMode() == QFileDialog::ExistingFiles );
  QStringList out( QFileDialog::getOpenFileNames( static_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, _options() ) );
  if( !(out.empty() || out.front().isNull() ) ) _workingDirectory() = QFileInfo( out.front() ).path();
  return out;
}
