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
  \file AnimatedTreeView.cpp
  \brief animated Tree View
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

//#include "AnimatedScrollBar.h"
#include "AnimatedTreeView.h"
#include "Debug.h"
#include "TransitionWidget.h"

using namespace std;

//________________________________________________________________________
AnimatedTreeView::AnimatedTreeView( QWidget* parent ):
  TreeView( parent ),
  transition_widget_( new TransitionWidget( this ) )
{
  Debug::Throw( "AnimatedTreeView::AnimatedTreeView.\n" );

  // transition widget
  _transitionWidget().setFlag( TransitionWidget::FROM_PARENT, false );
  _transitionWidget().hide();
  connect( &_transitionWidget().timeLine(), SIGNAL( finished() ), &_transitionWidget(), SLOT( hide() ) );
    
}

//________________________________________________________________________
bool AnimatedTreeView::initializeAnimation( void )
{

  // transition
  if( _transitionWidget().isEnabled() && QTreeView::isVisible() )
  {
    _transitionWidget().initialize();
    return true;
  } else return false;

}

//________________________________________________________________________
bool AnimatedTreeView::startAnimation( void )
{
  // transition
  if( _transitionWidget().isEnabled() && QTreeView::isVisible() ) 
  {
    _transitionWidget().start();
    return true;
  } else return false;

}
