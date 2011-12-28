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

#include "RecentFilesMenu.h"

#include "BaseIcons.h"
#include "FileRecordBaseProperties.h"
#include "IconEngine.h"
#include "QuestionDialog.h"
#include "XmlOptions.h"

#include <algorithm>
#include <set>

//_______________________________________________
RecentFilesMenu::RecentFilesMenu( QWidget *parent, FileList& files ):
    QMenu( parent ),
    Counter( "RecentFilesMenu" ),
    fileList_( &files )
{
    Debug::Throw( "RecentFilesMenu::RecentFilesMenu.\n" );

    setTitle( "Open &Recent" );
    connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _open( QAction* ) ) );
    connect( this, SIGNAL( aboutToShow( void ) ), &_fileList(), SLOT( checkValidFiles( void ) ) );
    connect( this, SIGNAL( aboutToShow( void ) ), SLOT( _loadFiles( void ) ) );
    connect( &_fileList(), SIGNAL( validFilesChecked( void ) ), SLOT( _updateActions( void ) ) );

    // icons
    setIcon( IconEngine::get( ICONS::OPEN ) );

    actionGroup_ = new QActionGroup( this );
    actionGroup_->setExclusive( true );

    addAction( cleanAction_ = new QAction( IconEngine::get( ICONS::DELETE ), "&Clean", this ) );
    connect( &_cleanAction(), SIGNAL( triggered() ), SLOT( _clean() ) );
    _cleanAction().setEnabled( false );
    addSeparator();

}

//______________________________________
RecentFilesMenu::~RecentFilesMenu( void )
{ Debug::Throw( "RecentFilesMenu::~RecentFilesMenu.\n" ); }

//______________________________________
bool RecentFilesMenu::openLastValidFile( void )
{

    Debug::Throw( "RecentFilesMenu::openLastValidFile.\n" );
    FileRecord record( _fileList().lastValidFile() );
    if( record.file().isEmpty() ) return false;

    emit fileSelected( record );
    return true;

}

//______________________________________
void RecentFilesMenu::setCurrentFile( const File& file )
{ if( !file.isEmpty() ) setCurrentFile( _fileList().add( file.expand() ) ); }

//______________________________________
void RecentFilesMenu::_updateActions( void )
{

    Debug::Throw( "RecentFilesMenu::_updateActions.\n" );

    // set actions enability
    FileRecord::List records( _fileList().records() );
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    {

        FileRecord::List::const_iterator found = find_if(
            records.begin(),
            records.end(),
            FileRecord::SameFileFTor( iter->second.file() ) );
        if( found == records.end() ) continue;
        iter->second.setValid( found->isValid() );
        iter->first->setEnabled( found->isValid() );

    }

    _cleanAction().setEnabled( _fileList().cleanEnabled() );

}

//______________________________________
void RecentFilesMenu::_clean( void )
{
    if( !_fileList().check() && !QuestionDialog( this,"clear list ?" ).exec() ) return;
    else if( _fileList().check() && !QuestionDialog( this,"Remove invalid or duplicated files from list ?" ).exec() ) return;
    _fileList().clean();
}

//_______________________________________________
void RecentFilesMenu::_open( QAction* action )
{

    Debug::Throw( "RecentFilesMenu::_Open.\n" );

    // find Action in map
    ActionMap::iterator iter( actions_.find( action ) );
    if( iter == actions_.end() ) return;
    emit fileSelected( iter->second );

}

//_______________________________________________
void RecentFilesMenu::_loadFiles( void )
{
    Debug::Throw( "RecentFilesMenu::_loadFiles.\n" );

    // run thread to check file validity
    _cleanAction().setEnabled( _fileList().cleanEnabled() );

    // clear menu an actions map
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter->first; }
    actions_.clear();

    // redo all actions
    FileRecord::List records( _fileList().records() );
    if( XmlOptions::get().get<bool>("SORT_FILES_BY_DATE") ) { sort( records.begin(), records.end(), FileRecord::FirstOpenFTor() ); }
    else { sort( records.begin(), records.end(), FileRecord::FileFTor() ); }

    // retrieve stored file record
    for( FileRecord::List::const_iterator iter = records.begin(); iter != records.end(); ++iter )
    {

        QString label( iter->file() );
        QAction* action = addAction( label );

        // add icon
        if( iter->hasProperty( FileRecordProperties::ICON ) ) { action->setIcon( IconEngine::get( iter->property( FileRecordProperties::ICON ) ) ); }

        // check action if match file
        action->setCheckable( true );
        action->setChecked( iter->file() == currentFile().file() );
        actionGroup_->addAction( action );

        if( _fileList().check() ) action->setEnabled( iter->file().size() && iter->isValid() );
        actions_.insert( std::make_pair( action, *iter ) );
    }

}
