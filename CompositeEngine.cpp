// $Id$

/***************************************************************************
 *   Copyright (C) 2006 by Helio Chissini de Castro   *
 *   helio@mandriva.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*!
  \file    CompositeEngine.h
  \brief   initialize compositing, if available
  \author  Hugo Pereira
  \version $Revision$
  \date    $Date$
*/

#include "CompositeEngine.h"
#include "Debug.h"

using namespace std;
using namespace TRANSPARENCY;

//_______________________________________________________________
CompositeEngine& CompositeEngine::get( void )
{
  static CompositeEngine singleton;
  return singleton;
}

//_______________________________________________________________
CompositeEngine::CompositeEngine( void ):
  valid_( false ),
  initialized_( false ),
  display_( 0 ),
  visual_( 0 ),
  colormap_( 0 )
{ Debug::Throw( "CompositeEngine::CompositeEngine.\n" ); }


//_______________________________________________________________
void CompositeEngine::initialize( void )
{
  
  if( initialized_ ) return;
  initialized_ = true;
  
  // display
  display_ = XOpenDisplay(0);
  
  int screen = DefaultScreen(display_);  
  int n_visuals;
  
  XVisualInfo templ;
  templ.screen  = screen;
  templ.depth   = 32;
  templ.c_class = TrueColor;
  XVisualInfo *visual_info = XGetVisualInfo(display_, VisualScreenMask | VisualDepthMask | VisualClassMask, &templ, &n_visuals);    
      
  for (int i = 0; i < n_visuals; ++i)
  {
    XRenderPictFormat *format = XRenderFindVisualFormat(display_, visual_info[i].visual);
    if (format->type == PictTypeDirect && format->direct.alphaMask)
    {
      visual_ = visual_info[i].visual;
      colormap_ = XCreateColormap(display_, RootWindow(display_, screen), visual_, AllocNone);
      valid_ = true;
      break;
    }
    
  }
  
}
