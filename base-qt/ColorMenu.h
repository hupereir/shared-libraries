#ifndef ColorMenu_h
#define ColorMenu_h

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

#include <QMenu>
#include <QPaintEvent>
#include <QPixmap>
#include <QHash>
#include <QMap>

//* Color selection menu
class ColorMenu: public QMenu, private Base::Counter<ColorMenu>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit ColorMenu( QWidget* );

    //*@name accessors
    //@{

    //* retrieve colors
    Base::Color::Set colors() const;

    //* retrieves last selected color
    QColor lastColor( void ) const
    { return lastColor_; }

    //@}

    //*@name modifiers
    //@{

    //* clear
    void reset( void )
    { colors_.clear(); }

    //* add
    void add( QColor );

    //@}

    Q_SIGNALS:

    //* emitted when a color is selected
    void selected( QColor );

    protected:

    //* paint event (overloaded)
    void paintEvent( QPaintEvent* event ) override;

    private Q_SLOTS:

    //* display colors when about to show
    void _display( void );

    //* adds a new color
    void _new( void );

    //* select the no color choice
    /** emits colorSelected with an invalid color */
    void _default( void );

    //* get color associated to action, emit ColorSelected
    void _selected( QAction* );

    private:

    //* map colors to display pixmap
    using ColorMap = QMap<Base::Color, QBrush>;

    //* list of loaded colors
    ColorMap colors_;

    //* map actions to colors
    using ActionMap = QHash<QAction*, QColor>;

    //* map actions to colors
    ActionMap actions_;

    //* keep track of the last selected color
    QColor lastColor_;

};
#endif
