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
#include "BaseIconNames.h"
#include "FileRecordBaseProperties.h"
#include "IconEngine.h"
#include "QuestionDialog.h"
#include "XmlOptions.h"

#include <QActionGroup>
#include <algorithm>

//_______________________________________________
RecentFilesMenu::RecentFilesMenu( QWidget *parent, FileList& files ):
    QMenu( parent ),
    Counter( QStringLiteral("RecentFilesMenu") ),
    fileList_( &files )
{
    Debug::Throw( QStringLiteral("RecentFilesMenu::RecentFilesMenu.\n") );

    setTitle( tr( "Open Recent" ) );
    connect( this, &QMenu::triggered, this, &RecentFilesMenu::_open );
    connect( this, &QMenu::aboutToShow, fileList_, &FileList::checkValidFiles );
    connect( this, &QMenu::aboutToShow, this, &RecentFilesMenu::_loadFiles );
    connect( fileList_, &FileList::validFilesChecked, this, &RecentFilesMenu::_updateActions );

    // icons
    setIcon( IconEngine::get( IconNames::Open ) );

    actionGroup_ = new QActionGroup( this );
    actionGroup_->setExclusive( true );

    addAction( cleanAction_ = new QAction( IconEngine::get( IconNames::Delete ), tr( "Clean" ), this ) );
    connect( cleanAction_, &QAction::triggered, this, &RecentFilesMenu::_clean );
    cleanAction_->setEnabled( false );
    addSeparator();

}

//______________________________________
bool RecentFilesMenu::openLastValidFile()
{
    Debug::Throw( QStringLiteral("RecentFilesMenu::openLastValidFile.\n") );
    const FileRecord record( fileList_->lastValidFile() );
    if( record.file().isEmpty() ) return false;
    else {
        emit fileSelected( record );
        return true;
    }
}

//______________________________________
void RecentFilesMenu::setCurrentFile( const File& file )
{ if( !file.isEmpty() )  setCurrentFile( fileList_->add( file.expanded() ) ); }

//______________________________________
void RecentFilesMenu::_updateActions()
{

    Debug::Throw( QStringLiteral("RecentFilesMenu::_updateActions.\n") );

    // set actions enability
    FileRecord::List records( fileList_->records() );
    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    {

        const auto found = std::find_if(
            records.begin(),
            records.end(),
            FileRecord::SameFileFTorUnary( iter.value().file() ) );
        if( found != records.end() )
        {
            iter.value().setValid( found->isValid() );
            iter.key()->setEnabled( found->isValid() );
        }

    }

    cleanAction_->setEnabled( fileList_->cleanEnabled() );

}

//______________________________________
void RecentFilesMenu::_clean()
{
    if( !fileList_->check() && !QuestionDialog( this, tr( "Clear list ?" ) ).exec() ) return;
    else if( fileList_->check() && !QuestionDialog( this, tr( "Remove invalid or duplicated files from list ?" ) ).exec() ) return;
    fileList_->clean();
}

//_______________________________________________
void RecentFilesMenu::_open( QAction* action )
{

    Debug::Throw( QStringLiteral("RecentFilesMenu::_Open.\n") );

    // find Action in map
    ActionMap::iterator iter( actions_.find( action ) );
    if( iter == actions_.end() ) return;
    emit fileSelected( iter.value() );

}

//_______________________________________________
void RecentFilesMenu::_loadFiles()
{
    Debug::Throw( QStringLiteral("RecentFilesMenu::_loadFiles.\n") );

    // run thread to check file validity
    cleanAction_->setEnabled( fileList_->cleanEnabled() );

    // clear menu and actions map
    for( auto&& iter = actions_.begin(); iter != actions_.end(); ++iter )
    { delete iter.key(); }
    actions_.clear();

    // redo all actions
    FileRecord::List records( fileList_->records() );
    if( XmlOptions::get().get<bool>(QStringLiteral("SORT_FILES_BY_DATE")) ) { std::sort( records.begin(), records.end(), FileRecord::FirstOpenFTor() ); }
    else { std::sort( records.begin(), records.end(), FileRecord::FileFTor() ); }

    // retrieve stored file record
    for( const auto& record:records )
    {

        QString label( record.file() );
        QAction* action = addAction( label );

        // add icon
        if( record.hasProperty( FileRecordProperties::Icon ) ) { action->setIcon( IconEngine::get( record.property( FileRecordProperties::Icon ) ) ); }

        // check action if match file
        action->setCheckable( true );
        action->setChecked( record.file() == currentFile().file() );
        actionGroup_->addAction( action );

        if( fileList_->check() ) action->setEnabled( !record.file().isEmpty() && record.isValid() );
        actions_.insert( action, record );
    }

}
