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

#include "ScratchFileMonitor.h"
#include "ScratchFileRemoveDialog.h"

#include <QApplication>

//_______________________________________________________________________________________
void ScratchFileMonitor::add( const File& file )
{
    Debug::Throw() << "ScratchFileMonitor::add - " << file << endl;
    files_.insert( file );
}

//_______________________________________________________________________________________
void ScratchFileMonitor::deleteScratchFiles()
{
    Debug::Throw( "ScratchFileMonitor::deleteScratchFiles.\n" );

    // create records
    FileRecordModel::List records;
    for( const auto& file:files_ )
    {

        if( file.exists() && file.isWritable() && (file.isLink() || !file.isDirectory() ) )
        { records.append( FileRecord( file ) ); }

    }

    if( records.empty() ) return;

    // create dialog
    ScratchFileRemoveDialog dialog( nullptr, records );
    if( dialog.exec() == QDialog::Rejected  || ( records = dialog.selectedFiles() ).empty() ) return;

    // remove all selected files
    for( const auto& record:records )
    {
        record.file().remove();
        files_.remove( record.file() );
    }

    // try remove directories
    // subdirectories are automatically removed unless non-empty
    for( const auto& file:files_ )
    {
        if( file.isLink() || !file.isDirectory() ) continue;

        // get list of contained files
        const auto files( file.listFiles( File::ListFlag::Recursive ) );
        if( std::none_of( files.begin(), files.end(), []( const File& file ) { return file.isLink() || !file.isDirectory(); } ) )
        { file.removeRecursive(); }

    }

}
