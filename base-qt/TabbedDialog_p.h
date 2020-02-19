#ifndef TabbedDialog_p_h
#define TabbedDialog_p_h
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

namespace Private
{
    //* item model
    class TabbedDialogItem final: private Base::Counter<TabbedDialogItem>
    {

        public:

        //* constructor
        explicit TabbedDialogItem():
            Counter( QStringLiteral("TabbedDialogItem") )
        {}

        //* constructor
        explicit TabbedDialogItem( const QString& name, QWidget* widget ):
            Counter( QStringLiteral("TabbedDialogItem") ),
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

        //* name
        QString name_;

        //* associated widget
        QWidget* widget_ = nullptr;

        //* icon
        QIcon icon_;


        //* equal to operator
        friend bool operator == (const TabbedDialogItem& first, const TabbedDialogItem& second)
        { return first.widget_ == second.widget_;  }

        //* less than operator
        friend bool operator < (const TabbedDialogItem& first, const TabbedDialogItem& second)
        { return first.widget_ < second.widget_; }

    };

    //* model
    class TabbedDialogModel: public ListModel<TabbedDialogItem>
    {

        Q_OBJECT

        public:

        explicit TabbedDialogModel( QObject* parent ):
        ListModel( parent )
        {}

        //* column type enumeration
        enum { nColumns = 1 };

        //*@name methods reimplemented from base class
        //@{

        //* flags
        Qt::ItemFlags flags(const QModelIndex& index ) const override
        {
            if( !index.isValid() ) return Qt::ItemIsDropEnabled;
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
        }

        //* return data
        QVariant data(const QModelIndex &index, int role) const override;

        //* header data
        QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
        { return QVariant(); }

        //* number of columns for a given index
        int columnCount(const QModelIndex& = QModelIndex()) const override
        { return nColumns; }

        //* mime type
        QStringList mimeTypes() const override;

        //* mime data
        QMimeData* mimeData( const QModelIndexList& ) const override;

        //* drop mine data
        bool dropMimeData(const QMimeData*, Qt::DropAction, int, int, const QModelIndex&) override;

        //@}

        Q_SIGNALS:

        //* emitted when an item is dragged in the list
        void itemOrderChanged( int, int);

    };

}

#endif
