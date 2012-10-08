#ifndef OptionWidgetList_h
#define OptionWidgetList_h

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

#include "Debug.h"
#include "OptionWidget.h"

#include <QtCore/QList>

//! abstract container for OptionWidgets
class OptionWidgetList: public OptionWidget
{

    public:

    //! constructor
    OptionWidgetList( void ):
        OptionWidget( "generic" )
    {}

    //! add option widget
    virtual void addOptionWidget( OptionWidget* widget )
    {
        Debug::Throw( "OptionWidgetList::addOptionWidget.\n" );
        optionWidgets_ << widget;
    }

    //! clear option widgets
    virtual void clearOptionWidgets( void )
    {
        Debug::Throw( "OptionWidgetList::clearOptionWidgets.\n" );
        optionWidgets_.clear();
    }

    //! read options
    virtual void read( void );

    //! read options
    virtual void write( void ) const;

    private:

    //! contained options
    QList< OptionWidget* > optionWidgets_;

};

#endif
