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

#ifndef StateFrame_h
#define StateFrame_h

/*!
   \file StateFrame.h
   \brief  customized line edit for application state
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QLabel>
#include <QLayout>
#include <QApplication>
#include <string>
#include <vector>

#include "Counter.h"
#include "Exception.h"

//! local label for additional slots
class StateFrameLabel: public QLabel, public Counter
{
  
  //! Qt meta object macro
  Q_OBJECT

  public:
  StateFrameLabel( QWidget* parent ):
    QLabel( parent ),
    Counter( "StateFrameLabel" )
    {}
  
  public slots:
  
  //! set label text and process events
  void setTextAndUpdate( const QString& message )
  { 
    setText( message );
    qApp->processEvents();
  }

};
    
/*!
   \class StateFrame
   \brief  customized line edit for application state
*/
class StateFrame: public QWidget, public Counter 
{

  public:
  
  //! constructor
  StateFrame( QWidget* parent, unsigned int n_labels = 1 );
  
  //! destructor
  ~StateFrame( void )
  {}
  
  //! retrieve layout
  QBoxLayout& getLayout( void )
  { 
    Exception::assert( layout_, DESCRIPTION( "layout_ not initialized" ) ); 
    return *layout_;
  }
  
  //! retrieves label with given index
  StateFrameLabel& label( unsigned int i_label = 0  ) const
  {
    if( i_label >= labels_.size() ) throw std::runtime_error( DESCRIPTION( "invalid label index" ) );
    return *labels_[i_label];  
  }
  
  //! changes label text
  void setText( const std::string& text, bool update = true, unsigned int i_label = 0 )
  {
    Exception::assert( i_label < labels_.size(), DESCRIPTION( "invalid label index" ) );
    if( update ) labels_[i_label]->setTextAndUpdate( text.c_str() );
    else labels_[i_label]->setText( text.c_str() );
    return;
  }
  
  private:

  //! layout
  QBoxLayout* layout_;
  
  //! vector of output labels.
  std::vector< StateFrameLabel* > labels_;
  
};

#endif
