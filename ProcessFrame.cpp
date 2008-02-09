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
  \file ProcessFrame.cpp  
  \brief parse input file and display includes recursively 
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QCheckBox>
#include <QPushButton>

#include "Debug.h"
#include "ProcessFrame.h"
#include "TextFormat.h"
#include "QtUtil.h"

using namespace std;

//_____________________________________________________________
ProcessFrame::ProcessFrame( QWidget* parent ):
  QWidget( parent ),
  Counter( "ProcessFrame" ),
  suspended_( false ),
  process_( this )
{
  Debug::Throw("ProcessFrame::ProcessFrame.\n" );

  // main layout
  main_layout_ = new QVBoxLayout();
  main_layout_->setSpacing( 10 );
  main_layout_->setMargin( 10 );
  setLayout( main_layout_ );

  main_layout_->addWidget( editor_ = new CustomTextEdit( this ) );
  editor_->setReadOnly( true );
  editor_->setLineWrapMode( QTextEdit::NoWrap );

  // button layout
  button_layout_ = new QHBoxLayout();
  button_layout_->setSpacing( 10 );
  button_layout_->setMargin( 0 );
  main_layout_->addLayout( button_layout_ );
  
  button_layout_->addStretch( 1 );
  
  // suspend checkbox
  QCheckBox *checkbox = new QCheckBox( "&Suspend", this );
  connect( checkbox, SIGNAL( toggled( bool ) ), SLOT( _toggleUpdate( bool ) ) );
  checkbox->setToolTip( "toggle auto update" );
  button_layout_->addWidget( checkbox );
  
  // clear button
  QPushButton *button = new QPushButton( "&Clear", this );
  button->setAutoDefault( false );
  connect( button, SIGNAL( clicked( void ) ), SLOT( clear( void ) ) );
  checkbox->setToolTip( "clear" );
  button_layout_->addWidget( button );
  
  // connections
  connect( &process_, SIGNAL( readyRead() ), SLOT( _read() ) );
  connect( &process_, SIGNAL( error( QProcess::ProcessError ) ), SLOT( _error( QProcess::ProcessError ) ) );
  connect( &process_, SIGNAL( finished( int, QProcess::ExitStatus ) ), SLOT( _completed( int, QProcess::ExitStatus ) ) );
  
}

//_____________________________________________________________
ProcessFrame::~ProcessFrame( void )
{
  
  if( process_.state() != QProcess::NotRunning ) 
  {
    process_.terminate();
    process_.waitForFinished();
  }
  
}

//_____________________________________________________________
bool ProcessFrame::start( const std::string& arguments, QProcess::OpenMode mode )
{
  Debug::Throw( "ProcessFrame::start.\n" );
  
  // check if last time process is not still running
  if( process_.state() != QProcess::NotRunning ) return false; 
  
  append( (arguments + "\n" ).c_str(), FORMAT::BOLD );
  process().start( arguments, mode );
  return true;
  
}

//_____________________________________________________________
void ProcessFrame::append( const QString& text, const unsigned int& format )
{  if( suspended_ ) buffer_.push_back( make_pair( text, format ) );
  else _append( text, format );
  
}
  
//_____________________________________________________________
void ProcessFrame::clear( void )
{ 
  Debug::Throw( "ProcessFrame::clear.\n" );
  editor_->clear(); 
  buffer_.clear();
}

//_____________________________________________________________
void ProcessFrame::_toggleUpdate( bool checked )
{ 
  Debug::Throw( "ProcessFrame::_toggleUpdate.\n" );
  suspended_ = checked;
  if( !suspended_ ) _flush();
}

//_____________________________________________________________
void ProcessFrame::_read( void )
{
  
  if( process_.bytesAvailable() < 0 ) return;
  char* data = new char[process_.bytesAvailable()];
  qint64 size = process_.read( data, process_.bytesAvailable() );
  if( size <= 0 ) return;
  append( QString( data ).left( size ) ); 

}

//_______________________________________________
void ProcessFrame::_error( QProcess::ProcessError error )
{
  Debug::Throw( "ProcessFrame::_error.\n" );
  ostringstream what;
  what << endl << CustomProcess::errorMessage( error ) << endl;
  append( what.str().c_str(), FORMAT::BOLD );
}

//_______________________________________________
void ProcessFrame::_completed( int code, QProcess::ExitStatus status )
{
  Debug::Throw( "ProcessFrame::_completed.\n" );
  ostringstream what;
  what << endl << "process exited with code " << code << endl;
  append( what.str().c_str(), FORMAT::BOLD );
}

//_____________________________________________________________
void ProcessFrame::_flush( void )
{
  for( Buffer::iterator iter = buffer_.begin(); iter != buffer_.end(); iter++ )
  { _append( iter->first, iter->second ); }
  
  buffer_.clear();
  
}
  
//_____________________________________________________________
void ProcessFrame::_append( const QString& text, const unsigned int& format )
{
 
  // retrieve current cursor
  QTextCursor cursor( editor_->textCursor() );
  
  // move to the end of the text
  cursor.movePosition( QTextCursor::End );

  QTextCharFormat char_format;
  if( format ) 
  {
    char_format.setFontWeight( (format&FORMAT::BOLD) ? QFont::Bold : QFont::Normal );
    char_format.setFontItalic( format&FORMAT::ITALIC );
    char_format.setFontUnderline( format&FORMAT::UNDERLINE );
    char_format.setFontStrikeOut( format&FORMAT::STRIKE );        
  }
  cursor.setCharFormat( char_format );
  cursor.insertText( text );  
  return;
  
}
