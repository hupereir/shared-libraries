#ifndef TextEncodingMenu_h
#define TextEncodingMenu_h

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

#include "Counter.h"
#include "base_qt_export.h"

#include <QAction>
#include <QHash>
#include <QMenu>

class BASE_QT_EXPORT TextEncodingMenu: public QMenu, private Base::Counter<TextEncodingMenu>
{

    Q_OBJECT

    public:

    //* constructor
    explicit TextEncodingMenu( QWidget* = 0 );

    //* select
    void select( const QByteArray& );

    Q_SIGNALS:

    //* emitted when encoding is changed
    void encodingChanged( const QByteArray& );

    private:

    //* size selected from action
    void _selected( QAction* );

    //* map action to encoding
    using ActionMap = QHash<QAction*, QByteArray>;
    ActionMap actions_;

};

#endif
