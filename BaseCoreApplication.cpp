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

#include "ErrorHandler.h"
#include "BaseCoreApplication.h"
#include "XmlOptions.h"

using namespace std;
using namespace SERVER;
using namespace Qt;

//____________________________________________
BaseCoreApplication::BaseCoreApplication( QObject* parent, CommandLineArguments arguments ) :
  QObject( parent ),
  application_manager_( 0 ),
  arguments_( arguments ),
  realized_( false )
{ 
  
  Debug::Throw( "BaseCoreApplication::BaseCoreApplication.\n" ); 
  connect( this, SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );

}

//____________________________________________
BaseCoreApplication::~BaseCoreApplication( void )
{ 

  Debug::Throw( "BaseCoreApplication::~BaseCoreApplication.\n" );
  emit saveConfiguration();
  XmlOptions::write();

  if( _hasApplicationManager() ) 
  {
    delete application_manager_; 
    application_manager_ = 0;
  }

  ErrorHandler::get().exit();  
  Debug::Throw( "BaseCoreApplication::~BaseCoreApplication - done.\n" );
  
}

//____________________________________________
void BaseCoreApplication::initApplicationManager( void )
{
  Debug::Throw( "BaseCoreApplication::initApplicationManager.\n" ); 

  if( _hasApplicationManager() ) return;

  if( ApplicationManager::commandLineParser( _arguments() ).hasFlag( "--no-server" ) ) 
  {
    realizeWidget();
    return;
  }
  
  // create application manager
  application_manager_ = new ApplicationManager( this );
  application_manager_->setApplicationName( XmlOptions::get().raw( "APP_NAME" ) );
  
  // connections
  connect( 
    application_manager_, SIGNAL( stateChanged( SERVER::ApplicationManager::State ) ),
    SLOT( _stateChanged( SERVER::ApplicationManager::State ) ) );
    
  connect( 
    application_manager_, SIGNAL( serverRequest( const CommandLineArguments& ) ), 
    SLOT( _processRequest( const CommandLineArguments& ) ) );
    
  // initialization
  application_manager_->initialize( arguments_ );
  Debug::Throw( "BaseCoreApplication::initApplicationManager - done.\n" ); 
  
}

//____________________________________________
bool BaseCoreApplication::realizeWidget( void )
{
  Debug::Throw( "BaseCoreApplication::realizeWidget.\n" );
   
  //! check if the method has already been called.
  if( realized_ ) return false;  
  realized_ = true;
  return true;
  
}


//_______________________________________________
void BaseCoreApplication::_updateConfiguration( void )
{
  Debug::Throw( "BaseCoreApplication::_updateConfiguration.\n" );
    
  // debug
  Debug::setLevel( XmlOptions::get().get<int>( "DEBUG_LEVEL" ) );

  // emit signal to propagate changes to other widgets
  Debug::Throw( "BaseCoreApplication::_updateConfiguration - done.\n" );
  
}

//________________________________________________
void BaseCoreApplication::_stateChanged( ApplicationManager::State state )
{

  Debug::Throw() << "BaseCoreApplication::_stateChanged - state=" << state << endl;

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
