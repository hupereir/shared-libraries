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
 * ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or            
 * FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License            
 * for more details.                        
 *                             
 * You should have received a copy of the GNU General Public License along with 
 * software; if not, write to the Free Software Foundation, Inc., 59 Temple       
 * Place, Suite 330, Boston, MA   02111-1307 USA                                      
 *                            
 *                            
 *******************************************************************************/
/*!
   \file SystemEnvironmentDialog.cpp
   \brief displays system environment
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPushButton>
#include <QProcess>
#include <QLayout>

#include "SystemEnvironmentDialog.h"
#include "TreeView.h"
#include "Str.h"

using namespace std;

//__________________________________________________________________________
SystemEnvironmentDialog::SystemEnvironmentDialog( QWidget* parent ):
  CustomDialog( parent, CustomDialog::OK_BUTTON )
{
  
  Debug::Throw( "SystemEnvironmentDialog::SystemEnvironmentDialog.\n" );
  setWindowTitle( "object counters" );
  _setSizeOptionName( "SYSTEM_ENVIRONMENT_DIALOG" );

  // tell dialog to delete when close
  setAttribute( Qt::WA_DeleteOnClose );
  
  setLayout( new QVBoxLayout() );
  layout()->setMargin(10);
  layout()->setSpacing(10);
  
  // insert list
  TreeView* list = new TreeView( this );
  layout()->addWidget( list );
  list->setModel( &model_ );
  
  // retrieve environment variables from QProcess
  QStringList env( QProcess::systemEnvironment() );
  OptionModel::List options;
  for( QStringList::iterator iter = env.begin(); iter != env.end(); iter++ )
  {
    QStringList parsed( (iter)->split( "=" ) );
    if( parsed.empty() ) continue;
    
    options.push_back( Option( qPrintable( parsed[0] ), parsed.size() > 1 ? qPrintable( parsed[1] ): "" ) );
  
  }
  
  model_.add( options );
  list->resizeColumns();
  
}
