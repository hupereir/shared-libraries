#ifndef XcbDefines_h
#define XcbDefines_h

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

#include "base_qt_export.h"
#include <inttypes.h>

class BASE_QT_EXPORT XcbDefines
{

    public:

    //* destructor
    virtual ~XcbDefines() = default;

    //* used to access properties
    static const uint32_t MAX_PROP_SIZE = 100000;

    //* used for stickyness
    static const uint32_t ALL_DESKTOPS = ~0;

    //* Atoms enumeration
    enum class AtomId {
        WM_STATE,

        _NET_SUPPORTED,
        _NET_CURRENT_DESKTOP,
        _NET_WM_DESKTOP,
        _NET_WM_STATE,
        _NET_WM_STATE_STICKY,
        _NET_WM_STATE_STAYS_ON_TOP,
        _NET_WM_STATE_ABOVE,
        _NET_WM_STATE_SKIP_TASKBAR,
        _NET_WM_STATE_SKIP_PAGER,
        _NET_WM_MOVERESIZE,
        _NET_WM_CM,
        _NET_WM_PID,
        _NET_WM_ICON,
        _NET_WM_NAME,
        _NET_NUMBER_OF_DESKTOPS,

        // kde specific atoms
        _KDE_NET_WM_BLUR_BEHIND_REGION
    };

    enum class Direction {
        _NET_WM_MOVERESIZE_SIZE_TOPLEFT=0,
        _NET_WM_MOVERESIZE_SIZE_TOP=1,
        _NET_WM_MOVERESIZE_SIZE_TOPRIGHT=2,
        _NET_WM_MOVERESIZE_SIZE_RIGHT=3,
        _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT=4,
        _NET_WM_MOVERESIZE_SIZE_BOTTOM=5,
        _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT=6,
        _NET_WM_MOVERESIZE_SIZE_LEFT=7,
        _NET_WM_MOVERESIZE_MOVE=8,
        _NET_WM_MOVERESIZE_SIZE_KEYBOARD=9,
        _NET_WM_MOVERESIZE_MOVE_KEYBOARD=10,
    };

};

#endif
