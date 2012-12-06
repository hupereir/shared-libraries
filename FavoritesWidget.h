#ifndef FavoritesWidget_h
#define FavoritesWidget_h

// $Id$

/*******************************************************************************
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
#include "BaseFileInfo.h"

#include <QtCore/QList>

#include <QtGui/QAbstractButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QLayout>
#include <QtGui/QAbstractItemView>
#include <QtGui/QWidget>

// forward declaration
class FavoritesWidgetItem;
class BaseFileIconProvider;

//! favorites widget
class FavoritesWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    FavoritesWidget( QWidget* = 0 );

    //! destructor
    virtual ~FavoritesWidget( void )
    {}

    //! set icon provider
    void setIconProvider( BaseFileIconProvider* value )
    { iconProvider_ = value; }

    public slots:

    //! add item
    void add( const BaseFileInfo& fileInfo )
    { add( fileInfo.file().localName(), fileInfo ); }

    //! add item
    void add( const QString&, const BaseFileInfo& );

    //! add item
    void add( const QIcon&, const QString&, const BaseFileInfo& );

    signals:

    //! item selected
    void itemSelected( const BaseFileInfo& );

    protected slots:

    //! button clicked
    void _buttonClicked( QAbstractButton* );

    private:

    //! dummy treeview, needed for rendering items
    QAbstractItemView* itemView_;

    //! button layout
    QBoxLayout* buttonLayout_;

    //! button group
    QButtonGroup* group_;

    //! list of items
    QList<FavoritesWidgetItem*> items_;

    //! icon provider
    BaseFileIconProvider* iconProvider_;

};

#endif
