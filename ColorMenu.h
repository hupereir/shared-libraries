#ifndef _ColorMenu_h_
#define _ColorMenu_h_

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

#include <QMenu>
#include <QPaintEvent>
#include <QPixmap>
#include <QHash>
#include <QMap>

//! Color selection menu
class ColorMenu: public QMenu, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! sorted set of colors
    typedef QOrderedSet<BASE::Color> ColorSet;

    //! constructor
    ColorMenu( QWidget* parent );

    //! destructor
    virtual ~ColorMenu( void )
    { Debug::Throw( "ColorMenu::~ColorMenu.\n" ); }

    //!@name accessors
    //@{

    //! retrieve colors
    ColorSet colors() const;

    //! retrieves last selected color
    QColor lastColor( void ) const
    { return lastColor_; }

    //@}

    //!@name modifiers
    //@{

    //! clear
    void reset( void )
    { colors_.clear(); }

    //! add
    void add( QColor );

    //@}

    signals:

    //! emmited when a color is selected
    void selected( QColor );

    protected:

    //! paint event (overloaded)
    void paintEvent( QPaintEvent* event );

    private slots:

    //! display colors when about to show
    void _display( void );

    //! adds a new color
    void _new( void );

    //! select the no color choice
    /*! emits colorSelected with an invalid color */
    void _default( void );

    //! get color associated to action, emit ColorSelected
    void _selected( QAction* );

    private:

    //! add a color
    void add( QString );

    //! map colors to display pixmap
    typedef QMap<BASE::Color, QBrush> ColorMap;

    //! list of loaded colors
    ColorMap colors_;

    //! map actions to colors
    typedef QHash<QAction*, QColor> ActionMap;

    //! map actions to colors
    ActionMap actions_;

    //! keep track of the last selected color
    QColor lastColor_;

};
#endif
