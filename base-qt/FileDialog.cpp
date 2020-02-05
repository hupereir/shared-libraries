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
FileDialog::FileDialog( QWidget* parent ):
    QObject( parent ),
    Counter( QStringLiteral("FileDialog") ),
    selectedFile_( _workingDirectory() )
{ Debug::Throw( QStringLiteral("FileDialog::FileDialog.\n") ); }

//_______________________________________________________
FileDialog& FileDialog::selectFile( const File& file )
{
    Debug::Throw( QStringLiteral("FileDialog::selectFile.\n") );
    selectedFile_ = file;
    _workingDirectory() = QFileInfo( file ).isDir() ? File( file ): File( file ).path();
    return *this;
}

//_______________________________________________________
File FileDialog::getFile()
{
    Debug::Throw( QStringLiteral("FileDialog::getFile.\n") );
    File out;
    const auto caption = caption_.isEmpty() ? _defaultCaption() : Util::windowTitle( caption_ );
    if( acceptMode_ == QFileDialog::AcceptOpen )
    {
        if( fileMode_ == QFileDialog::DirectoryOnly || fileMode_ == QFileDialog::Directory )
        {

            options_ |= QFileDialog::ShowDirsOnly;
            out = File( QFileDialog::getExistingDirectory( qobject_cast<QWidget*>( parent() ), caption, selectedFile_, options_ ) );

        } else {

            out = File( QFileDialog::getOpenFileName( qobject_cast<QWidget*>( parent() ), caption, selectedFile_, filter_, nullptr, options_ ) );

        }

    } else {

        Debug::Throw( QStringLiteral("FileDialog::getFile - getSaveFileName.\n") );
        out = File( QFileDialog::getSaveFileName( qobject_cast<QWidget*>( parent() ), caption, selectedFile_, filter_, nullptr, options_ ) );

    }

    Debug::Throw( QStringLiteral("FileDialog::getFile. Done.\n") );

    // store working directory
    if( !out.isEmpty() ) _workingDirectory() = File( QFileInfo( out ).path() );
    return out;

}

//_______________________________________________________
void FileDialog::setWorkingDirectory( const File& value )
{
    Debug::Throw( QStringLiteral("FileDialog::setWorkingDirectory.\n") );
    _workingDirectory() = value;
}

//_______________________________________________________
File& FileDialog::_workingDirectory()
{
    static File workingDirectory = Util::workingDirectory();
    return workingDirectory;
}

//_______________________________________________________
QString FileDialog::_defaultCaption() const
{
    if( acceptMode_ == QFileDialog::AcceptOpen )
    {

        if( fileMode_ == QFileDialog::DirectoryOnly || fileMode_ == QFileDialog::Directory ) return Util::windowTitle( tr( "Open Directory" ) );
        else return Util::windowTitle( tr( "Open File" ) );

    } else {

        return Util::windowTitle( tr( "Save File" ) );

    }

}
