#ifndef WindowMonitor_h
#define WindowMonitor_h

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
\file WindowMonitor.h
\brief used to automatically save size of top level widgets
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <QBasicTimer>
#include <QWidget>
#include <QSize>
#include <QTimerEvent>

#include "Counter.h"

//! used to automatically save size of top level widgets
class WindowMonitor: public QObject, public Counter
{

    public:

    //! constructor
    WindowMonitor( QWidget* );

    //! mode
    enum Mode
    {
        None = 0,
        Size = 1<<0,
        Position = 1<<1
    };

    Q_DECLARE_FLAGS( Modes, Mode );

    //! mode
    void setMode( Modes value )
    { mode_ = value; }

    //! size option name
    virtual void setOptionName( const QString& );

    //! size hint
    /*! an invalid size is returned when option name is not set */
    QSize sizeHint( void ) const;

    //! position
    QPoint position( void ) const;

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    protected:

    //! timer event
    /* need to save updated window size */
    virtual void timerEvent( QTimerEvent* );

    private:

    //! true when option name was set
    bool _hasOptionName( void ) const
    { return !widthOptionName_.isEmpty(); }

    //! save window size
    void _saveWindowSize( void ) const;

    //! save position
    void _saveWindowPosition( void ) const;

    //! resize timer
    QBasicTimer timer_;

    //! mode
    Modes mode_;

    //! window size option name
    QString widthOptionName_;

    //! window size option name
    QString heightOptionName_;

    //! position option name
    QString xOptionName_;

    //! position option name
    QString yOptionName_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( WindowMonitor::Modes )

#endif
