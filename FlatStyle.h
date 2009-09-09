#ifndef FlatStyle_h
#define FlatStyle_h

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
   \file FlatStyle.h
   \brief Customized style based on plastique with less frames
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Counter.h"
#include <QPlastiqueStyle>

//! Customized style based on plastique with less frames
class FlatStyle : public QPlastiqueStyle, public Counter
{

  public:

  //! constructor
  FlatStyle( void ):
    Counter( "FlatStyle" )
    {}

  //! pixel metric
  virtual int pixelMetric (
    PixelMetric metric,
    const QStyleOption * option = 0,
    const QWidget * widget = 0 ) const;

  //! primitive
  virtual void drawPrimitive(
    PrimitiveElement elem,
    const QStyleOption* option,
    QPainter* painter,
    const QWidget* widget = 0 ) const;

  //! control
  virtual void drawControl(
    ControlElement element,
    const QStyleOption *option,
    QPainter* painter,
    const QWidget* widget = 0 ) const;

};

#endif
