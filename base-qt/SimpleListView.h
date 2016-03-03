#ifndef SimpleListView_h
#define SimpleListView_h

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

#include "TreeView.h"
#include <QListView>
class SimpleListView: public QListView
{

    Q_OBJECT

    public:

    //* constructor
    SimpleListView( QWidget* = nullptr );

    //* model
    virtual void setModel( QAbstractItemModel* );

    Q_SIGNALS:

    //* emitted when index is hovered
    /**
    this is similar to the 'entered' signal,
    except that it is also sent when no index is selected,
    for instance, to hide tooltips
    */
    void hovered( const QModelIndex& );

    protected Q_SLOTS:

    //* adjust width
    void _adjustWidth( void );

    //* hover index
    virtual void _indexEntered( const QModelIndex& );

    protected:

    //* generic event
    virtual bool event( QEvent* );

    //* mouse move event
    virtual void mouseMoveEvent( QMouseEvent* );

    //* mouse press event
    virtual void mousePressEvent( QMouseEvent* );

    //* hover index
    virtual void _setHoverIndex( const QModelIndex& );

    private:

    //* hover index
    QModelIndex hoverIndex_;


};

#endif
