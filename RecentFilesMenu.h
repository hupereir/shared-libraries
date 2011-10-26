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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#ifndef RecentFilesMenu_h
#define RecentFilesMenu_h

/*!
\file   RecentFilesMenu.h
\brief   customized popup menu to open a previous logbook
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Counter.h"
#include "Debug.h"
#include "FileList.h"

#include <QtGui/QMenu>
#include <list>

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
    { _fileList().remove( File( file ).expand() ); }

    //! check if a file is in database, adds it if not, select the file
    virtual void select( const QString& file )
    {
        File new_file( File( file ).expand() );
        emit fileSelected( _fileList().add( new_file ) );
    }

    //! open last valid file, returns true if any
    bool openLastValidFile( void );

    //! current file
    const FileRecord& currentFile( void ) const
    { return currentFile_; }

    //! set current file
    void setCurrentFile( const File& file );

    //! set current file
    void setCurrentFile( const FileRecord& file )
    { currentFile_ = file; }

    signals:

    //! signal emited when a file is selected
    void fileSelected( FileRecord );

    public slots:

    //! check if a file is in database, adds it if not
    virtual FileRecord& add( const QString& file )
    { return _fileList().add( File( file ) ); }

    //! check if a file is in database, adds it if not
    virtual FileRecord& add( File file )
    { return _fileList().add( file.expand() ); }

    private slots:

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

    //! file list
    FileList& _fileList( void ) const
    {
        assert( fileList_ );
        return *fileList_;
    }

    //! clean action
    QAction& _cleanAction( void ) const
    { return *cleanAction_; }

    //! associated file list
    FileList* fileList_;

    //! store current file (to appear checked in list)
    FileRecord currentFile_;

    //! action group
    QActionGroup* actionGroup_;

    //! clean action
    QAction* cleanAction_;

    //! map actions to file recors
    typedef std::map<QAction*, FileRecord> ActionMap;

    //! map actions to FileRecords
    ActionMap actions_;

};

#endif
