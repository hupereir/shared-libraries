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
 * ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or            
 * FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License            
 * for more details.                        
 *                             
 * You should have received a copy of the GNU General Public License along with 
 * software; if not, write to the Free Software Foundation, Inc., 59 Temple       
 * Place, Suite 330, Boston, MA   02111-1307 USA                                      
 *                            
 *                            
 *******************************************************************************/

/*!
   \file FlatStyle.cc
   \brief Customized style based on plastique with less frames 
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include <QPainter>
#include <QStyleOption>
#include <QToolBar>
#include <QMainWindow>

#include "Debug.h"
#include "FlatStyle.h"

//________________________________________________________________
int FlatStyle::pixelMetric( PixelMetric metric, const QStyleOption * option, const QWidget * widget ) const
{
  // no extra margin for menu bars
  switch (metric)
  {
    case PM_MenuBarPanelWidth: return 2;
    case PM_ToolBarHandleExtent: return 7;
    default: return QPlastiqueStyle::pixelMetric( metric, option, widget );
  }
  
  
}

//________________________________________________________________
void FlatStyle::drawPrimitive( PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget ) const
{ 
  switch( element )
  {
    case PE_PanelMenuBar:
    {
      painter->setPen( option->palette.window().color() );
      painter->drawLine(option->rect.left(), option->rect.bottom(), option->rect.right(), option->rect.bottom());
      painter->drawLine(option->rect.left(), option->rect.top(), option->rect.right(), option->rect.top());
    }
   return;
    
   case PE_IndicatorToolBarHandle: return;
    
    default: return QPlastiqueStyle::drawPrimitive( element, option, painter, widget ); 
  }
}

//________________________________________________________________
void FlatStyle::drawControl( ControlElement element, const QStyleOption *option, QPainter* painter, const QWidget* widget ) const
{
  switch( element )
  {
    
    case CE_MenuBarEmptyArea:
      if (widget && qobject_cast<const QMainWindow *>(widget->parentWidget())) 
      { 
        painter->fillRect(option->rect, option->palette.window()); 
        QPen oldPen = painter->pen();
        painter->setPen(QPen(option->palette.window().color()));
        painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
        painter->setPen(oldPen);
      }
      return;
   
    case CE_ToolBar: return;
      
      
    default: return QPlastiqueStyle::drawControl( element, option, painter, widget ); 
  }
}
