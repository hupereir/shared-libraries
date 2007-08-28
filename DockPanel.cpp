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
  \file DockPanel.cc
  \brief detachable generic panel
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "DockPanel.h"
#include "Debug.h"
#include "File.h"
#include "XmlOptions.h"

using namespace std;

//___________________________________________________________
DockPanel::DockPanel( QWidget* parent, const unsigned int& flags ):
    QWidget( parent ),
    Counter( "DockPanel" ),
    flags_( flags ),
    label_(0)
{
  Debug::Throw( "DockPanel::DockPanel.\n" );
  
  // this layout
  setLayout( new QVBoxLayout() );
  layout()->setMargin(0);
  layout()->setSpacing(2);
  
  // main widget
  Debug::Throw( "DocPanel::DockPanel - main_.\n" );
  main_ = new LocalWidget( this );
  layout()->addWidget( main_ );
  
  main_->setFrameStyle( QFrame::StyledPanel|QFrame::Plain );
  
  // vertical layout for children
  Debug::Throw( "DocPanel::DockPanel - main_layout.\n" );
  main_layout_ = new QVBoxLayout();
  main_layout_->setMargin( 5 );
  main_layout_->setSpacing( 5 );
  main_->setLayout( main_layout_ );
  
  // vertical panel
  Debug::Throw( "DocPanel::DockPanel - panel.\n" );
  panel_ = new QWidget( main_ );
  panel_->setLayout( new QVBoxLayout() );
  panel_->layout()->setMargin(0);
  panel_->layout()->setSpacing(2);
  
  main_layout_->addWidget( panel_ );
  
  // insert hpanel layout for buttons
  Debug::Throw( "DocPanel::DockPanel - button_layout_.\n" );
  button_layout_ = new QHBoxLayout();
  button_layout_->setMargin(0);
  button_layout_->setSpacing( 5 );
  main_layout_->addLayout( button_layout_ );
  
  // detach button
  Debug::Throw( "DocPanel::DockPanel - button_.\n" );
  button_ = new QPushButton( "&detach", main_ );
  connect( button_, SIGNAL( clicked() ), SLOT( _toggleDock() ) );
  button_->setToolTip( "dock/undock panel" );
  button_layout_->addWidget( button_ );
  
}

//___________________________________________________________
void DockPanel::_toggleDock( void )
{
  
  Debug::Throw( "DockPanel::_toggleDock.\n" );

  if( !main_->parent() ) 
  {
  
    detached_size_ = main_->size();
    main_->setParent( this );
    layout()->addWidget( main_ );
    main_->setFrameStyle( QFrame::StyledPanel|QFrame::Plain );
    main_->show();    
    
    button_->setText("&detach");
    
    emit attached( true );
    emit attached();
  
  } else {
    
    main_->setParent( 0 );
    if( flags_ & STAYS_ON_TOP ) main_->setWindowFlags( Qt::WindowStaysOnTopHint );
    main_->move( mapToGlobal( QPoint(0,0) ) );
    main_->setWindowIcon( QPixmap(File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str() ) );
    
    button_->setText("&attach");
    if( detached_size_ != QSize() ) main_->resize( detached_size_ );    
    
    main_->setFrameStyle( QFrame::NoFrame );
    main_->show();
    
    emit attached( false );
    emit detached();
  
  }  
  
}
