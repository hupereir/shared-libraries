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
#include "base_qt_export.h"

#include <QCloseEvent>
#include <QDialog>
#include <QDialogButtonBox>
#include <QIcon>
#include <QLayout>
#include <QStackedWidget>

class SimpleListView;

namespace Private
{
    class TabbedDialogModel;
}

//* tabbed dialog
/** a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class BASE_QT_EXPORT TabbedDialog: public BaseDialog, private Base::Counter<TabbedDialog>
{

    Q_OBJECT

    public:

    //* creator
    explicit TabbedDialog( QWidget* );

    //*@name accessors
    //@{

    //* button box
    QDialogButtonBox& buttonBox() const
    { return *buttonBox_; }

    //* retrieve button layout
    QBoxLayout& buttonLayout() const
    { return *(qobject_cast<QBoxLayout*>(buttonBox_->layout())); }

    //* returns true if OK button is valid
    bool hasOkButton() const
    { return buttonBox_->button(QDialogButtonBox::Ok)||buttonBox_->button(QDialogButtonBox::Close); }

    //* retrieve OK button
    QPushButton& okButton() const
    {
        if( QPushButton* button = buttonBox_->button(QDialogButtonBox::Ok) ) return *button;
        else return *buttonBox_->button(QDialogButtonBox::Close);
    }

    //* returns true if close button is valid
    bool hasCloseButton() const
    { return hasOkButton(); }

    //* retrieve close button
    QPushButton& closeButton() const
    { return okButton(); }

    //* returns true if Cancel button is valid
    bool hasCancelButton() const
    { return buttonBox_->button(QDialogButtonBox::Cancel); }

    //* retrieve CANCEL button
    QPushButton& cancelButton() const
    { return *buttonBox_->button(QDialogButtonBox::Cancel); }

    //@}

    //*@name modifiers
    //@{

    //* adds a new Item, returns associated Box
    QWidget& addPage( const QString& title, const QString& tooltip = QString(), bool expand = false )
    { return addPage( QIcon(), title, tooltip, expand ); }

    //* adds a new Item, returns associated Box
    QWidget& addPage( const QIcon&, const QString&, const QString& tooltip = QString(), bool expand = false );

    //@}

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

    //* get page matching a given index
    QWidget* _findPage( const QModelIndex& ) const;

    //* clear
    /** this will clear the model and delete all pages */
    void _clear();

    private:

    //* display item page
    void _display( const QModelIndex& );

    //* reorder items using drag and drop from the list
    void _reorder( int, int );

    //* model
    Private::TabbedDialogModel* model_ = nullptr;

    //* Configuration list
    SimpleListView* list_ = nullptr;

    //* Widget stack
    QStackedWidget* stackedWidget_ = nullptr;

    //* button layout (needed to add extra buttons)
    QDialogButtonBox* buttonBox_ = nullptr;

};


#endif
