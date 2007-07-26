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
   \file StatusBar.cc
   \brief  customized line edit for application state
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <qlayout.h>

#include "StatusBar.h"
#include "ClockLabel.h"
#include "QtUtil.h"
#include "Debug.h"

using namespace std;

//___________________________________________
StatusBar::StatusBar( QWidget* parent )
{
  
  Debug::Throw( "StatusBar::StatusBar.\n" );
  QHBoxLayout* layout = new QHBoxLayout();
  layout->setMargin( 2 );
  layout->setSpacing( 10 );
  setLayout( layout );
  
  layout_ = new QHBoxLayout();
  layout_->setMargin( 0 );
  layout_->setSpacing( 10 );
  layout->addLayout( layout_, 1 );
  
  layout->addWidget( grip_ = new QSizeGrip(this), 0 );
  
}


//___________________________________________
void StatusBar::addClock( void )
{ 
  Debug::Throw( "StatusBar::addClock.\n" );
  getLayout().addWidget( new ClockLabel( this ) ); 
}

//____________________________________________
void StatusBar::addLabel( const int& stretch )
{
  Debug::Throw( "StatusBar::addLabel.\n" );
  
  StatusBarLabel* label = new StatusBarLabel( this );
  label->setMargin(2);
  layout_->addWidget( label, 0, Qt::AlignVCenter );
  labels_.push_back( label );  
}
