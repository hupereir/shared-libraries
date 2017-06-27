#ifndef PlacesWidget_h
#define PlacesWidget_h

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
#include "BaseFileInfo.h"
#include "IconSize.h"
#include "FileSystemWatcher.h"

#include <QList>
#include <QMimeData>

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QAction>
#include <QButtonGroup>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QLayout>
#include <QPaintEvent>
#include <QWidget>

// forward declaration
namespace Private
{
    class PlacesToolTipWidget;
    class PlacesWidgetItem;
}

class BaseFileIconProvider;
class IconSizeMenu;

//* places widget
class PlacesWidget: public QWidget, private Base::Counter<PlacesWidget>
{

    Q_OBJECT

    public:

    //* constructor
    explicit PlacesWidget( QWidget* = nullptr );

    //* set icon provider
    void setIconProvider( BaseFileIconProvider* );

    //* true if empty
    bool isEmpty() const
    { return group_->buttons().isEmpty(); }

    //* list of items
    QList<BaseFileInfo> items() const;

    //* set item enabled
    bool setItemIsValid( const BaseFileInfo&, bool );

    //* event filter
    bool eventFilter( QObject*, QEvent* ) override;

    Q_SIGNALS:

    //* item added
    void remoteItemAdded( const BaseFileInfo& );

    //* item selected
    void itemSelected( const BaseFileInfo& );

    public Q_SLOTS:

    //* clear all items
    void clear();

    //* add item
    void add( const BaseFileInfo& );

    //* add item
    void add( const QString&, const BaseFileInfo& );

    //* add item
    void add( const QIcon&, const QString&, const BaseFileInfo& );

    //* insert item
    void insert( int, const BaseFileInfo& );

    //* insert item
    void insert( int, const QString&, const BaseFileInfo& );

    //* insert item
    void insert( int, const QIcon&, const QString&, const BaseFileInfo& );

    protected Q_SLOTS:

    //* button clicked
    void _buttonClicked( QAbstractButton* );

    //* update focus
    void _updateFocus( QAbstractButton* );

    //* update context menu
    void _updateContextMenu( const QPoint& );

    //* icon size
    void _updateIconSize( IconSize::Size );

    //* add item
    void _addItem();

    //* add separator
    void _addSeparator();

    //* add separator
    void _insertSeparator();

    //* edit item
    void _editItem();

    //* remove item
    void _removeItem();

    //* update item state
    void _updateItems();

    //* hide item
    void _toggleHideItem( bool );

    //* show all entries
    void _toggleShowAllEntries( bool );

    protected:

    //*@name event handlers
    //@{

    //* drag enter event
    void dragEnterEvent( QDragEnterEvent* ) override;

    //* drag move
    void dragMoveEvent( QDragMoveEvent* ) override;

    //* drag leave
    void dragLeaveEvent( QDragLeaveEvent* ) override;

    //* drop
    void dropEvent( QDropEvent* ) override;

    //* mouse press
    void mousePressEvent( QMouseEvent* ) override;

    //* paint event
    void paintEvent( QPaintEvent* ) override;

    //@}

    //* Read fileList from file
    bool _read();

    //* write fileList to file
    bool _write();

    //* install default folders
    void _addDefaultPlaces();

    //* set db file
    bool _setDBFile( const File& );

    //* update drag state
    void _updateDragState() const;

    //* drag target
    QPoint _updateDragTarget( const QPoint& ) const;

    //* return item index based on position
    int _index( const QPoint& ) const;

    //* true if mime data can be decoded
    bool _canDecode( const QMimeData* ) const;

    //* decode mime data
    QList<BaseFileInfo> _decode( const QMimeData* ) const;

    //* return item that have focus or none
    Private::PlacesWidgetItem* _focusItem() const;

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration();

    //* save configuration
    void _saveConfiguration();

    private:

    //* install actions
    void _installActions();

    //* dummy treeview, needed for rendering items
    QAbstractItemView* itemView_ = nullptr;

    //* button layout
    QBoxLayout* buttonLayout_ = nullptr;

    //* button group
    QButtonGroup* group_ = nullptr;

    //* icon size menu
    IconSizeMenu* iconSizeMenu_ = nullptr;

    //* tooltip widget
    Private::PlacesToolTipWidget* toolTipWidget_ = nullptr;

    //* list of items
    QList<Private::PlacesWidgetItem*> items_;

    //* focus item (for context menu actions)
    Private::PlacesWidgetItem* focusItem_ = nullptr;

    //* drag item
    Private::PlacesWidgetItem* dragItem_ = nullptr;

    //* icon provider
    BaseFileIconProvider* iconProvider_ = nullptr;

    //* file system watcher, for local selection frame
    FileSystemWatcher fileSystemWatcher_;

    //* file from/to wich the files are saved
    File dbFile_;

    //* true if drag is in progress
    bool dragInProgress_ = false;

    //* drag point (for internal dragging)
    QPoint dragPoint_;

    //* drag point
    QPoint dragTarget_;

    //*@name actions
    //@{

    //* add item
    QAction* addItemAction_ = nullptr;

    //* add separator
    QAction* addSeparatorAction_ = nullptr;

    //* edit item
    QAction* editItemAction_ = nullptr;

    //* delete item
    QAction* removeItemAction_ = nullptr;

    //* hide item
    QAction* hideItemAction_ = nullptr;

    //* show all entries action
    QAction* showAllEntriesAction_ = nullptr;

    //@}


};

#endif
