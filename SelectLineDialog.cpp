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
  \file SelectLineDialog.cpp
  \brief goto_line_number_dialog dialog for text editor widgets
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QValidator>

#include "SelectLineDialog.h"
#include "QtUtil.h"

using namespace std;

//_______________________________________________________
SelectLineDialog::SelectLineDialog( QWidget* parent, Qt::WFlags flags ):
    QDialog( parent, flags ),
    Counter( "SelectLineDialog" )
{
  
  Debug::Throw( "SelectLineDialog::SelectLineDialog.\n" );
    
  // set dialog title
  setWindowTitle( "goto line number" );
    
  // create vbox layout
  setLayout( new QVBoxLayout() );
  layout()->setSpacing(5);
  layout()->setMargin(10);
    
  // insert text editor
  QLabel *label( new QLabel( "goto line number: ", this ) );
  layout()->addWidget( label );
  QtUtil::fixSize( label );
  
  layout()->addWidget( editor_ = new LineEditor( this ) );
  connect( editor_, SIGNAL(returnPressed()), SLOT( _selectLine( void ) ) );
  connect( editor_, SIGNAL(textChanged( const QString& ) ), SLOT( _selectLine( const QString& ) ) );
  
  QIntValidator *validator = new QIntValidator( this );
  validator->setBottom(0);
  editor_->setValidator( validator );
 
  QHBoxLayout* h_layout( new QHBoxLayout() );
  h_layout->setMargin(0);
  h_layout->setSpacing( 5 );
  layout()->addItem( h_layout );
  
  // insert Find button
  QPushButton *button;
  h_layout->addWidget( button = new QPushButton( "&Ok", this ) );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), SLOT( _selectLine( void ) ) );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
      
  // insert Cancel button
  h_layout->addWidget( button = new QPushButton( "&Cancel", this ) );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked() ), SLOT( close() ) );
  setMinimumSize( QSize( 250, 100 ) );
  adjustSize();
  
}
 
