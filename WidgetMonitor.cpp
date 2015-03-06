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

#include "WidgetMonitor.h"
#include "XmlOptions.h"

//_________________________________________________________
WidgetMonitor::WidgetMonitor( QWidget* parent ):
    QObject( parent ),
    Counter( "WidgetMonitor" ),
    mode_( Size )
{ parent->installEventFilter(this); }

//_________________________________________________________
void WidgetMonitor::setOptionName( const QString& name )
{
    Debug::Throw( "WidgetMonitor::setOptionName.\n" );
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
QSize WidgetMonitor::sizeHint( void ) const
{

    // resize
    if( _hasOptionName() && ( mode_&Size ) && XmlOptions::get().contains( widthOptionName_ ) && XmlOptions::get().contains( heightOptionName_ ) )
    {

        QSize size(
            XmlOptions::get().get<int>( widthOptionName_ ),
            XmlOptions::get().get<int>( heightOptionName_ ) );

        Debug::Throw()
            << "WidgetMonitor::sizeHint - ("
            << size.width() << ","
            << size.height() << ")"
            << endl;

        return size;

    } else return QSize();

}

//_______________________________________________________
QPoint WidgetMonitor::position( void ) const
{

    // resize
    if( _hasOptionName() && ( mode_&Position ) && XmlOptions::get().contains( xOptionName_ ) && XmlOptions::get().contains( yOptionName_ ) )
    {

        QPoint position(
            XmlOptions::get().get<int>( xOptionName_ ),
            XmlOptions::get().get<int>( yOptionName_ ) );

        Debug::Throw()
            << "WidgetMonitor::position - ("
            << position.x() << ","
            << position.y() << ")"
            << endl;

        return position;

    } else return QPoint();

}

//_______________________________________________________
bool WidgetMonitor::eventFilter( QObject* target, QEvent* event )
{

    if( target != parent()  ) return false;
    if( !_hasOptionName() ) return false;
    if(
        (event->type() == QEvent::Resize && ( mode_&Size )) ||
        (event->type() == QEvent::Move && ( mode_&Position ) ) )
    { timer_.start( 200, this ); }

    return false;
}

//_______________________________________________________
void WidgetMonitor::timerEvent( QTimerEvent* event )
{

    if( event->timerId() == timer_.timerId() )
    {

        // stop timer
        Q_ASSERT( _hasOptionName() );
        timer_.stop();

        // save size
        if( mode_&Size ) _saveWindowSize();
        if( mode_&Position ) _saveWindowPosition();

    } else return QObject::timerEvent( event );

}

//____________________________________________________________
void WidgetMonitor::_saveWindowSize( void ) const
{

    QSize size( static_cast<QWidget*>(parent())->size() );

    Debug::Throw()
        << "WidgetMonitor::_saveWindowSize - ("
        << size.width() << ","
        << size.height() << ")"
        << endl;

    XmlOptions::get().set<int>( widthOptionName_, size.width() );
    XmlOptions::get().set<int>( heightOptionName_, size.height() );
}

//____________________________________________________________
void WidgetMonitor::_saveWindowPosition( void ) const
{

    QPoint position( static_cast<QWidget*>(parent())->pos() );

    Debug::Throw()
        << "WidgetMonitor::_saveWindowPosition - ("
        << position.x() << ","
        << position.y() << ")"
        << endl;

    XmlOptions::get().set<int>( xOptionName_, position.x() );
    XmlOptions::get().set<int>( yOptionName_, position.y() );
}
