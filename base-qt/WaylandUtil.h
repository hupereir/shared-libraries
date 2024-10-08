#ifndef WaylandUtil_h
#define WaylandUtil_h

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

#include "NonCopyable.h"
#include "XcbDefines.h"
#include "base_qt_export.h"

#include <QHash>
#include <QIcon>
#include <QWidget>

#include <memory>

class BASE_QT_EXPORT WaylandUtil: private Base::NonCopyable<WaylandUtil>
{

    public:

    //* singleton
    static WaylandUtil& get();

    //* destructor
    ~WaylandUtil() = default;

    //*@name accessors
    //@{

    //* true if platform is X11
    static bool isWayland();

    //* move a top level widget to a given position
    static void moveWidget( QWidget*, const QPoint& );

    //* hide widget from taskbar
    static void toggleHideWidgetFromTaskbar( QWidget*, bool );

    //* hide widget from taskbar
    static void toggleShowWidgetOnAllDesktops( QWidget*, bool );

    //* stay on top
    static void toggleWidgetStaysOnTop( QWidget*, bool );

    //@}

    private:

    //* constructor
    explicit WaylandUtil();

    //* private pointer
    class Private;
    std::unique_ptr<Private> d;

};

#endif
