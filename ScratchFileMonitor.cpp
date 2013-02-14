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
void ScratchFileMonitor::deleteScratchFiles( void )
{
    Debug::Throw( "ScratchFileMonitor::deleteScratchFiles.\n" );

    // create records
    FileRecordModel::List records;
    foreach( const File& file, files_ )
    {

        if( file.exists() && file.isWritable() && (file.isLink() || !file.isDirectory() ) )
        { records << FileRecord( file ); }

    }

    if( records.empty() ) return;

    // create dialog
    ScratchFileRemoveDialog dialog( 0L, records );
    if( dialog.exec() == QDialog::Rejected  || ( records = dialog.selectedFiles() ).empty() ) return;

    // convert back to QSet
    FileSet temp;
    {
        FileRecordModel::ListIterator iter( records );
        iter.toBack();
        while( iter.hasPrevious() )
        { temp.insert( iter.previous().file() ); }
    }

    // remove all files
    {
        FileSetIterator iter( temp );
        iter.toBack();
        while( iter.hasPrevious() )
        {
            const File& file( iter.previous() );
            file.remove();
            files_.remove( file );
        }
    }

    // try remove directories
    // subdirectories are automatically removed unless non-empty
    foreach( const File& file, files_ )
    {
        if( file.isLink() || !file.isDirectory() ) continue;

        // get list of contained files
        bool empty( true );
        foreach( const File& child, file.listFiles( File::Recursive ) )
        {
            if( child.isLink() || !child.isDirectory() )
            {
                empty = false;
                break;
            }
        }

        if( empty ) file.removeRecursive();
    }

}
