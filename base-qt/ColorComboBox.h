#ifndef ColorComboBox_h
#define ColorComboBox_h

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

#include "Color.h"
#include "Counter.h"

#include "Debug.h"
#include "QOrderedSet.h"

#include <QComboBox>

class ColorComboBox: public QComboBox, private Base::Counter<ColorComboBox>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:


    //* constructor
    explicit ColorComboBox( QWidget* );

    //*@name accessors
    //@{

    //* retrieve colors
    Base::Color::Set colors() const;

    //* current color
    QColor color() const;

    //@}

    //*@name modifiers
    //@{

    //* colors
    void setColors( const Base::Color::Set& );

    //* add color
    void addColor( const QColor& );

    //* select color
    void selectColor( const QColor& );

    //@}

    private Q_SLOTS:

    //* update selected color
    void _updateActiveIndex( int );

    private:

    //* last selected color
    QColor lastColor_;

};

#endif
