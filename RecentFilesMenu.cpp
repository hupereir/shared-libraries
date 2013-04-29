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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "RecentFilesMenu.h"

#include "BaseIcons.h"
#include "FileRecordBaseProperties.h"
#include "IconEngine.h"
#include "QuestionDialog.h"
#include "XmlOptions.h"

#include <algorithm>

//_______________________________________________
RecentFilesMenu::RecentFilesMenu( QWidget *parent, FileList& files ):
    QMenu( parent ),
    Counter( "RecentFilesMenu" ),
    fileList_( &files )
{
    Debug::Throw( "RecentFilesMenu::RecentFilesMenu.\n" );

    setTitle( tr( "Open Recent" ) );
    connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _open( QAction* ) ) );
    connect( this, SIGNAL( aboutToShow( void ) ), fileList_, SLOT( checkValidFiles( void ) ) );
    connect( this, SIGNAL( aboutToShow( void ) ), SLOT( _loadFiles( void ) ) );
    connect( fileList_, SIGNAL( validFilesChecked( void ) ), SLOT( _updateActions( void ) ) );

    // icons
    setIcon( IconEngine::get( ICONS::OPEN ) );

    actionGroup_ = new QActionGroup( this );
    actionGroup_->setExclusive( true );

    addAction( cleanAction_ = new QAction( IconEngine::get( ICONS::DELETE ), tr( "Clean" ), this ) );
    connect( cleanAction_, SIGNAL( triggered() ), SLOT( _clean() ) );
    cleanAction_->setEnabled( false );
    addSeparator();

}

//______________________________________
RecentFilesMenu::~RecentFilesMenu( void )
{ Debug::Throw( "RecentFilesMenu::~RecentFilesMenu.\n" ); }

//______________________________________
bool RecentFilesMenu::openLastValidFile( void )
{
    Debug::Throw( "RecentFilesMenu::openLastValidFile.\n" );
    const FileRecord record( fileList_->lastValidFile() );
    if( record.file().isEmpty() ) return false;
    else {
        emit fileSelected( record );
        return true;
    }
}

//______________________________________
void RecentFilesMenu::setCurrentFile( const File& file )
{ if( !file.isEmpty() )  setCurrentFile( fileList_->add( file.expand() ) ); }

//______________________________________
void RecentFilesMenu::_updateActions( void )
{

    Debug::Throw( "RecentFilesMenu::_updateActions.\n" );

    // set actions enability
    FileRecord::List records( fileList_->records() );
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    {

        FileRecord::List::const_iterator found = std::find_if(
            records.begin(),
            records.end(),
            FileRecord::SameFileFTor( iter.value().file() ) );
        if( found == records.end() ) continue;
        iter.value().setValid( found->isValid() );
        iter.key()->setEnabled( found->isValid() );

    }

    cleanAction_->setEnabled( fileList_->cleanEnabled() );

}

//______________________________________
void RecentFilesMenu::_clean( void )
{
    if( !fileList_->check() && !QuestionDialog( this, tr( "Clear list ?" ) ).exec() ) return;
    else if( fileList_->check() && !QuestionDialog( this, tr( "Remove invalid or duplicated files from list ?" ) ).exec() ) return;
    fileList_->clean();
}

//_______________________________________________
void RecentFilesMenu::_open( QAction* action )
{

    Debug::Throw( "RecentFilesMenu::_Open.\n" );

    // find Action in map
    ActionMap::iterator iter( actions_.find( action ) );
    if( iter == actions_.end() ) return;
    emit fileSelected( iter.value() );

}

//_______________________________________________
void RecentFilesMenu::_loadFiles( void )
{
    Debug::Throw( "RecentFilesMenu::_loadFiles.\n" );

    // run thread to check file validity
    cleanAction_->setEnabled( fileList_->cleanEnabled() );

    // clear menu an actions map
    for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter.key(); }
    actions_.clear();

    // redo all actions
    FileRecord::List records( fileList_->records() );
    if( XmlOptions::get().get<bool>("SORT_FILES_BY_DATE") ) { std::sort( records.begin(), records.end(), FileRecord::FirstOpenFTor() ); }
    else { std::sort( records.begin(), records.end(), FileRecord::FileFTor() ); }

    // retrieve stored file record
    foreach( const FileRecord& record, records )
    {

        QString label( record.file() );
        QAction* action = addAction( label );

        // add icon
        if( record.hasProperty( FileRecordProperties::ICON ) ) { action->setIcon( IconEngine::get( record.property( FileRecordProperties::ICON ) ) ); }

        // check action if match file
        action->setCheckable( true );
        action->setChecked( record.file() == currentFile().file() );
        actionGroup_->addAction( action );

        if( fileList_->check() ) action->setEnabled( record.file().size() && record.isValid() );
        actions_.insert( action, record );
    }

}
