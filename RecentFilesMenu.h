#ifndef RecentFilesMenu_h
#define RecentFilesMenu_h

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

#include "Counter.h"
#include "Debug.h"
#include "FileList.h"

#include <QMenu>
#include <QHash>

/*!
\class   RecentFilesMenu
\brief   customized popup menu to open a previously opened file
*/
class RecentFilesMenu: public QMenu, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    RecentFilesMenu( QWidget*, FileList& );

    //! destructor
    virtual ~RecentFilesMenu( void );

    //! check if a file is in database, removes it if yes
    virtual void remove( const QString& file )
    { fileList_->remove( File( file ).expand() ); }

    //! check if a file is in database, adds it if not, select the file
    virtual void select( const QString& file )
    {
        File newFile( File( file ).expand() );
        emit fileSelected( fileList_->add( newFile ) );
    }

    //! open last valid file, returns true if any
    bool openLastValidFile( void );

    //! current file
    const FileRecord& currentFile( void ) const
    { return currentFile_; }

    Q_SIGNALS:

    //! signal emited when a file is selected
    void fileSelected( FileRecord );

    public Q_SLOTS:

    //! set current file
    void setCurrentFile( const File& file );

    //! set current file
    void setCurrentFile( const FileRecord& file )
    { currentFile_ = file; }

    //! check if a file is in database, adds it if not
    virtual FileRecord& add( const QString& file )
    { return fileList_->add( File( file ) ); }

    //! check if a file is in database, adds it if not
    virtual FileRecord& add( File file )
    { return fileList_->add( file.expand() ); }

    private Q_SLOTS:

    //! update actions
    /*! this is trigger by the fileList valid file check completion */
    void _updateActions( void );

    //! remove unfound files from file list
    void _clean( void );

    //! open file
    void _open( QAction* );

    //! display files from internal list
    void _loadFiles( void );

    private:

    //! associated file list
    FileList* fileList_;

    //! store current file (to appear checked in list)
    FileRecord currentFile_;

    //! action group
    QActionGroup* actionGroup_;

    //! clean action
    QAction* cleanAction_;

    //! map actions to file recors
    using ActionMap = QHash<QAction*, FileRecord>;

    //! map actions to FileRecords
    ActionMap actions_;

};

#endif
