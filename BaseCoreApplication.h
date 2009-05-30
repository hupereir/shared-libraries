#ifndef BaseCoreApplication_h
#define BaseCoreApplication_h

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
  \file BaseCoreApplication.h
  \brief application main object
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QObject>

#include "ApplicationManager.h"
#include "CommandLineArguments.h"
#include "Counter.h"


//! Main Window singleton object
class BaseCoreApplication: public QObject
{

  //! Qt meta object declaration
  Q_OBJECT

  public:
    
  //! constructor
  BaseCoreApplication( QObject* parent, CommandLineArguments arguments = CommandLineArguments() ); 
    
  //! destructor
  virtual ~BaseCoreApplication( void );
  
  //! initialize application manager
  virtual void initApplicationManager( void );
  
  //! create all widgets
  virtual bool realizeWidget( void );
    
  signals:
  
  //! emmited when configuration needs to be saved
  void saveConfiguration( void );
  
  //! emmited when configuration is changed
  void configurationChanged( void );
  
  protected slots:

  //! process command from server
  /*! returns true if command has been accepted */
  virtual bool _processCommand( SERVER::ServerCommand );
    
  private slots:
  
  //! configuration
  void _updateConfiguration( void );
      
  protected:
    
  //! argument list
  void _setArguments( CommandLineArguments arguments )
  { arguments_ = arguments; }
  
  //! argument list
  CommandLineArguments& _arguments( void )
  { return arguments_; }
  
  //! argument list
  const CommandLineArguments& _arguments( void ) const
  { return arguments_; }
  
  //! realized
  bool _realized( void ) const
  { return realized_; }
  
  //! application manager
  bool _hasApplicationManager( void ) const
  { return (bool) application_manager_; }
  
  //! application manager
  SERVER::ApplicationManager& _applicationManager( void ) const
  { 
    assert( application_manager_ );
    return *application_manager_;
  }
  
  private:
     
  //! pointer to application manager
  SERVER::ApplicationManager* application_manager_;
   
  //! command line arguments
  CommandLineArguments arguments_;
  
  //! true when Realized Widget has been called.
  bool realized_; 

};

#endif  
