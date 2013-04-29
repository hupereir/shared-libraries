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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "X11Defines.h"

#include <QWidget>
#include <QHash>
#include <QMap>

class X11Util: public X11Defines
{

    public:

    //! singleton
    static X11Util& get( void );

    //! destructor
    virtual ~X11Util( void );

    //! supported atoms
    bool isSupported( Atoms atom );

    //! return true if property is found in NET_WM_STATE
    bool hasProperty( QWidget*, Atoms );

    //! change property
    bool changeProperty( QWidget*, Atoms, bool);

    //! change property
    bool changeProperty( QWidget*, Atoms, const unsigned char*, int size );

    //! print window state
    void printWindowState( QWidget* );

    //! display
    Qt::HANDLE display( void ) const;

    //! atom
    Qt::HANDLE findAtom( Atoms ) const;

    //! application root window
    WId appRootWindow( void ) const;

    //! get atom carninal value
    unsigned long cardinal( QWidget* widget, Atoms atom )
    { return cardinal( widget->winId(), atom ); }

    //! get atom carninal value
    unsigned long cardinal( WId, Atoms );

    //! get atom carninal value
    bool changeCardinal( QWidget* widget, Atoms atom, unsigned long value )
    {
        return widget->isHidden() ?
            _changeCardinal( widget, atom, value ):
            _requestCardinalChange( widget, atom, value );
    }

    //! move widget using X11 window manager
    /*! returns true on success */
    bool moveWidget( QWidget* widget, const QPoint& position )
    { return moveResizeWidget( widget, position, X11Util::_NET_WM_MOVERESIZE_MOVE ); }

    //! move/resize widget using X11 window manager
    /*! returns true on success */
    bool moveResizeWidget(
        QWidget*,
        const QPoint&,
        const Direction&,
        Qt::MouseButton button = Qt::LeftButton );

    private:

    //! constructor
    X11Util( void );

    //! change property
    bool _changeProperty( QWidget*, Atoms, bool value );

    //! change property
    bool _requestPropertyChange( QWidget*, Atoms, bool value );

    //! cardinal
    bool _changeCardinal( QWidget*, Atoms, unsigned long value );

    //! cardinal
    bool _requestCardinalChange( QWidget*, Atoms, unsigned long value );

    //! atom names
    typedef QMap<Atoms, QString> AtomNameMap;

    //! supported atoms
    typedef QHash<Atoms, bool > SupportedAtomMap;

    SupportedAtomMap supportedAtoms_;

    //! private pointer
    class Private;
    Private* d;

};

#endif
