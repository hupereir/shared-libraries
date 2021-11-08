#ifndef OptionWidget_h
#define OptionWidget_h

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

#include "Options.h"
#include "base_qt_export.h"

#include <QObject>
#include <QString>

class BASE_QT_EXPORT OptionWidget
{
    public:

    //* constructor
    explicit OptionWidget( const QString& name ):
        optionName_( name )
        {}

    //* destructor
    virtual ~OptionWidget() = default;

    //* set widget value from option value
    virtual void read( const Options& ) = 0;

    //* set option value from widget value
    virtual void write( Options& ) const = 0;

    //* option name
    const QString& optionName() const
    { return optionName_; }

    protected:

    //* true when connected
    bool _connected() const
    { return connected_; }

    //* set connected
    void _setConnected( bool value = true )
    { connected_ = value; }

    private:

    //* name of the option linked to the widget
    QString optionName_;

    //* true when connected
    bool connected_ = false;

};

#endif
