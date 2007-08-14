#ifndef ProcessFrame_h
#define ProcessFrame_h

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
  \file ProcessFrame.h
  \brief display command output
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QWidget>
#include <QLayout>

#include <string>
#include <list>

#include "Counter.h"
#include "CustomProcess.h"
#include "CustomTextEdit.h"
#include "Exception.h"
#include "File.h"
#include "Options.h"

//! display command output
class ProcessFrame:public QWidget, public Counter
{
  

  //! Qt meta object declaration
  Q_OBJECT
  
  public:
      
  //! constructor
  ProcessFrame( QWidget* parent );

  /*! 
    \brief 
    add arguments. 
    every space separated argument is added separately
  */
  bool start( const std::string& arguments, QProcess::OpenMode mode = QProcess::ReadWrite );


  //! append line (with format)
  void append( const QString& text, const unsigned int& format = 0 );

  //! associated process
  CustomProcess& process( void )
  { return process_; }
  
  //! button layout
  QVBoxLayout &mainLayout( void ) const
  { 
    Exception::checkPointer( main_layout_, DESCRIPTION( "main_layout_ not initialized" ) );
    return *main_layout_;
  }

  //! button layout
  QHBoxLayout &buttonLayout( void ) const
  { 
    Exception::checkPointer( button_layout_, DESCRIPTION( "button_layout_ not initialized" ) );
    return *button_layout_;
  }
  
  public slots:
  
  //! clear
  void clear( void );
  
  private slots:
  
  //! toggle log frame update
  void _toggleUpdate( bool checked );
  
  //! read from process
  void _read( void );
  
  //! process exited
  void _completed( int, QProcess::ExitStatus );
  
  private:

  //! clear buffer
  void _flush( void );
  
  //! append line to Text editor
  void _append( const QString& text, const unsigned int& format );
  
  //! text editor
  CustomTextEdit* text_;
  
  //! suspend state
  bool suspended_;
  
  //! buffer line
  typedef std::pair< QString, unsigned int > BufferLine;
  
  //! buffer
  typedef std::list< BufferLine > Buffer;
  
  //! buffer
  Buffer buffer_;
  
  //! associated process
  CustomProcess process_;
  
  //! main layout, for customization
  QVBoxLayout* main_layout_;
  
  //! button layout, for customization
  QHBoxLayout* button_layout_;
  
};

#endif
