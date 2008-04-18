
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
  \file TabWidget.cpp
  \brief Tab widget with detachable pages
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QGridLayout>

#include "TabWidget.h"
#include "File.h"
#include "XmlOptions.h"

using namespace std;

//________________________________________________________
TabWidget::TabWidget( QTabWidget* parent, const unsigned int& flags ):
    QFrame(), 
    Counter( "TabWidget" ),
    flags_( flags ),
    parent_( parent ),
    index_( 0 )
{

  Debug::Throw( "TabWidget::TabWidget.\n" );
  setFrameStyle( QFrame::NoFrame );

  // grid layout to overlay main layout and invisible grip
  QGridLayout *grid_layout( new QGridLayout() );
  grid_layout->setMargin(0);
  grid_layout->setSpacing(0);
  setLayout( grid_layout );

  grid_layout->addLayout( main_layout_ = new QVBoxLayout(), 0, 0, 1, 1 );
  main_layout_->setMargin(2);
  main_layout_->setSpacing(2);
  
  // vertical box
  box_ = new QWidget( this );
  box().setLayout( new QVBoxLayout() );
  box().layout()->setSpacing( 2 );
  box().layout()->setMargin( 0 );
  
  main_layout_->addWidget( box_ );
  
  // insert hbox layout for buttons
  button_layout_ = new QHBoxLayout();  
  button_layout_->setMargin( 0 );  
  button_layout_->setSpacing( 5 );  
  main_layout_->addLayout( button_layout_ );
  
  button_ = new QPushButton( "&detach", this );
  button_layout_->addWidget( button_ );
  connect( button_, SIGNAL( clicked() ), SLOT( _toggleDock() ) );
  button_->setToolTip( "dock/undock tab" );
 
  // size grip
  grid_layout->addWidget( size_grip_ = new LocalGrip( this ), 0, 0, 1, 1, Qt::AlignBottom|Qt::AlignRight );
  size_grip_->hide(); 
  
}

//___________________________________________________________
void TabWidget::_toggleDock( void )
{
  
  Debug::Throw( "TabWidget::_toggleDock.\n" );

  if( !parent() ) {
    
    // store size for later detach
    detached_size_ = size();
    
    // reinsert into parent and select
    parent_->QTabWidget::insertTab( index_, this, title_.c_str() );
    parent_->QTabWidget::setCurrentWidget( this );
    
    // modify button text
    size_grip_->hide();
    button_->setText("&detach");
        
    emit attached();
    
  } else {
    
    // keep track of index, for later re-insertion
    // and remove from parent TabWidget
    index_ = parent_->indexOf( this );
    parent_->removeTab( index_ );
    
    // keep track of parent
    QWidget *parent( parentWidget() );
    
    // reparent to top level
    setParent( 0 );
    if( flags_ & STAYS_ON_TOP ) setWindowFlags( Qt::WindowStaysOnTopHint );
   
    // change title
    // setWindowIcon( QPixmap( File( XmlOptions::get().raw( "ICON_PIXMAP" ) ).expand().c_str()  ) );
    if( !title_.empty() ) 
    {
      ostringstream what;
      what << "TexTool - " << title_;
      setWindowTitle( what.str().c_str() );
    }
    
    // modify button, move, and resize
    button_->setText("&attach");
    move( parent->mapToGlobal( QPoint(0,0) ) );    
    if( detached_size_ != QSize() ) resize( detached_size_ );
    size_grip_->show();
    show();
    
    emit detached();
  }  
  
}
