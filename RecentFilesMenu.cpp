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

/*!
   \file RecentFilesMenu.cpp
   \brief customized popup menu to open a previous logbook
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <algorithm>
#include <QApplication>
#include <sstream>
#include <set>

#include "BaseIcons.h"
#include "File.h"
#include "IconEngine.h"
#include "RecentFilesMenu.h"
#include "QtUtil.h"
#include "XmlOptions.h"

using namespace std;

//_______________________________________________
RecentFilesMenu::RecentFilesMenu( QWidget *parent ):
  QMenu( parent ),
  valid_file_thread_( this )
{ 
  Debug::Throw( "RecentFilesMenu::RecentFilesMenu.\n" );
  
  setTitle( "Open &Recent" );
  
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _open( QAction* ) ) );
  connect( this, SIGNAL( aboutToShow() ), SLOT( _loadFiles() ) );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _saveConfiguration() ) );
  _updateConfiguration();

  // icons
  setIcon( IconEngine::get( ICONS::OPEN ) );

  // clean action
  clean_action_ = new QAction( IconEngine::get( ICONS::DELETE ), "&Clean", this );
  connect( clean_action_, SIGNAL( triggered() ), SLOT( _clean() ) );
  addAction( clean_action_ );
  addSeparator();
  
  // action group
  action_group_ = new QActionGroup( this );
  action_group_->setExclusive( true );
  
}

//______________________________________
RecentFilesMenu::~RecentFilesMenu( void )
{ Debug::Throw( "RecentFilesMenu::~RecentFilesMenu.\n" ); }

//______________________________________
bool RecentFilesMenu::read( void )
{
  Debug::Throw( "RecentFilesMenu::read.\n" );
  if( !XmlFileList::read() ) return false;
  
  // run separate thread to check file validity
  if( _check() ) _checkValidFiles();
  
  return true;
}
  
//______________________________________ 
bool RecentFilesMenu::openLastValidFile( void ) 
{    
  
  Debug::Throw( "RecentFilesMenu::openLastValidFile.\n" );
  FileRecord record( lastValidFile() );
  if( record.file().empty() ) return false;
  
  emit fileSelected( record );
  return true;

}


//_______________________________________________
void RecentFilesMenu::_checkValidFiles( void )
{
  Debug::Throw( "RecentFilesMenu::_CheckValidFiles.\n" );
  if( valid_file_thread_.isRunning() ) return;
  valid_file_thread_.setFiles( _records() );
  valid_file_thread_.start();
}

//_______________________________________________ 
void RecentFilesMenu::customEvent( QEvent* event )
{
  if( event->type() != QEvent::User ) 
  { 
    Debug::Throw() << "EditFrame::customEvent - unrecognized type " << event->type() << endl;
    return;
  }
  
  ValidFileEvent* valid_file_event( dynamic_cast<ValidFileEvent*>(event) );
  if( !valid_file_event ) return;
  
  Debug::Throw( "RecentFilesMenu::customEvent.\n" );
  
  // set file records validity
  FileRecord::List& current_file_list( _records() );
  const FileRecord::List& file_list( valid_file_event->files() ); 
  for( FileRecord::List::iterator iter = current_file_list.begin(); iter != current_file_list.end(); iter++ )
  {
    FileRecord::List::const_iterator found = find_if( 
      file_list.begin(),
      file_list.end(), 
      FileRecord::SameFileFTor( iter->file() ) );
    if( found == file_list.end() ) continue;
    iter->setValid( found->isValid() );
  }
    
  // set actions enability
  for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); iter++ )
  {
    FileRecord::List::const_iterator found = find_if( 
      file_list.begin(),
      file_list.end(), 
      FileRecord::SameFileFTor( iter->second.file() ) );
    if( found == file_list.end() ) continue;
    iter->second.setValid( found->isValid() );
    iter->first->setEnabled( found->isValid() );
    
    // enable clean button
    if( !found->isValid() ) clean_action_->setEnabled( true );
    
  }
  
  Debug::Throw( "RecentFilesMenu::customEvent. Done.\n" ); 
  
  return;
}

//______________________________________
void RecentFilesMenu::_updateConfiguration( void )
{
  Debug::Throw( "RecentFilesMenu::_updateConfiguration.\n" );
  
  // DB file
  setDBFile( XmlOptions::get().raw("DB_FILE") );
  setMaxSize( XmlOptions::get().get<int>( "DB_SIZE" ) );
  return;
  
}

//______________________________________
void RecentFilesMenu::_saveConfiguration( void )
{
  Debug::Throw( "RecentFilesMenu::_saveConfiguration.\n" );
  write();
}
  
//______________________________________
void RecentFilesMenu::_clean( void )
{    
  if( !_check() && !QtUtil::questionDialog( this,"clear list ?" ) ) return;
  else if( _check() && !QtUtil::questionDialog( this,"Remove invalid files from list ?" ) ) return;
  XmlFileList::_clean();
}

//_______________________________________________
void RecentFilesMenu::_open( QAction* action )
{
  
  Debug::Throw( "RecentFilesMenu::_Open.\n" );
  
  // find Action in map
  ActionMap::iterator iter( actions_.find( action ) );
  if( iter == actions_.end() ) return;
  //emit fileSelected( _store( iter->second ) );
  emit fileSelected( iter->second );
  
}

//_______________________________________________
void RecentFilesMenu::_loadFiles( void )
{
  Debug::Throw( "RecentFilesMenu::_loadFiles.\n" );
  
  // run thread to check file validity
  if( _check() ) _checkValidFiles(); 
  if( _check() ) clean_action_->setEnabled( _invalidFiles() );
  
  // clear menu an actions map
  for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); iter++ )
  { delete iter->first; }
  actions_.clear();

  // redo all actions
  FileRecord::List records( _records() ); 
  if( XmlOptions::get().get<bool>("SORT_FILES_BY_DATE") ) { sort( records.begin(), records.end(), FileRecord::FirstOpenFTor() ); }
  else { sort( records.begin(), records.end(), FileRecord::SameFileFTor() ); }

  // retrieve stored file record
  const FileRecord& stored( _stored() );
  for( FileRecord::List::const_iterator iter = records.begin(); iter != records.end(); iter++ )
  {
    
    QString label( iter->file().c_str() );
    QAction* action = addAction( label );
    
    // add icon
    if( iter->hasProperty( "icon" ) ) { action->setIcon( IconEngine::get( iter->property( "icon" ) ) ); }
    
    // check action if match file
    action_group_->addAction( action );
    action->setCheckable( true );
    action->setChecked( iter->file() == stored.file() );

    if( _check() ) action->setEnabled( iter->file().size() && iter->isValid() );
    actions_.insert( make_pair( action, *iter ) );
  }
    
}
