#ifndef X11Util_h
#define X11Util_h

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
\file    X11Util.h
\brief   some X11 specific utilities
\author  Hugo Pereira
\version $Revision$
\date    $Date$
*/

#include <QtGui/QWidget>
#include <QtCore/QHash>
#include <QtCore/QMap>

#if defined(Q_WS_X11)
#include <QtGui/QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

class X11Util
{

    public:

    //! singleton
    static X11Util& get( void );

    //! used for stickyness
    static const unsigned long ALL_DESKTOPS = ~0;

    //! Atoms enumeration
    enum Atoms {
        _NET_SUPPORTED,
        _NET_CURRENT_DESKTOP,
        _NET_WM_DESKTOP,
        _NET_WM_STATE,
        _NET_WM_STATE_STICKY,
        _NET_WM_STATE_STAYS_ON_TOP,
        _NET_WM_STATE_ABOVE,
        _NET_WM_STATE_SKIP_TASKBAR,
        _NET_WM_STATE_SKIP_PAGER,
        _NET_WM_MOVERESIZE,
        _NET_WM_CM,

        // kde specific atoms
        _KDE_NET_WM_BLUR_BEHIND_REGION
    };

    enum Direction {
        _NET_WM_MOVERESIZE_SIZE_TOPLEFT=0,
        _NET_WM_MOVERESIZE_SIZE_TOP=1,
        _NET_WM_MOVERESIZE_SIZE_TOPRIGHT=2,
        _NET_WM_MOVERESIZE_SIZE_RIGHT=3,
        _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT=4,
        _NET_WM_MOVERESIZE_SIZE_BOTTOM=5,
        _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT=6,
        _NET_WM_MOVERESIZE_SIZE_LEFT=7,
        _NET_WM_MOVERESIZE_MOVE=8,
        _NET_WM_MOVERESIZE_SIZE_KEYBOARD=9,
        _NET_WM_MOVERESIZE_MOVE_KEYBOARD=10,
    };

    //! supported atoms
    bool isSupported( const Atoms& atom );

    //! return true if property is found in NET_WM_STATE
    bool hasProperty( const QWidget&, const Atoms& );

    //! change property
    bool changeProperty( const QWidget& widget, const Atoms& atom, bool value )
    {
        return widget.isHidden()  ?
            _changeProperty( widget, atom, value ):
            _requestPropertyChange( widget, atom, value );
    }

    //! change property
    bool changeProperty( const QWidget&, const Atoms&, const unsigned char*, int size );

    //! print window state
    void printWindowState( const QWidget& );

    //! get atom carninal value
    unsigned long cardinal( const QWidget& widget, const Atoms& atom )
    { return cardinal( widget.winId(), atom ); }

    //! get atom carninal value
    unsigned long cardinal( const WId&, const Atoms& );

    //! get atom carninal value
    bool changeCardinal( const QWidget& widget, const Atoms& atom, const unsigned long& value )
    {
        return widget.isHidden() ?
            _changeCardinal( widget, atom, value ):
            _requestCardinalChange( widget, atom, value );
    }

    //! move widget using X11 window manager
    /*! returns true on success */
    bool moveWidget( const QWidget& widget, const QPoint& position )
    { return moveResizeWidget( widget, position, X11Util::_NET_WM_MOVERESIZE_MOVE ); }

    //! move/resize widget using X11 window manager
    /*! returns true on success */
    bool moveResizeWidget(
        const QWidget&,
        const QPoint&,
        const Direction&,
        Qt::MouseButton button = Qt::LeftButton );

    #if defined(Q_WS_X11)

    //! find atom
    Atom findAtom( const Atoms& atom );

    #endif

    private:

    //! constructor
    X11Util( void );

    //! change property
    bool _changeProperty( const QWidget&, const Atoms&, bool value );

    //! change property
    bool _requestPropertyChange( const QWidget&, const Atoms&, bool value );

    //! cardinal
    bool _changeCardinal( const QWidget&, const Atoms&, const unsigned long& value );

    //! cardinal
    bool _requestCardinalChange( const QWidget&, const Atoms&, const unsigned long& value );

    //! atom names
    typedef QMap<Atoms, QString> AtomNameMap;

    //! atom names
    void _initializeAtomNames( void );

    //! atom names
    const AtomNameMap& _atomNames( void ) const
    { return atomNames_; }

    AtomNameMap atomNames_;

    //! supported atoms
    typedef QHash<Atoms, bool > SupportedAtomMap;

    //! supported atoms
    const SupportedAtomMap& _supportedAtoms( void ) const
    { return supportedAtoms_; }

    SupportedAtomMap supportedAtoms_;

    #if defined(Q_WS_X11)

    //! atom map
    typedef QHash<Atoms, Atom> AtomMap;

    //! atoms
    // const AtomMap& _atoms( void ) const
    AtomMap& _atoms( void )
    { return atoms_; }

    //! atoms
    AtomMap atoms_;

    #endif

};

#endif
