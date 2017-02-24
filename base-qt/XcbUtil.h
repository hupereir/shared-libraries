#ifndef XcbUtil_h
#define XcbUtil_h

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

#include "XcbDefines.h"

#include <QHash>
#include <QWidget>

#include <memory>

class XcbUtil: private XcbDefines
{

    public:

    template <typename T> using ScopedPointer = std::unique_ptr<T>;

    //* singleton
    static XcbUtil& get( void );

    //* destructor
    virtual ~XcbUtil( void );

    //*@name accessors
    //@{

    //* connection
    template <typename T> T* connection( void ) const;

    //* default screen number
    int defaultScreenNumber( void ) const;

    //* application root window
    WId appRootWindow( void ) const;

    //* atom
    template <typename T> T* atom( const QString& ) const;

    //* atom
    template <typename T> T* atom( AtomId ) const;

    //* supported atoms
    bool isSupported( AtomId ) const;

    //* return true if a give window is 'real'
    /** window is considered real if it has the WM_STATE property */
    bool isRealWindow( WId ) const;

    //* return formated window id
    QString windowIdString( WId id ) const
    { return QString( "0x%1" ).arg(quint64( id), 0, 16 ); }

    //* return true if atom is found in NET_WM_STATE
    bool hasState( QWidget* widget, AtomId atom ) const
    { return hasState( widget->winId(), atom ); }

    //* return true if atom is found in NET_WM_STATE
    bool hasState( WId, AtomId ) const;

    //* print window state
    void printState( QWidget* widget ) const
    { printState( widget->winId() ); }

    //* print window state
    void printState( WId ) const;

    //* get atom carninal value
    uint32_t cardinal( QWidget* widget, AtomId atom ) const
    { return cardinal( widget->winId(), atom ); }

    //* get atom carninal value
    uint32_t cardinal( WId, AtomId ) const;

    //@}

    //*@name modifiers
    //@{

    //* change NET_WM_STATE atom
    bool changeState( QWidget*, AtomId, bool) const;

    //* change cardinal
    bool changeCardinal( QWidget*, AtomId, uint32_t ) const;

    //* move widget using X11 window manager
    /** returns true on success */
    bool moveWidget( QWidget* widget, QPoint position )
    { return moveResizeWidget( widget, position, XcbUtil::_NET_WM_MOVERESIZE_MOVE ); }

    //* move/resize widget using X11 window manager
    /** returns true on success */
    bool moveResizeWidget( QWidget*, QPoint, Direction, Qt::MouseButton button = Qt::LeftButton );

    //@}

    protected:

    //* connection
    Qt::HANDLE _connection( void ) const;

    //* atom
    Qt::HANDLE _atom( const QString& ) const;

    //* atom
    Qt::HANDLE _atom( AtomId ) const;

    private:

    //* change state
    bool _changeState( QWidget*, AtomId, bool ) const;

    //* request state change
    bool _requestStateChange( QWidget*, AtomId, bool ) const;

    //* cardinal
    bool _changeCardinal( QWidget*, AtomId, uint32_t ) const;

    //* cardinal
    bool _requestCardinalChange( QWidget*, AtomId, uint32_t ) const;

    //* constructor
    XcbUtil( void );

    //* private pointer
    class Private;
    std::unique_ptr<Private> d;

};

//________________________________________________________________________
template <typename T> T* XcbUtil::connection( void ) const
{ return reinterpret_cast<T*>( _connection() ); }

//________________________________________________________________________
template <typename T> T* XcbUtil::atom( const QString& value ) const
{ return reinterpret_cast<T*>( _atom( value ) ); }

//________________________________________________________________________
template <typename T> T* XcbUtil::atom( XcbUtil::AtomId value ) const
{ return reinterpret_cast<T*>( _atom( value ) ); }

#endif
