#ifndef StatusBar_h
#define StatusBar_h

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
   \file StatusBar.h
   \brief status bar
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <assert.h>
#include <QApplication>
#include <QContextMenuEvent>
#include <QStatusBar>

#include <vector>

#include "AnimatedLabel.h"
#include "Counter.h"

//! local label for additional slots
class StatusBarLabel: public AnimatedLabel
{
  
  //! Qt meta object macro
  Q_OBJECT

  public:
  StatusBarLabel( QWidget* parent = 0 ):
    AnimatedLabel( parent )
  {}
    
  public slots:
  
  //! set text
  virtual void setText( const QString& message, const bool& value = true )
  {
    if( value ) setTextAndUpdate( message );
    else AnimatedLabel::setText( message );
  }
  
  //! set label text and process events
  void setTextAndUpdate( const QString& );

};
  
/*!
   \class StatusBar
   \brief  customized line edit for application state
*/
class StatusBar: public QStatusBar, public Counter 
{

  public:
  
  //! constructor
  StatusBar( QWidget* parent );
  
  //! destructor
  ~StatusBar( void )
  {}
     
  //! add clock
  void addClock( void );
  
  //! add label
  void addLabel( const int& stretch = 0, bool animated = false );
  
  //! add labels
  void addLabels( const unsigned int& n, const int& stretch = 0, bool animated = false )
  { for( unsigned int i=0; i<n; i++ ) addLabel( stretch, animated ); }
  
  //! retrieves label with given index
  virtual StatusBarLabel& label( const unsigned int& i = 0  ) const
  {
    assert( i < labels_.size() );
    return *labels_[i];  
  }
  
  protected:
  
  //! context menu
  virtual void contextMenuEvent( QContextMenuEvent* );
  
  private:

  //! vector of output labels.
  std::vector< StatusBarLabel* > labels_;
  
};

#endif
