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

#ifndef StatusBar_h
#define StatusBar_h

/*!
   \file StatusBar.h
   \brief  customized line edit for application state
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QSizeGrip>
#include <QLabel>
#include <QLayout>
#include <QApplication>
#include <string>
#include <vector>

#include "Counter.h"
#include "Exception.h"

//! local label for additional slots
class StatusBarLabel: public QLabel, public Counter
{
  
  //! Qt meta object macro
  Q_OBJECT

  public:
  StatusBarLabel( QWidget* parent ):
    QLabel( parent ),
    Counter( "StatusBarLabel" )
  {}
  
  //! set text
  virtual void setText( const QString& message, const bool& value = true )
  {
    if( value ) setTextAndUpdate( message );
    else QLabel::setText( message );
  }
    
  public slots:
  
  //! set label text and process events
  void setTextAndUpdate( const QString& message )
  { 
    QLabel::setText( message );
    qApp->processEvents();
  }

};
    
/*!
   \class StatusBar
   \brief  customized line edit for application state
*/
class StatusBar: public QWidget, public Counter 
{

  public:
  
  //! constructor
  StatusBar( QWidget* parent );
  
  //! destructor
  ~StatusBar( void )
  {}
  
  //! size/hide grip
  void setSizeGripEnabled( const bool& value )
  { 
    if( value ) grip().show();
    else grip().hide();
  }
  
  //! size grip
  QSizeGrip& grip( void )
  { 
    Exception::checkPointer( grip_, DESCRIPTION( "grip_ not initialized" ) );
    return *grip_;
  }
  
  //! retrieve layout
  QBoxLayout& getLayout( void )
  { 
    Exception::checkPointer( layout_, DESCRIPTION( "layout_ not initialized" ) ); 
    return *layout_;
  }
  
  //! add clock
  void addClock( void );
  
  //! add label
  void addLabel( const int& stretch = 0 );
  
  //! add labels
  void addLabels( const unsigned int& n, const int& stretch = 0 )
  { for( unsigned int i=0; i<n; i++ ) addLabel( stretch ); }
  
  //! retrieves label with given index
  StatusBarLabel& label( const unsigned int& i = 0  )
  {
    Exception::check( i < labels_.size(), DESCRIPTION( "invalid index" ) );
    return *labels_[i];  
  }
  
  private:

  //! layout
  QBoxLayout* layout_;
  
  //! size grip
  QSizeGrip* grip_;
  
  //! vector of output labels.
  std::vector< StatusBarLabel* > labels_;
  
};

#endif
