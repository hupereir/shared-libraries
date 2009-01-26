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
   \file    ServerConfiguration.cpp
   \brief   transparency common configuration
   \version $Revision$
   \date    $Date$
*/

#include <QGroupBox>
#include <QLabel>
#include <QLayout>

#include "GridLayout.h"
#include "Debug.h"
#include "OptionLineEditor.h"
#include "OptionSpinBox.h"
#include "ServerConfiguration.h"

using namespace std;
using namespace SERVER;

//________________________________________________________________________
ServerConfiguration::ServerConfiguration( QWidget* parent, QString title ):
  QWidget( parent ),
  Counter( "ServerConfiguration" )
{
  Debug::Throw( "ServerConfiguration::ServerConfiguration.\n" );
  
  setLayout( new QVBoxLayout() );
  layout()->setSpacing(5);
  layout()->setMargin(0);
   
  // generic objects
  QGroupBox *box;
  OptionLineEditor* line_editor;
  OptionSpinBox* spin_box;
  
  // general
  if( title.isEmpty() ) layout()->addWidget( box = new QGroupBox( this ) );  
  else layout()->addWidget( box = new QGroupBox( title, this ) );  
    
  GridLayout* grid_layout = new GridLayout();
  grid_layout->setSpacing(5);
  grid_layout->setMargin(5);
  grid_layout->setMaxCount( 2 );
  box->setLayout( grid_layout );

  QString tooltip( "The application server configuration refers to the host name\n"
    "and port used for inter-process communication. It notably ensures\n"
    "that only one instance of each application runs at a time." );
  
  // host
  grid_layout->addWidget( new QLabel( "Host:", box ) );
  grid_layout->addWidget( line_editor = new OptionLineEditor( box, "SERVER_HOST" ) );
  line_editor->setToolTip( tooltip );
  addOptionWidget( line_editor );
 
  // shadow color
  grid_layout->addWidget( new QLabel( "Port:", box ) );
  grid_layout->addWidget( spin_box = new OptionSpinBox( box, "SERVER_PORT" ) );
  spin_box->setMinimum(0);
  spin_box->setMaximum(10000);
  spin_box->setToolTip( tooltip );
  addOptionWidget( spin_box ); 
  
  return;
}
