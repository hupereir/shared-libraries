#ifndef SvgRenderer_h
#define SvgRenderer_h

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
  \file SvgRenderer.h
  \brief construct pixmap of given size using Svg renderer
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/

#include <QSvgRenderer>
#include <QPaintDevice>

#include "Counter.h"

//! construct pixmap of given size using Svg renderer
namespace SVG
{
  class SvgRenderer: public QSvgRenderer
  {

    public:

    //! constructor
    SvgRenderer( void ):
      QSvgRenderer()
    {}

    //! destructor
    virtual ~SvgRenderer( void )
    {}

    //! render
    void render( QPaintDevice&, const double& offset = 0, const QString& id = QString() );

    //! validity
    virtual bool isValid( void ) const;

  };

};
#endif
