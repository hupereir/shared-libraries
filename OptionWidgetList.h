#ifndef OptionWidgetList_h
#define OptionWidgetList_h

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

#include "Debug.h"
#include "OptionWidget.h"

#include <QList>

//! abstract container for OptionWidgets
class OptionWidgetList: public OptionWidget
{

    public:

    //! constructor
    OptionWidgetList( QObject* buddy = 0 ):
        OptionWidget( "generic", buddy )
    {}

    //! add option widget
    virtual void addOptionWidget( OptionWidget* );

    //! clear option widgets
    virtual void clearOptionWidgets( void )
    {
        Debug::Throw( "OptionWidgetList::clearOptionWidgets.\n" );
        optionWidgets_.clear();
    }

    //! read options
    virtual void read( const Options& );

    //! read options
    virtual void write( Options& ) const;

    protected:

    //! get necessary spacing for checkbox alignment
    int _checkBoxSpacing( void ) const;

    private:

    //! contained options
    QList< OptionWidget* > optionWidgets_;

};

#endif
