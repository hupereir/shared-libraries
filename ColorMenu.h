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

#include "Counter.h"
#include "Debug.h"

#include <QMenu>
#include <QPaintEvent>
#include <QPixmap>
#include <QSet>
#include <QHash>
#include <QMap>

//! Color selection menu
class ColorMenu: public QMenu, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! default name for no-color
    static const QString NONE;

    //! sorted set of colors
    typedef QSet<QString> ColorSet;

    //! constructor
    ColorMenu( QWidget* parent );

    //! destructor
    virtual ~ColorMenu( void )
    { Debug::Throw( "ColorMenu::~ColorMenu.\n" ); }

    //! add a color
    void add( const QString& );

    //! retrieve colors
    ColorSet colors() const;

    //! retrieves last selected color
    const QColor& lastColor( void ) const
    { return lastColor_; }

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
    void _add( const QColor& );

    //! map colors to display pixmap
    typedef QMap<QString, QBrush> ColorMap;

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
