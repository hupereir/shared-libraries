#ifndef IconSize_h
#define IconSize_h

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

#include <QSize>
#include <QMap>

//* default icon sizes
class IconSize final
{

    public:

    //* icon sizes
    enum Size
    {
        Default = 0,
        Minimum = 8,
        Small = 16,
        Medium = 22,
        Large = 32,
        Huge = 48,
        VeryHuge = 64,
        Oversized = 128,
        Maximum = 256
    };

    //* constructor
    explicit IconSize( Size );

    //* static accessor
    static QSize get( Size );

    //* accessor
    const QSize& get() const { return size_; }

    //* map text to icon size
    using Map = QMap<Size, QString>;

    //* text to icon size
    static const Map& map();

    private:

    //* size
    QSize size_;

};

#endif
