#ifndef _DebugMenu_h_
#define _DebugMenu_h_

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

#include "Counter.h"

#include <QMenu>
#include <QDialog>

class CounterDialog;
class IconCacheDialog;

//! some basic runtime debuging
class DebugMenu: public QMenu, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! flags
    enum Flag
    {
        Counters = 1<<0,
        Icons = 1<<2,
        System = 1<<3,
        Options = 1<<4,
        Default = Counters|Icons|System|Options
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //! constructor
    DebugMenu( QWidget* parent, Flags flags = Default );

    //! destructor
    ~DebugMenu( void )
    {}

    private slots:

    //! print object counters
    void _showCounterDialog( void );

    //! icon cache dialog
    void _showIconCacheDialog( void );

    //! show system environment
    void _showSystemEnvironment( void );

    //! show options
    void _showOptions( void );

    private:

    //! non modal Counter dialog
    CounterDialog* counterDialog_;

    //! non modal icon cache dialog
    IconCacheDialog* iconCacheDialog_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( DebugMenu::Flags )

#endif
