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
    //case PM_ScrollBarSliderMin: return 1024;
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
    
    case PE_FrameStatusBar: return;
    
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
    
    case CE_ScrollBarSlider:
    if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) 
    {  
      QColor borderColor = option->palette.background().color().dark(178);
      QColor gradientStartColor = option->palette.button().color().light(104);
      QColor gradientStopColor = option->palette.button().color().dark(105);
      bool isEnabled = scrollBar->state & State_Enabled;
      
      // The slider
      if (option->rect.isValid()) 
      {
        
        QPixmap cache;
        cache = QPixmap(option->rect.size());
        cache.fill(Qt::white);
        QRect pixmapRect(0, 0, cache.width(), cache.height());
        QPainter sliderPainter(&cache);
        bool sunken = (scrollBar->state & State_Sunken);
        
        if (isEnabled) 
        {
          QLinearGradient gradient(pixmapRect.center().x(), pixmapRect.top(), pixmapRect.center().x(), pixmapRect.bottom());
          if (sunken) 
          {
            gradient.setColorAt(0, gradientStartColor.light(110));
            gradient.setColorAt(1, gradientStopColor.light(105));
          } else {
            gradient.setColorAt(0, gradientStartColor.light(105));
            gradient.setColorAt(1, gradientStopColor);
          }
          sliderPainter.fillRect(pixmapRect.adjusted(2, 2, -2, -2), gradient);
        } else {
          sliderPainter.fillRect(pixmapRect.adjusted(2, 2, -2, -2), option->palette.background());
        }
        
        sliderPainter.setPen(borderColor);
        sliderPainter.drawRect(pixmapRect.adjusted(0, 0, -1, -1));
        //sliderPainter.setPen(alphaCornerColor);
        QPoint points[4] = {
          QPoint(pixmapRect.left(), pixmapRect.top()),
          QPoint(pixmapRect.left(), pixmapRect.bottom()),
          QPoint(pixmapRect.right(), pixmapRect.top()),
          QPoint(pixmapRect.right(), pixmapRect.bottom()) 
        };
        sliderPainter.drawPoints(points, 4);
        
        QLine lines[2];
        sliderPainter.setPen(sunken ? gradientStartColor.light(110) : gradientStartColor.light(105));
        lines[0] = QLine(pixmapRect.left() + 1, pixmapRect.top() + 1, pixmapRect.right() - 1, pixmapRect.top() + 1);
        lines[1] = QLine(pixmapRect.left() + 1, pixmapRect.top() + 2, pixmapRect.left() + 1, pixmapRect.bottom() - 2);
        sliderPainter.drawLines(lines, 2);
        
        sliderPainter.setPen(sunken ? gradientStopColor.light(105) : gradientStopColor);
        lines[0] = QLine(pixmapRect.left() + 1, pixmapRect.bottom() - 1, pixmapRect.right() - 1, pixmapRect.bottom() - 1);
        lines[1] = QLine(pixmapRect.right() - 1, pixmapRect.top() + 2, pixmapRect.right() - 1, pixmapRect.bottom() - 1);
        sliderPainter.drawLines(lines, 2);
        sliderPainter.end();
        
        painter->drawPixmap(option->rect.topLeft(), cache);
      }
    }
    break;

    case CE_ToolBar: return;
    case CE_Splitter: return;
      
      
    default: return QPlastiqueStyle::drawControl( element, option, painter, widget ); 
  }
}
