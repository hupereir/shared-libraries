#ifndef _AnimatedTreeView_h_
#define _AnimatedTreeView_h_

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
  \file AnimatedTreeView.h
  \brief animated Tree View
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include "TreeView.h"

class TransitionWidget;

//! customized tree view
class AnimatedTreeView: public TreeView
{
  
  public:
  
  //! constructor
  AnimatedTreeView( QWidget* parent );
     
  //! destructor
  virtual ~AnimatedTreeView( void )
  {}

  //! initialize animation
  bool initializeAnimation( void );
  
  //! start animation
  bool startAnimation( void );

  protected:
  
  //! transition widget
  TransitionWidget& _transitionWidget( void ) const
  { return *transition_widget_; }
   
  private:

  //! transition widget
  TransitionWidget* transition_widget_; 
   
};

#endif
