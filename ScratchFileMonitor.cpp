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

#include <QtGui/QApplication>

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

    for( FileSet::const_iterator iter = files_.begin(); iter != files_.end(); iter++ )
    {

        if( iter->exists() && iter->isWritable() && (iter->isLink() || !iter->isDirectory() ) )
        { records.push_back( FileRecord( *iter ) ); }

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

    // re-add directories
    for( FileSet::const_iterator iter = files_.begin(); iter != files_.end(); iter++ )
    { if( iter->isDirectory() && !iter->isLink() ) temp.insert( *iter ); }

    // remove
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

}
