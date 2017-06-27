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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "Debug.h"
#include "File.h"
#include "FileDialog.h"
#include "Util.h"

//_______________________________________________________
File& FileDialog::_workingDirectory()
{
    static File workingDirectory = Util::workingDirectory();
    return workingDirectory;
}

//_______________________________________________________
FileDialog::FileDialog( QWidget* parent ):
    QObject( parent ),
    Counter( "FileDialog" ),
    selectedFile_( _workingDirectory() )
{ Debug::Throw( "FileDialog::FileDialog.\n" ); }

//_______________________________________________________
FileDialog& FileDialog::selectFile( const File& file )
{
    selectedFile_ = file;
    _workingDirectory() = QFileInfo( file ).isDir() ? File( file ): File( file ).path();
    return *this;
}

//_______________________________________________________
File FileDialog::getFile()
{
    Debug::Throw( "FileDialog::getFile.\n" );
    File out;
    if( acceptMode_ == QFileDialog::AcceptOpen )
    {
        if( fileMode_ == QFileDialog::DirectoryOnly || fileMode_ == QFileDialog::Directory )
        {

            options_ |= QFileDialog::ShowDirsOnly;
            out = File( QFileDialog::getExistingDirectory( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), options_ ) );

        } else {

            out = File( QFileDialog::getOpenFileName( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, options_ ) );

        }

    } else {

        Debug::Throw( "FileDialog::getFile - getSaveFileName.\n" );
        out = File( QFileDialog::getSaveFileName( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, options_ ) );

    }

    Debug::Throw( "FileDialog::getFile. Done.\n" );

    // store working directory
    if( !out.isEmpty() ) _workingDirectory() = File( QFileInfo( out ).path() );
    return out;

}

//_______________________________________________________
File::List FileDialog::getFiles()
{

    Debug::Throw( "FileDialog::getFiles.\n" );
    Q_ASSERT( acceptMode_ == QFileDialog::AcceptOpen );
    Q_ASSERT( fileMode_ == QFileDialog::ExistingFiles );
    auto files( QFileDialog::getOpenFileNames( static_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, options_ ) );
    if( !(files.empty() || files.front().isEmpty() ) ) _workingDirectory() = File( QFileInfo( files.front() ).path() );

    File::List out;
    for( auto&& file:files ) { out.append( File( file ) ); }
    return out;
}
