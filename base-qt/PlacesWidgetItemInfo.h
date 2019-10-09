#ifndef PlacesWidgetItemInfo_h
#define PlacesWidgetItemInfo_h

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

#include "BaseFileInfo.h"

#include <QDomElement>
#include <QDomDocument>

//* local file info, needed to store flags
class PlacesWidgetItemInfo: public BaseFileInfo
{

    public:

    enum Flag
    {
        ReadOnly = 1<<0,
        Hidden = 1<<1,
        Separator = 1<<2
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //* default constructor
    explicit PlacesWidgetItemInfo()
    {}

    //* copy constructor
    explicit PlacesWidgetItemInfo( const BaseFileInfo& other ):
        BaseFileInfo( other )
    {}

    //* constructor from DOM element
    explicit PlacesWidgetItemInfo( const QDomElement& );

    //*@name accessors
    //@{

    //* dump to dom element
    QDomElement domElement( QDomDocument& ) const;

    //* flags
    Flags flags() const
    { return flags_; }

    //* has flag
    bool hasFlag( Flag flag ) const
    { return flags_&flag; }

    //@}

    //*@name modifiers
    //@{

    //* set flags
    void setFlags( Flags flags )
    { flags_ = flags; }

    //* set flag
    void setFlag( Flag flag, bool value )
    {
        if( value ) flags_ |= flag;
        else flags_ &= ~flag;
    }

    //@}


    //* read/write file info list to xml
    using List = QList<PlacesWidgetItemInfo>;
    class Helper
    {

        public:

        static List list( const QDomElement& );
        static QDomElement domElement( const List&, QDomDocument& );

    };

    private:

    //* flags
    Flags flags_ = 0;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( PlacesWidgetItemInfo::Flags )

#endif
