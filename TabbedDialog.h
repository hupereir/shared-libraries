#ifndef TabbedDialog_h
#define TabbedDialog_h

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

#include "BaseDialog.h"
#include "Counter.h"
#include "ListModel.h"
#include "OptionWidget.h"
#include "XmlOptions.h"

#include <QtGui/QCloseEvent>
#include <QtGui/QDialog>
#include <QtGui/QIcon>
#include <QtGui/QLayout>

class AnimatedStackedWidget;
class TreeView;

//! tabbed dialog
/*! a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class TabbedDialog: public BaseDialog, public Counter
{

    Q_OBJECT

    public:

    //! creator
    TabbedDialog( QWidget *parent );

    //! destructor
    virtual ~TabbedDialog();

    //! adds a new Item, returns associated Box
    virtual QWidget& addPage( const QString& title, const QString& tooltip = QString(), const bool& expand = false )
    { return addPage( QIcon(), title, tooltip, expand ); }

    //! adds a new Item, returns associated Box
    virtual QWidget& addPage( const QIcon&, const QString&, const QString& tooltip = QString(), const bool& expand = false );

    protected slots:

    //! display item page
    virtual void _display( const QModelIndex& );

    protected:

    //! retrieve list
    virtual TreeView& _list( void )
    { return *list_; }

    //! retrieve stack
    virtual AnimatedStackedWidget& _stack( void )
    { return *stack_; }

    //! button layout
    QBoxLayout& _buttonLayout( void ) const
    { return *buttonLayout_; }

    private:

    //! item model
    class Item: public Counter
    {

        public:

        //! constructor
        Item( void ):
            Counter( "TabbedDialog::Item" ),
            widget_( 0L )
        {}

        //! constructor
        Item( const QString& name, QWidget* widget ):
            Counter( "TabbedDialog::Item" ),
            name_( name ),
            widget_( widget )
        {}

        //! name
        void setName( const QString& name )
        { name_ = name; }

        //! widget
        void setWidget( QWidget* widget )
        { widget_ = widget; }

        //! icon
        void setIcon( const QIcon& icon )
        { icon_ = icon; }

        //! name
        const QString& name( void ) const
        { return name_; }

        //! widget
        QWidget* widget( void ) const
        { return widget_; }

        //! icon
        const QIcon& icon( void ) const
        { return icon_; }

        //! equal to operator
        bool operator == (const Item& other ) const
        { return widget_ == other.widget_;  }

        //! less than operator
        bool operator < (const Item& other ) const
        { return widget_ < other.widget_; }

        private:

        //! name
        QString name_;

        //! associated widget
        QWidget* widget_;

        //! icon
        QIcon icon_;

    };

    //! model
    class Model: public ListModel<Item>
    {

        public:

        //! number of columns
        enum { nColumns = 1 };

        //! column type enumeration
        enum ColumnType {
            NAME
        };

        //!@name methods reimplemented from base class
        //@{

        //! flags
        virtual Qt::ItemFlags flags(const QModelIndex& ) const
        { return Qt::ItemIsEnabled |  Qt::ItemIsSelectable; }

        //! return data
        virtual QVariant data(const QModelIndex &index, int role) const;

        //! header data
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
        {
            if( orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < nColumns )
            { return columnTitles_[section]; }

            // return empty
            return QVariant();

        }

        //! number of columns for a given index
        virtual int columnCount(const QModelIndex &parent = QModelIndex()) const
        { return nColumns; }

        //@}

        protected:

        //! sort
        virtual void _sort( int, Qt::SortOrder order = Qt::AscendingOrder )
        {}

        //! list column names
        static const QString columnTitles_[nColumns];

    };

    //! model
    Model& _model( void )
    { return model_; }

    //! model
    Model model_;

    //! Configuration list
    TreeView* list_;

    //! Widget stack
    AnimatedStackedWidget* stack_;

    //! button layout (needed to add extra buttons)
    QBoxLayout *buttonLayout_;

};


#endif
