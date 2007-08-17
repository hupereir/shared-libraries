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
  \file ProcessDialog.cc
  \brief customized dialog to handle QProcess objects and throw output in the text
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLayout>
#include <QPushButton>

#include "ProcessDialog.h"
#include "QtUtil.h"
#include "Debug.h"

using namespace std;

//________________________________________
ProcessDialog::ProcessDialog( QWidget* parent ):
  QDialog( parent ),
  Counter( "ProcessDialog" )
{
  Debug::Throw( "ProcessDialog::ProcessDialog.\n" );

  // tell frame to delete on exit
  setAttribute( Qt::WA_DeleteOnClose );
  
  // create layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin(0);
  layout()->setSpacing(0);
  layout()->addWidget( process_frame_ = new ProcessFrame( this ) );
  
  // close button
  QPushButton *button( new QPushButton( "&Close", &processFrame() ) );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  processFrame().buttonLayout().addWidget( button );
  
}
