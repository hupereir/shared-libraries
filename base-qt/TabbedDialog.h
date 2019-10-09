#ifndef TabbedDialog_h
#define TabbedDialog_h
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

#include "BaseDialog.h"
#include "Counter.h"
#include "ListModel.h"
#include "OptionWidget.h"
#include "XmlOptions.h"

#include <QCloseEvent>
#include <QDialog>
#include <QDialogButtonBox>
#include <QIcon>
#include <QLayout>
#include <QStackedWidget>

class SimpleListView;

//* tabbed dialog
/** a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class TabbedDialog: public BaseDialog, private Base::Counter<TabbedDialog>
{

    Q_OBJECT

    public:

    //* creator
    explicit TabbedDialog( QWidget* );

    //* adds a new Item, returns associated Box
    QWidget& addPage( const QString& title, const QString& tooltip = QString(), bool expand = false )
    { return addPage( QIcon(), title, tooltip, expand ); }

    //* adds a new Item, returns associated Box
    QWidget& addPage( const QIcon&, const QString&, const QString& tooltip = QString(), bool expand = false );

    protected:

    //* retrieve list
    const SimpleListView& _list() const
    { return *list_; }

    //* retrieve list
    SimpleListView& _list()
    { return *list_; }

    //* retrieve stack
    const QStackedWidget& _stackedWidget() const
    { return *stackedWidget_; }

    //* retrieve stack
    QStackedWidget& _stackedWidget()
    { return *stackedWidget_; }

    //* button box
    const QDialogButtonBox& _buttonBox() const
    { return *buttonBox_; }

    //* button box
    QDialogButtonBox& _buttonBox()
    { return *buttonBox_; }

    //* get page matching a given index
    QWidget* _findPage( const QModelIndex& ) const;

    //* clear
    /** this will clear the model and delete all pages */
    void _clear();

    private Q_SLOTS:

    //* display item page
    void _display( const QModelIndex& );

    private:

    //* item model
    class Item final: private Base::Counter<Item>
    {

        public:

        //* constructor
        explicit Item():
            Counter( "TabbedDialog::Item" )
        {}

        //* constructor
        explicit Item( const QString& name, QWidget* widget ):
            Counter( "TabbedDialog::Item" ),
            name_( name ),
            widget_( widget )
        {}

        //* name
        void setName( const QString& name )
        { name_ = name; }

        //* widget
        void setWidget( QWidget* widget )
        { widget_ = widget; }

        //* icon
        void setIcon( const QIcon& icon )
        { icon_ = icon; }

        //* name
        const QString& name() const
        { return name_; }

        //* widget
        QWidget* widget() const
        { return widget_; }

        //* icon
        const QIcon& icon() const
        { return icon_; }

        private:

        //* name
        QString name_;

        //* associated widget
        QWidget* widget_ = nullptr;

        //* icon
        QIcon icon_;


        //* equal to operator
        friend bool operator == (const Item& first, const Item& second)
        { return first.widget_ == second.widget_;  }

        //* less than operator
        friend bool operator < (const Item& first, const Item& second)
        { return first.widget_ < second.widget_; }

    };

    //* model
    class Model: public ListModel<Item>
    {

        public:

        //* column type enumeration
        enum { nColumns = 1 };

        //*@name methods reimplemented from base class
        //@{

        //* flags
        Qt::ItemFlags flags(const QModelIndex& ) const override
        { return Qt::ItemIsEnabled |  Qt::ItemIsSelectable; }

        //* return data
        QVariant data(const QModelIndex &index, int role) const override;

        //* header data
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
        { return QVariant(); }

        //* number of columns for a given index
        int columnCount(const QModelIndex& = QModelIndex()) const override
        { return nColumns; }

        //@}

    };

    //* model
    Model model_;

    //* Configuration list
    SimpleListView* list_ = nullptr;

    //* Widget stack
    QStackedWidget* stackedWidget_ = nullptr;

    //* button layout (needed to add extra buttons)
    QDialogButtonBox* buttonBox_ = nullptr;

};


#endif
