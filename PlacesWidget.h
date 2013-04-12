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
class PlacesToolTipWidget;
class PlacesWidgetItem;
class BaseFileIconProvider;
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

    //! install default folders
    void installDefaultFolders( void );

    //! true if empty
    bool isEmpty( void ) const
    { return group_->buttons().isEmpty(); }

    //! list of items
    QList<BaseFileInfo> items( void ) const;

    //! set item enabled
    bool setItemIsValid( const BaseFileInfo&, bool );

    //! Read fileList from file
    bool read( File = File() );

    //! write fileList to file
    bool write( File = File() );

    //! event filter
    virtual bool eventFilter( QObject*, QEvent* );

    signals:

    //! item added
    void remoteItemAdded( const BaseFileInfo& );

    //! item selected
    void itemSelected( const BaseFileInfo& );

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

    //! update focus
    void _updateFocus( QAbstractButton* );

    //! update context menu
    void _updateContextMenu( const QPoint& );

    //! icon size
    void _updateIconSize( IconSize::Size );

    //! add item
    void _addItem( void );

    //! add separator
    void _addSeparator( void );

    //! add separator
    void _insertSeparator( void );

    //! edit item
    void _editItem( void );

    //! remove item
    void _removeItem( void );

    //! update item state
    void _updateItems( void );

    //! hide item
    void _toggleHideItem( bool );

    //! show all entries
    void _toggleShowAllEntries( bool );

    protected:

    //!@name event handlers
    //@{

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

    //@}

    //! set db file
    bool _setDBFile( const File& );

    //! update drag state
    void _updateDragState( void ) const;

    //! drag target
    QPoint _updateDragTarget( const QPoint& ) const;

    //! return item index based on position
    int _index( const QPoint& ) const;

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

    //! icon size menu
    IconSizeMenu* iconSizeMenu_;

    //! tooltip widget
    PlacesToolTipWidget* toolTipWidget_;

    //! list of items
    QList<PlacesWidgetItem*> items_;

    //! focus item (for context menu actions)
    PlacesWidgetItem* focusItem_;

    //! drag item
    PlacesWidgetItem* dragItem_;

    //! icon provider
    BaseFileIconProvider* iconProvider_;

    //! file system watcher, for local selection frame
    FileSystemWatcher fileSystemWatcher_;

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

    //! add separator
    QAction* addSeparatorAction_;

    //! edit item
    QAction* editItemAction_;

    //! delete item
    QAction* removeItemAction_;

    //! hide item
    QAction* hideItemAction_;

    //! show all entries action
    QAction* showAllEntriesAction_;

    //@}


};

#endif
