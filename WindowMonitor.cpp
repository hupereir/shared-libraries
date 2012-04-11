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
\file WindowMonitor.cpp
\brief used to automatically save size of top level widgets
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <cassert>

#include "XmlOptions.h"
#include "WindowMonitor.h"



//_________________________________________________________
WindowMonitor::WindowMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( "WindowMonitor" ),
    mode_( SIZE )
{ parent->installEventFilter(this); }

//_________________________________________________________
void WindowMonitor::setOptionName( const QString& name )
{
    Debug::Throw( "WindowMonitor::setOptionName.\n" );
    if( name.isEmpty() )
    {

        widthOptionName_.clear();
        heightOptionName_.clear();
        xOptionName_.clear();
        yOptionName_.clear();

    } else {

        widthOptionName_ = name + "_WIDTH";
        heightOptionName_ = name + "_HEIGHT";
        xOptionName_ = name + "_X";
        yOptionName_ = name + "_Y";

    }
}

//_________________________________________________________
QSize WindowMonitor::sizeHint( void ) const
{

    // resize
    if( _hasOptionName() && ( _mode()&SIZE ) && XmlOptions::get().contains( _widthOptionName() ) && XmlOptions::get().contains( _heightOptionName() ) )
    {

        QSize size(
            XmlOptions::get().get<int>( _widthOptionName() ),
            XmlOptions::get().get<int>( _heightOptionName() ) );

        Debug::Throw()
            << "WindowMonitor::sizeHint - ("
            << size.width() << ","
            << size.height() << ")"
            << endl;

        return size;

    } else return QSize();

}

//_______________________________________________________
QPoint WindowMonitor::position( void ) const
{

    // resize
    if( _hasOptionName() && ( _mode()&POSITION ) && XmlOptions::get().contains( _xOptionName() ) && XmlOptions::get().contains( _yOptionName() ) )
    {

        QPoint position(
            XmlOptions::get().get<int>( _xOptionName() ),
            XmlOptions::get().get<int>( _yOptionName() ) );

        Debug::Throw()
            << "WindowMonitor::position - ("
            << position.x() << ","
            << position.y() << ")"
            << endl;

        return position;

    } else return QPoint();

}

//_______________________________________________________
bool WindowMonitor::eventFilter( QObject* target, QEvent* event )
{

    if( target != parent()  ) return false;
    if( !_hasOptionName() ) return false;
    if(
        (event->type() == QEvent::Resize && ( _mode()&SIZE )) ||
        (event->type() == QEvent::Move && ( _mode()&POSITION ) ) )
    { timer_.start( 200, this ); }

    return false;
}

//_______________________________________________________
void WindowMonitor::timerEvent( QTimerEvent* event )
{

    if( event->timerId() == timer_.timerId() )
    {

        // stop timer
        assert( _hasOptionName() );
        timer_.stop();

        // save size
        if( _mode()&SIZE ) _saveWindowSize();
        if( _mode()&POSITION ) _saveWindowPosition();

    } else return QObject::timerEvent( event );

}

//____________________________________________________________
void WindowMonitor::_saveWindowSize( void ) const
{

    QSize size( static_cast<QWidget*>(parent())->size() );

    Debug::Throw()
        << "WindowMonitor::_saveWindowSize - ("
        << size.width() << ","
        << size.height() << ")"
        << endl;

    XmlOptions::get().set<int>( _widthOptionName(), size.width() );
    XmlOptions::get().set<int>( _heightOptionName(), size.height() );
}

//____________________________________________________________
void WindowMonitor::_saveWindowPosition( void ) const
{

    QPoint position( static_cast<QWidget*>(parent())->pos() );

    Debug::Throw()
        << "WindowMonitor::_saveWindowPosition - ("
        << position.x() << ","
        << position.y() << ")"
        << endl;

    XmlOptions::get().set<int>( _xOptionName(), position.x() );
    XmlOptions::get().set<int>( _yOptionName(), position.y() );
}
