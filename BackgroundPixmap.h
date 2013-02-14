#ifndef _BackgroundPixmap_h_
#define _BackgroundPixmap_h_

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
   \file    BackgroundPixmap.h
   \brief   stores/manipulates the X11 background Pixmap
   \author  Hugo Pereira
   \version $Revision$
   \date    $Date$
*/

#include <QObject>
#include <QPixmap>

//! stores/manipulates the X11 background Pixmap
namespace TRANSPARENCY
{

  //! stores/manipulates the X11 background Pixmap
  class BackgroundPixmap: public QObject
  {

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! returns singleton
    static BackgroundPixmap& get( void );

    //! returns true if background image is valid
    bool isValid( void ) const
    { return !backgroundPixmap_.isNull(); }

    //! returns a Pixmap of given position and size from the background image
    QPixmap pixmap( const QRect& rect ) const;

    //! returns a Pixmap of given position and size from the background image
    QPixmap fullPixmap() const
    { return backgroundPixmap_; }

    #if defined(Q_WS_X11)
    //! check X11 event
    bool x11Event( XEvent* event );
    #endif

    public slots:

    //! loads the background image internaly
    void reload( void );

    signals:

    //! emited when background is changed
    void backgroundChanged( void );

    private:

    //! creator
    BackgroundPixmap( void );

    //! try load the desktopwindow tp which arg widget belongs
    bool _loadDesktopWindow( void );

    //! full background image
    QPixmap backgroundPixmap_;

    #if defined(Q_WS_X11)

    //! desktop window id
    WId desktop_;

    #endif


  };

};
#endif
