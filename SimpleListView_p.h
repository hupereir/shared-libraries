#ifndef SimpleListView_p_h
#define SimpleListView_p_h
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
*******************************************************************************/

#include "Counter.h"

#include <QAbstractItemDelegate>
#include <QStyleOptionViewItem>
#include <QTextLayout>

//! item delegate
class SimpleListViewDelegate : public QAbstractItemDelegate, public Counter
{

    public:

    //! constructor
    SimpleListViewDelegate( QObject* = 0 );

    //! destructor
    virtual ~SimpleListViewDelegate( void )
    {}

    //! paint
    virtual void paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;

    //! size
    virtual QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    private:

    //! layout text
    int _layoutText(QTextLayout*, int) const;

    //! focus
    void _drawFocus( QPainter*, const QStyleOptionViewItem&, const QRect& ) const;

    //! icon size
    int iconSize_;

};


#endif
