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
   \file OpenPreviousMenu.cc
   \brief customized popup menu to open a previous logbook
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QApplication>
#include <sstream>
#include <set>

#include "BaseIcons.h"
#include "File.h"
#include "IconEngine.h"
#include "OpenPreviousMenu.h"
#include "XmlOptions.h"
#include "QtUtil.h"

using namespace std;

//_______________________________________________
OpenPreviousMenu::OpenPreviousMenu( QWidget *parent ):
  QMenu( parent ),
  valid_file_thread_( this )
{ 
  Debug::Throw( "OpenPreviousMenu::OpenPreviousMenu.\n" ); 
  connect( this, SIGNAL( triggered( QAction* ) ), SLOT( _open( QAction* ) ) );
  connect( this, SIGNAL( aboutToShow() ), SLOT( _loadFiles() ) );
  connect( qApp, SIGNAL( configurationChanged() ), SLOT( updateConfiguration() ) );
  updateConfiguration();

  // clean action
  list<string> path_list( XmlOptions::get().specialOptions<string>( "PIXMAP_PATH" ) );
  clean_action_ = new QAction( IconEngine::get( ICONS::DELETE, path_list ), "&Clean", 0 );
  connect( clean_action_, SIGNAL( triggered() ), SLOT( _clean() ) );
  addAction( clean_action_ );
  addSeparator();
  
}

//______________________________________
OpenPreviousMenu::~OpenPreviousMenu( void )
{ Debug::Throw( "OpenPreviousMenu::~OpenPreviousMenu.\n" ); }

//______________________________________
bool OpenPreviousMenu::read( void )
{
  Debug::Throw( "OpenPreviousMenu::read.\n" );
  if( !XmlFileList::read() ) return false;
  
  // run separate thread to check file validity
  if( _check() ) _checkValidFiles();
  
  return true;
}
  
//______________________________________ 
bool OpenPreviousMenu::openLastValidFile( void ) 
{    
  
  Debug::Throw( "OpenPreviousMenu::openLastValidFile.\n" );
  FileRecord record( lastValidFile() );
  if( record.file().empty() ) return false;
  
  emit fileSelected( record );
  return true;

}

//______________________________________
void OpenPreviousMenu::updateConfiguration( void )
{
  Debug::Throw( "OpenPreviousMenu::updateConfiguration.\n" );
  
  // DB file
  setDBFile( XmlOptions::get().get<string>("DB_FILE") );
  setMaxSize( XmlOptions::get().get<int>( "DB_SIZE" ) );
  return;
  
}

//______________________________________
void OpenPreviousMenu::_clean( void )
{    
  if( !_check() && !QtUtil::questionDialog( this,"clear list ?" ) ) return;
  else if( _check() && !QtUtil::questionDialog( this,"Remove invalid files from list ?" ) ) return;
  XmlFileList::_clean();
}

//_______________________________________________
void OpenPreviousMenu::_open( QAction* action )
{
  
  Debug::Throw( "OpenPreviousMenu::_Open.\n" );
  
  // find Action in map
  ActionMap::iterator iter( actions_.find( action ) );
  if( iter == actions_.end() ) return;
  emit fileSelected( _store( iter->second ) );
  
}

//_______________________________________________
void OpenPreviousMenu::_loadFiles( void )
{
  Debug::Throw( "OpenPreviousMenu::_loadFiles.\n" );
  
  // run thread to check file validity
  if( _check() ) _checkValidFiles(); 
  if( _check() ) clean_action_->setEnabled( _invalidFiles() );
  
  // clear menu an actions map
  for( ActionMap::iterator iter = actions_.begin(); iter != actions_.end(); iter++ )
  { delete iter->first; }
  actions_.clear();

  // redo all actions
  FileRecord::List records( _records() );
  
  if( XmlOptions::get().get<bool>("SORT_FILES_BY_DATE") )
  { records.sort( FileRecord::FirstOpenFTor() ); }
  else { records.sort( FileRecord::SameFileFTor() ); }
  
  // retrieve stored file record
  const FileRecord& stored( _stored() );
  
  for( FileRecord::List::const_iterator iter = records.begin(); iter != records.end(); iter++ )
  {
    
    QString label( iter->file().c_str() );
    
    QAction* action = addAction( label );
    if( iter->file() == stored.file() )
    {
      QFont font( QMenu::font() );
      font.setWeight( QFont::Bold );
      action->setFont( font );
    }
    
    if( _check() ) action->setEnabled( iter->file().size() && iter->isValid() );
    actions_.insert( make_pair( action, *iter ) );
  }
    
}

//_______________________________________________
void OpenPreviousMenu::_checkValidFiles( void )
{
  Debug::Throw( "OpenPreviousMenu::_CheckValidFiles.\n" );
  if( valid_file_thread_.isRunning() ) return;
  valid_file_thread_.setFiles( _records() );
  valid_file_thread_.start();
}

//_______________________________________________ 
void OpenPreviousMenu::customEvent( QEvent* event )
{
  if( event->type() != QEvent::User ) 
  { 
    Debug::Throw() << "EditFrame::customEvent - unrecognized type " << event->type() << endl;
    return;
  }
  
  ValidFileEvent* valid_file_event( dynamic_cast<ValidFileEvent*>(event) );
  if( !valid_file_event ) return;
  
  Debug::Throw( "OpenPreviousMenu::customEvent.\n" );
  
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
  
  Debug::Throw( "OpenPreviousMenu::customEvent. Done.\n" ); 
  
  return;
}
