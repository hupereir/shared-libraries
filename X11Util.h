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

#include "X11Defines.h"

#include <QWidget>
#include <QHash>
#include <QMap>

#if defined(Q_WS_X11) || defined( Q5_WS_X11 )
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

class X11Util: public X11Defines
{

    public:

    //! singleton
    static X11Util& get( void );

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

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    //! display
    Display* display( void ) const;

    //! visual
    Visual* appVisual( void ) const;

    #endif

    //! application root window
    WId appRootWindow( void ) const;

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

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

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

    #if defined(Q_WS_X11) || defined( Q5_WS_X11 )

    //! display
    Display* display_;

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
