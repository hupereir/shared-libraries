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
  \file IconFileDialog.cpp
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

using namespace std;

#include <QImage>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>

#include "CustomPixmap.h"
#include "IconFileDialog.h"
#include "File.h"  
#include "Debug.h"  
#include "Util.h"  

//______________________________________________________________________
IconFileDialog::IconFileDialog( QWidget* parent ):
  CustomFileDialog( parent )
{
  Debug::Throw( "IconFileDialog::IconFileDialog.\n" );
  
  setFileMode( QFileDialog::AnyFile );
  
  QSplitter* splitter = findChild<QSplitter*>( "splitter" );
  if( splitter )
  {
    QWidget *main( new QWidget() );
    splitter->addWidget( main );
    QVBoxLayout *v_layout = new QVBoxLayout();
    main->setLayout( v_layout );
    v_layout->setSpacing(5);
    v_layout->setMargin(0);
    v_layout->addWidget( preview_ = new QLabel( main ), 1 );
    preview_->setAlignment( Qt::AlignCenter );
    preview_->setFrameStyle( QFrame::StyledPanel|QFrame::Sunken );
    
    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->setSpacing(5);
    h_layout->setMargin(0);
    v_layout->addLayout( h_layout );
    
    h_layout->addWidget( automatic_preview_ = new QCheckBox( "&Automatic preview", main ) );
    automatic_preview_->setChecked( true );
    
    QPushButton* button = new QPushButton( "&Preview", main );
    h_layout->addWidget( button );
    connect( button, SIGNAL( clicked() ), SLOT( _preview() ) );
    
  } else Debug::Throw(0) << "CustomFileDialog::CustomFileDialog - unable to find splitter." << endl;
  
  connect( this, SIGNAL( currentChanged ( const QString& ) ), SLOT( _currentChanged( const QString& ) ) );
  
}  

//______________________________________________________________________
void IconFileDialog::_currentChanged( const QString& value )
{ 
  Debug::Throw( "IconFileDialog::_currentChanged.\n" );
  current_path_ = value;
  if( automatic_preview_->isChecked() ) _preview();
}

//______________________________________________________________________
void IconFileDialog::_preview( void )
{ 

  Debug::Throw( "IconFileDialog::_preview.\n" );
  
  CustomPixmap pixmap( current_path_, true );
  if( pixmap.isNull() ) preview_->setPixmap( QPixmap() );
  else {
    
    if( pixmap.width() > preview_->width()*0.8 || pixmap.height() > preview_->height()*0.8 )
    { pixmap = pixmap.scaled( preview_->width()*0.8, preview_->height()*0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation ); }
    preview_->setPixmap( pixmap );
    
  }

  Debug::Throw( "IconFileDialog::_preview - done.\n" );
  
}
