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
   \file StateFrame.cc
   \brief  customized line edit for application state
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <qlayout.h>

#include "StateFrame.h"
#include "CustomLineEdit.h"
#include "QtUtil.h"
#include "Debug.h"

using namespace std;

//____________________________________________
StateFrame::StateFrame( QWidget* parent, unsigned int n_labels ):
  QWidget( parent ),
  Counter( "StateFrame" )
{
  Debug::Throw( "StateFrame::StateFrame.\n" );
  if( !n_labels ) throw runtime_error( DESCRIPTION( "invalid number of labels" ) );
  layout_ = new QHBoxLayout( this );
  layout_->setMargin( 2 );
  layout_->setSpacing( 10 );
  
  for( unsigned int i=0; i<n_labels; i++ ) {
    StateFrameLabel* label = new StateFrameLabel( this );
    label->setMargin(2);
    layout_->addWidget( label, 0, Qt::AlignVCenter );
    labels_.push_back( label );
    setText( "", false, i );
  }
  
}
