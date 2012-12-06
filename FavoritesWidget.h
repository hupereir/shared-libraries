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
    void setIconProvider( BaseFileIconProvider* );

    //! true if empty
    bool isEmpty( void ) const
    { return group_->buttons().isEmpty(); }

    public slots:

    //! clear all items
    void clear( void );

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

    //! emmited when contents is changed
    void contentsChanged( void );

    protected slots:

    //! button clicked
    void _buttonClicked( QAbstractButton* );

    protected:

    //! Read fileList from file
    virtual bool _read( void );

    //! write fileList to file
    virtual bool _write( void );

    //! set db file
    virtual bool _setDBFile( const File& );

    //! list of items
    QList<FavoritesWidgetItem*> _items( void ) const;

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    //! save configuration
    void _saveConfiguration( void );

    private:

    //! dummy treeview, needed for rendering items
    QAbstractItemView* itemView_;

    //! button layout
    QBoxLayout* buttonLayout_;

    //! button group
    QButtonGroup* group_;

    //! icon provider
    BaseFileIconProvider* iconProvider_;

    //! file from/to wich the files are saved
    File dbFile_;

};

#endif
