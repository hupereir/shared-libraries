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
  \file FontEditor.cpp
  \brief three line editors and a browser to select fonts
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QHBoxLayout>
#include <QToolButton>

#include "BaseIcons.h"
#include "FontEditor.h"
#include "IconEngine.h"
#include "QtUtil.h"

using namespace std;

//______________________________________________
FontEditor::FontEditor( QWidget *parent ):
  QWidget( parent ),
  Counter( "FontEditor" )
{ 
  
  QHBoxLayout *layout( new QHBoxLayout() );
  layout->setMargin(0);
  layout->setSpacing(2);
  setLayout( layout );

  label_ =  new LineEditor( this );
  label_->setReadOnly( true );
  label_->setHasClearButton( false );
  label_->setAlignment( Qt::AlignCenter );
  label_->setFont( font() );
  layout->addWidget( label_, 1 );

  // update palette
  QPalette palette( label_->palette() );
  palette.setBrush( QPalette::Base, palette.brush( QPalette::Window ) );
  label_->setPalette( palette );
  
  // create push_button
  QToolButton *button( new QToolButton( this ) );
  button->setIcon( IconEngine::get( ICONS::OPEN ) );
  button->setAutoRaise( false );
  layout->addWidget( button, 0 );
  
  // connect push_button
  connect( button, SIGNAL( clicked() ), SLOT( _selectFont() ) );
  setFont( font() ); 
}
