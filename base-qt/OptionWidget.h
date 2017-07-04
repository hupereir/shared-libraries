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

#include <QObject>
#include <QString>

class OptionWidget
{
    public:

    //* constructor
    explicit OptionWidget( const QString& name, QObject* budy = nullptr ):
        optionName_( name ),
        buddy_( budy )
    {}

    //* destructor
    virtual ~OptionWidget() = default;

    //* set widget value from option value
    virtual void read( const Options& ) = 0;

    //* set option value from widget value
    virtual void write( Options& ) const = 0;

    //* true if has buddy
    bool hasBuddy() const
    { return buddy_; }

    //* buddy
    QObject& buddy() const
    { return *buddy_; }

    //* option name
    const QString& optionName() const
    { return optionName_; }

    protected:

    //* set buddy
    void _setBuddy( QObject* buddy )
    { buddy_ = buddy; }

    //* true when connected
    bool _connected() const
    { return connected_; }

    //* set connected
    void _setConnected( bool value = true )
    { connected_ = value; }

    private:

    //* name of the option linked to the widget
    QString optionName_;

    //* buddy
    /** it is used for signal/slots connections in derived class implementations */
    QObject* buddy_ = nullptr;

    //* true when connected
    bool connected_ = false;

};

#endif
