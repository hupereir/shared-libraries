#ifndef TextBlockData_h
#define TextBlockData_h

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

#include "TextBlockFlags.h"
#include "Counter.h"
#include "Debug.h"

#include <QTextBlockUserData>

//* TextBlock data
class TextBlockData: public QTextBlockUserData, private Base::Counter<TextBlockData>
{

    public:

    //* constructor
    explicit TextBlockData():
        QTextBlockUserData(),
        Counter( "TextBlockData" ),
        flags_( TextBlock::None )
    {}

    //* flags
    int flags( void ) const
    { return flags_; }

    //* flags
    void setFlags( int flags )
    { flags_ = flags; }

    //* flags
    bool hasFlag( int flag ) const
    { return flags_ & flag; }

    //* flags
    void setFlag( int flag, bool value )
    {
        if( value ) flags_ |= flag;
        else flags_ &= (~flag);
    }

    //* block background
    /** returns true if changed */
    bool setBackground( const QColor& color )
    {
        if( (background_.isValid() || color.isValid() ) && color != background_ )
        {
            background_ = color;
            setFlag( TextBlock::HasBackground, color.isValid() );
            return true;
        } else return false;
    }

    //* block background
    const QColor& background( void ) const
    { return background_; }

    private:

    //* flags
    /* is a bit pattern */
    int flags_ = TextBlock::None;

    //* block background color (overridden by active)
    QColor background_;

};

#endif
