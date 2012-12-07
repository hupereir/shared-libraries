#ifndef PlacesWidget_h
#define PlacesWidget_h

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
#include "IconSize.h"

#include <QtCore/QList>
#include <QtCore/QMimeData>

#include <QtGui/QAbstractButton>
#include <QtGui/QAbstractItemView>
#include <QtGui/QAction>
#include <QtGui/QButtonGroup>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDragLeaveEvent>
#include <QtGui/QLayout>
#include <QtGui/QPaintEvent>
#include <QtGui/QWidget>

// forward declaration
class PlacesWidgetItem;
class BaseFileIconProvider;
class ContextMenu;
class IconSizeMenu;

//! places widget
class PlacesWidget: public QWidget, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    PlacesWidget( QWidget* = 0 );

    //! destructor
    virtual ~PlacesWidget( void )
    {}

    //! set icon provider
    void setIconProvider( BaseFileIconProvider* );

    //! true if empty
    bool isEmpty( void ) const
    { return group_->buttons().isEmpty(); }

    signals:

    //! item selected
    void itemSelected( const BaseFileInfo& );

    //! emmited when contents is changed
    void contentsChanged( void );

    public slots:

    //! clear all items
    void clear( void );

    //! add item
    void add( const BaseFileInfo& );

    //! add item
    void add( const QString&, const BaseFileInfo& );

    //! add item
    void add( const QIcon&, const QString&, const BaseFileInfo& );

    //! insert item
    void insert( int, const BaseFileInfo& );

    //! insert item
    void insert( int, const QString&, const BaseFileInfo& );

    //! insert item
    void insert( int, const QIcon&, const QString&, const BaseFileInfo& );

    protected slots:

    //! button clicked
    void _buttonClicked( QAbstractButton* );

    //! update context menu
    void _updateMenu( void );

    //! icon size
    void _updateIconSize( IconSize::Size );

    //! add item
    void _addItem( void );

    //! edit item
    void _editItem( void );

    //! remove item
    void _removeItem( void );

    protected:

    //! drag enter event
    virtual void dragEnterEvent( QDragEnterEvent* );

    //! drag move
    virtual void dragMoveEvent( QDragMoveEvent* );

    //! drag leave
    virtual void dragLeaveEvent( QDragLeaveEvent* );

    //! drop
    virtual void dropEvent( QDropEvent* );

    //! mouse press
    virtual void mousePressEvent( QMouseEvent* );

    //! paint event
    virtual void paintEvent( QPaintEvent* );

    //! Read fileList from file
    bool _read( void );

    //! write fileList to file
    bool _write( void );

    //! set db file
    bool _setDBFile( const File& );

    //! drag target
    QPoint _dragTarget( const QPoint& ) const;

    //! true if mime data can be decoded
    bool _canDecode( const QMimeData* ) const;

    //! decode mime data
    QList<BaseFileInfo> _decode( const QMimeData* ) const;

    //! return item that have focus or none
    PlacesWidgetItem* _focusItem( void ) const;

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    //! save configuration
    void _saveConfiguration( void );

    private:

    //! install actions
    void _installActions( void );

    //! dummy treeview, needed for rendering items
    QAbstractItemView* itemView_;

    //! button layout
    QBoxLayout* buttonLayout_;

    //! button group
    QButtonGroup* group_;

    //! context menu
    ContextMenu* contextMenu_;

    //! icon size menu
    IconSizeMenu* iconSizeMenu_;

    //! list of items
    QList<PlacesWidgetItem*> items_;

    //! focus item (for context menu actions)
    PlacesWidgetItem* focusItem_;

    //! drag item
    PlacesWidgetItem* dragItem_;

    //! icon provider
    BaseFileIconProvider* iconProvider_;

    //! file from/to wich the files are saved
    File dbFile_;

    //! true if drag is in progress
    bool dragInProgress_;

    //! drag point (for internal dragging)
    QPoint dragPoint_;

    //! drag point
    QPoint dragTarget_;

    //!@name actions
    //@{

    //! add item
    QAction* addItemAction_;

    //! edit item
    QAction* editItemAction_;

    //! delete item
    QAction* removeItemAction_;

    //@}


};

#endif
