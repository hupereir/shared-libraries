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
  \file FontEdit.cpp
  \brief three line editors and a browser to select fonts
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QHBoxLayout>
#include <QPushButton>

#include "FontEdit.h"
#include "QtUtil.h"

using namespace std;

//______________________________________________
FontEdit::FontEdit( QWidget *parent ):
  QWidget( parent ),
  Counter( "FontEdit" )
{ 
	
  QHBoxLayout *layout( new QHBoxLayout() );
  layout->setMargin(0);
  layout->setSpacing(2);
  setLayout( layout );

  label_ =  new QLabel( "", this );
	label_->setAlignment( Qt::AlignCenter );
	QtUtil::fixSize( label_, QtUtil::NONE );
	layout->addWidget( label_ );
  
	QPushButton *button( new QPushButton( "...", this ) );
  QtUtil::fixSize( button );
	layout->addWidget( button );
  
  // connect push_button
  connect( button, SIGNAL( clicked() ), SLOT( _selectFont() ) );
	setFont( font() ); 
}
