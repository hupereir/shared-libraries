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
QString& FileDialog::_workingDirectory( void )
{
    static QString workingDirectory = Util::workingDirectory();
    return workingDirectory;
}

//_______________________________________________________
FileDialog::FileDialog( QWidget* parent ):
    QObject( parent ),
    Counter( "FileDialog" ),
    selectedFile_( _workingDirectory() )
{ Debug::Throw( "FileDialog::FileDialog.\n" ); }

//_______________________________________________________
FileDialog& FileDialog::selectFile( const QString& file )
{
    selectedFile_ = file;
    _workingDirectory() = QFileInfo( file ).isDir() ? file: File( file ).path();
    return *this;
}

//_______________________________________________________
QString FileDialog::getFile( void )
{
    Debug::Throw( "FileDialog::getFile.\n" );
    QString out;
    if( acceptMode_ == QFileDialog::AcceptOpen )
    {
        if( fileMode_ == QFileDialog::DirectoryOnly || fileMode_ == QFileDialog::Directory )
        {

            options_ |= QFileDialog::ShowDirsOnly;
            out = QFileDialog::getExistingDirectory( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), options_ );

        } else {

            Debug::Throw() << "FileDialog::getFile - parent: " << qobject_cast<QWidget*>( parent() ) << endl;
            Debug::Throw() << "FileDialog::getFile - caption: " << _caption() << endl;
            Debug::Throw() << "FileDialog::getFile - selection: " << _selectedFile() << endl;
            Debug::Throw() << "FileDialog::getFile - filter: " << _filter() << endl;
            Debug::Throw() << "FileDialog::getFile - options: " << options_ << endl;

            out = QFileDialog::getOpenFileName( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, options_ );

        }

    } else {

        Debug::Throw( "FileDialog::getFile - getSaveFileName.\n" );
        out = QFileDialog::getSaveFileName( qobject_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, options_ );

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
    Q_ASSERT( acceptMode_ == QFileDialog::AcceptOpen );
    Q_ASSERT( fileMode_ == QFileDialog::ExistingFiles );
    QStringList out( QFileDialog::getOpenFileNames( static_cast<QWidget*>( parent() ), _caption(), _selectedFile(), _filter(), 0, options_ ) );
    if( !(out.empty() || out.front().isNull() ) ) _workingDirectory() = QFileInfo( out.front() ).path();
    return out;
}
