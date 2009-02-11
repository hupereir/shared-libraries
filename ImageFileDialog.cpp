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
  \file ImageFileDialog.cpp
  \brief customized file selection dialog, to store last selected directory
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

using namespace std;

#include <QFileInfo>
#include <QImage>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QSplitter>
#include <QUrl>

#include "CustomPixmap.h"
#include "ImageFileDialog.h"
#include "FileDialog.h"
#include "File.h"  
#include "Debug.h"  
#include "Util.h"  

//______________________________________________________________________
ImageFileDialog::ImageFileDialog( QWidget* parent ):
  QFileDialog( parent )
{
  Debug::Throw( "ImageFileDialog::ImageFileDialog.\n" );
 
  // working directory
  if( !FileDialog::_workingDirectory().isEmpty() && QFileInfo( FileDialog::_workingDirectory() ).isDir() )
  { setDirectory( QDir( FileDialog::_workingDirectory() ) ); }
 
  connect( this, SIGNAL( currentChanged( const QString& ) ), SLOT( _saveWorkingDirectory( const QString& ) ) );
  
  // add image display
  QSplitter* splitter = findChild<QSplitter*>( "splitter" );
  if( splitter )
  {
    QWidget *main( new QWidget() );
    splitter->addWidget( main );
    QVBoxLayout *v_layout = new QVBoxLayout();
    main->setLayout( v_layout );
    v_layout->setSpacing(5);
    v_layout->setMargin(0);
    v_layout->addWidget( preview_ = new Label( main ), 1 );
    preview_->setAlignment( Qt::AlignCenter );
    preview_->setFrameStyle( QFrame::StyledPanel|QFrame::Sunken );
    
    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->setSpacing(5);
    h_layout->setMargin(0);
    v_layout->addLayout( h_layout );
    
    h_layout->addWidget( automatic_preview_ = new QCheckBox( "&Automatic preview", main ) );
    _automaticPreviewCheckbox().setChecked( true );
    
    QPushButton* button = new QPushButton( "&Preview", main );
    h_layout->addWidget( button );
    connect( button, SIGNAL( clicked() ), SLOT( _preview() ) );
    
  } else Debug::Throw(0) << "QFileDialog::QFileDialog - unable to find splitter." << endl;
  
  connect( this, SIGNAL( currentChanged ( const QString& ) ), SLOT( _currentChanged( const QString& ) ) );
  
}  

//______________________________________________________________________
ImageFileDialog::Label::Label( QWidget* parent ):
  QLabel( parent ),
  Counter( "ImageFileDialog::Label" )
{ setAcceptDrops( true ); }

//______________________________________________________________________
void ImageFileDialog::Label::dragEnterEvent( QDragEnterEvent *event )
{
  Debug::Throw( "ImageFileDialog::Label::dragEnterEvent.\n" );
  if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

//______________________________________________________________________
void ImageFileDialog::Label::dropEvent( QDropEvent *event )
{

  Debug::Throw( "ImageFileDialog::Label::dropEvent.\n" );
  
  // check if event is valid
  if( !event->mimeData()->hasUrls() ) return;
  
  // loop over event URLs
  QList<QUrl> urls( event->mimeData()->urls() );
  for( QList<QUrl>::const_iterator iter = urls.begin(); iter != urls.end(); iter++ )
  {
    QFileInfo file_info( iter->toLocalFile() );
    if( file_info.exists() )
    {
      ImageFileDialog& dialog( *static_cast<ImageFileDialog*>( window() ) );
      if( file_info.isDir() ) dialog.setDirectory( file_info.filePath() );
      else {
        
        dialog.setDirectory( file_info.path() );
        dialog.selectFile( file_info.fileName() );
        if( dialog._automaticPreviewCheckbox().isChecked() ) dialog._currentChanged( file_info.filePath() );
        
      }
      event->acceptProposedAction();
      return;
    }
  }
  
}

//______________________________________________________________________
void ImageFileDialog::_saveWorkingDirectory( const QString& directory )
{ 
  Debug::Throw( "ImageFileDialog::_saveWorkingDirectory.\n" );
  FileDialog::_workingDirectory() = QFileInfo( directory ).absolutePath();
}

//______________________________________________________________________
void ImageFileDialog::_currentChanged( const QString& value )
{ 
  Debug::Throw( "ImageFileDialog::_currentChanged.\n" );
  current_path_ = value;
  if( _automaticPreviewCheckbox().isChecked() ) _preview();
}

//______________________________________________________________________
void ImageFileDialog::_preview( void )
{ 

  Debug::Throw( "ImageFileDialog::_preview.\n" );
  
  CustomPixmap pixmap( current_path_ );
  if( pixmap.isNull() ) preview_->setPixmap( QPixmap() );
  else {
    
    if( pixmap.width() > preview_->width()*0.8 || pixmap.height() > preview_->height()*0.8 )
    { pixmap = pixmap.scaled( int(preview_->width()*0.8), int(preview_->height()*0.8), Qt::KeepAspectRatio, Qt::SmoothTransformation ); }
    preview_->setPixmap( pixmap );
    
  }

  Debug::Throw( "ImageFileDialog::_preview - done.\n" );
  
}
