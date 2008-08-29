#ifndef BaseApplication_h
#define BaseApplication_h

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
  \file BaseApplication.h
  \brief Main Window singleton object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QAction>
#include <QApplication>
#include <QCursor>
#include <string>

#include "ApplicationManager.h"
#include "ArgList.h"
#include "Counter.h"

class MainWindow;

//! Main Window singleton object
class BaseApplication: public QApplication
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
  
  //! constructor
  BaseApplication( int argc, char*argv[] ); 
   
  //! destructor
  virtual ~BaseApplication( void );
  
  //! initialize application manager
  virtual void initApplicationManager( void );
  
  //! create all widgets
  virtual bool realizeWidget( void );
          
  //! set application busy
  virtual void busy( void ) 
  {
    setOverrideCursor( Qt::WaitCursor ); 
    processEvents(); 
  }
  
  //! set application idle
  virtual void idle( void )
  { restoreOverrideCursor(); }
  
  //!@name actions
  //@{
    
  //! about
  QAction& aboutAction( void ) const
  { return *about_action_; }

  //! about
  QAction& aboutQtAction( void ) const
  { return *aboutqt_action_; }
  
  //! configuration
  QAction& configurationAction( void ) const
  { return *configuration_action_; }
  
  //! exit safely
  QAction& closeAction( void ) const
  { return *close_action_; }
    
  //@}
  
  signals:
  
  //! emmited when configuration needs to be saved
  void saveConfiguration( void );
  
  //! emmited when configuration is changed
  void configurationChanged( void );
  
  protected slots:
 
  //! application manager state is changed
  virtual void _stateChanged( SERVER::ApplicationManager::State );
    
  //! process request from application manager
  virtual void _about( void ) = 0;

  //! process request from application manager
  virtual void _processRequest( const ArgList& ) = 0;
  
  //! configuration
  virtual void _configuration( void ) = 0;

  private slots:
  
  //! configuration
  virtual void _updateConfiguration( void );
      
  protected:
  
  //! application 'about' dialog
  virtual void _about( std::string, std::string, std::string );
  
  //! argument list
  ArgList& _arguments( void )
  { return args_; }
  
  //! argument list
  const ArgList& _arguments( void ) const
  { return args_; }
  
  private:
   
  //! pointer to application manager
  SERVER::ApplicationManager* application_manager_;
   
  //! command line arguments
  ArgList args_;
  
  //! true when Realized Widget has been called.
  bool realized_; 

  //!@name actions
  //@{
  
  //! about action
  QAction* about_action_;
  
  //! about qt
  QAction* aboutqt_action_;
  
  //! configure
  QAction* configuration_action_;
  
  //! close
  QAction* close_action_;
  
  //@}

};

#endif  
