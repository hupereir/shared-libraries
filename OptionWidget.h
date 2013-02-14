#ifndef OptionWidget_h
#define OptionWidget_h

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

#include <QObject>
#include <QString>

class OptionWidget
{
    public:

    //! constructor
    OptionWidget( const QString& name ):
        optionName_( name ),
        buddy_(0)
    {}

    //! destructor
    virtual ~OptionWidget()
    {}

    //! set widget value from option value
    virtual void read( void ) = 0;

    //! set option value from widget value
    virtual void write( void ) const = 0;

    //! option name
    const QString& optionName( void ) const
    { return optionName_; }

    //! true if has buddy
    bool hasBuddy( void ) const
    { return buddy_; }

    //! buddy
    QObject& buddy( void ) const
    { return *buddy_; }

    protected:

    //! set buddy
    virtual void _setBuddy( QObject* buddy )
    { buddy_ = buddy; }

    private:

    //! name of the option linked to the widget
    QString optionName_;

    //! buddy
    /*! it is used for signal/slots connections in derived class implementations */
    QObject* buddy_;

};

#endif
