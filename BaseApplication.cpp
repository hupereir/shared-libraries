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

/*!
  \file Application.cc
  \brief application main object
  \author  Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QApplication>
#include <QLayout>
#include <QLabel>
#include <QMessageBox>

#include "BaseIcons.h"
#include "ErrorHandler.h"
#include "FlatStyle.h"
#include "IconEngine.h"
#include "BaseApplication.h"
#include "QtUtil.h"
#include "XmlOptions.h"
#include "Util.h"

using namespace std;
using namespace SERVER;
using namespace Qt;

//____________________________________________
BaseApplication::BaseApplication( QObject* parent, ArgList arguments ) :
  QObject( parent ),
  application_manager_( 0 ),
  arguments_( arguments ),
  realized_( false )
{ 
  
  Debug::Throw( "BaseApplication::BaseApplication.\n" ); 
  connect( this, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
  if( XmlOptions::get().get<bool>( "USE_FLAT_THEME" ) ) qApp->setStyle( new FlatStyle() );

}

//____________________________________________
BaseApplication::~BaseApplication( void )
{ 

  Debug::Throw( "BaseApplication::~BaseApplication.\n" );
  emit saveConfiguration();
  XmlOptions::write();

  if( _hasApplicationManager() ) 
  {
    delete application_manager_; 
    application_manager_ = 0;
  }

  // ErrorHandler::get().exit();  
  Debug::Throw( "BaseApplication::~BaseApplication - done.\n" );
  
}

//____________________________________________
void BaseApplication::initApplicationManager( void )
{
  Debug::Throw( "BaseApplication::initApplicationManager.\n" ); 

  if( _hasApplicationManager() ) return;

  if( arguments_.find( "--no-server" ) ) 
  {
    realizeWidget();
    return;
  }
  
  // create application manager
  application_manager_ = new ApplicationManager( this );
  application_manager_->setApplicationName( XmlOptions::get().get<string>( "APP_NAME" ).c_str() );
  
  // connections
  connect( 
    application_manager_, SIGNAL( stateChanged( SERVER::ApplicationManager::State ) ),
    SLOT( _stateChanged( SERVER::ApplicationManager::State ) ) );
    
  connect( 
    application_manager_, SIGNAL( serverRequest( const ArgList& ) ), 
    SLOT( _processRequest( const ArgList& ) ) );
    
  // initialization
  application_manager_->init( arguments_ );
  Debug::Throw( "BaseApplication::initApplicationManager - done.\n" ); 
  
}

//____________________________________________
bool BaseApplication::realizeWidget( void )
{
  Debug::Throw( "BaseApplication::realizeWidget.\n" );
   
  //! check if the method has already been called.
  if( realized_ ) return false;

  // actions
  about_action_ = new QAction( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).c_str() ), "About &this", this );
  connect( about_action_, SIGNAL( triggered() ), SLOT( _about() ) ); 

  aboutqt_action_ = new QAction( IconEngine::get( ICONS::ABOUT_QT ), "About &Qt", this );
  connect( aboutqt_action_, SIGNAL( triggered() ), qApp, SLOT( aboutQt() ) ); 

  close_action_ = new QAction( IconEngine::get( ICONS::EXIT ), "E&xit", this );
  close_action_->setShortcut( CTRL+Key_Q );
  connect( close_action_, SIGNAL( triggered() ), qApp, SLOT( quit() ) ); 
  
  configuration_action_ = new QAction( IconEngine::get( ICONS::CONFIGURE ), "Default &Configuration", this );
  connect( configuration_action_, SIGNAL( triggered() ), SLOT( _configuration() ) ); 
  
  realized_ = true;
  return true;
  
}

//_______________________________________________
void BaseApplication::_about( QString name, QString version, QString stamp )
{

  Debug::Throw( "BaseApplication::about.\n" );
  
  // modify version to remvoe qt4 for version
  if( version.indexOf( "qt4_" ) >= 0 )
  { version = version.replace( "qt4_", "" ) + " (qt4)"; }
  
  QString buffer;
  QTextStream in( &buffer );
  if( !name.isEmpty() ) { in << "<h3>" << name << "</h3>"; }
  if( !version.isEmpty() ) { in << "version " << version; }
  if( !stamp.isEmpty() ) { in << " (" << stamp << ")"; }

  in 
    << "<p>This application was written for personal use only. "
    << "It is not meant to be bug free, although all efforts "
    << "are made so that it remains/becomes so. "
    
    << "<p>Suggestions, comments and bug reports are welcome. "
    << "Please use the following e-mail address:"

    << "<p><a href=\"mailto:hugo.pereira@free.fr\">hugo.pereira@free.fr</a>";

  QMessageBox dialog;
  dialog.setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
  dialog.setIconPixmap( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
  dialog.setText( buffer );
  dialog.adjustSize();
  QtUtil::centerOnWidget( &dialog, qApp->activeWindow() );
  dialog.exec();

}

//_______________________________________________
void BaseApplication::_updateConfiguration( void )
{
  Debug::Throw( "BaseApplication::_updateConfiguration.\n" );
  
  // application icon
  qApp->setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );

  // set fonts
  QFont font;
  font.fromString( XmlOptions::get().raw( "FONT_NAME" ).c_str() );
  qApp->setFont( font );
  
  font.fromString( XmlOptions::get().raw( "FIXED_FONT_NAME" ).c_str() );
  qApp->setFont( font, "QLineEdit" ); 
  qApp->setFont( font, "QTextEdit" ); 
    
  // debug
  Debug::setLevel( XmlOptions::get().get<int>( "DEBUG_LEVEL" ) );
    
  // reload IconEngine cache (in case of icon_path_list that changed)
  IconEngine::get().reload();

  // emit signal to propagate changes to other widgets
  Debug::Throw( "BaseApplication::_updateConfiguration - done.\n" );
  
}

//________________________________________________
void BaseApplication::_stateChanged( ApplicationManager::State state )
{

  Debug::Throw() << "BaseApplication::_stateChanged - state=" << state << endl;

  switch ( state ) 
  {
  
    case ApplicationManager::ALIVE:
    realizeWidget();
    break;
    
    case ApplicationManager::DEAD:
    qApp->quit();
    break;
    
    default:
    break;
  }
  
  return;
  
}
  
//____________________________________________
void BaseApplication::busy( void )
{
  qApp->setOverrideCursor( Qt::WaitCursor ); 
  qApp->processEvents(); 
}
  
//____________________________________________
void BaseApplication::idle( void )
{ qApp->restoreOverrideCursor(); }
