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

#include "TabbedDialog.h"

#include "IconSize.h"
#include "QtUtil.h"
#include "SimpleListView.h"

#include <QApplication>
#include <QScrollArea>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>


//_________________________________________________________
TabbedDialog::TabbedDialog( QWidget* parent ):
    BaseDialog( parent ),
    Counter( QStringLiteral("TabbedDialog") )
{

    Debug::Throw( QStringLiteral("TabbedDialog::TabbedDialog.\n") );

    auto layout = new QVBoxLayout;
    layout->setMargin(0);
    setLayout( layout );

    // horizontal layout
    auto hLayout = new QHBoxLayout;
    hLayout->setSpacing(0);
    layout->addLayout( hLayout );

    // add widgets
    hLayout->addWidget( list_ = new SimpleListView( this ) );

    auto stackedLayout = new QVBoxLayout;
    hLayout->addLayout( stackedLayout );
    stackedLayout->addWidget( stackedWidget_ = new QStackedWidget(this) );

    // configure list
    list_->setProperty( "_kde_side_panel_view", true );
    list_->setModel( &model_ );

    // button box
    stackedLayout->addWidget( buttonBox_ = new QDialogButtonBox( this ), 0 );
    buttonBox_->layout()->setMargin(5);

    // connections
    connect( list_->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &TabbedDialog::_display );
    connect( new QShortcut( QKeySequence::Quit, this ), &QShortcut::activated, this, &TabbedDialog::close );

}

//_________________________________________________________
QWidget& TabbedDialog::addPage( const QIcon& icon, const QString& title, const QString& tooltip, bool expand )
{

    // base widget
    auto base = new QWidget;
    base->setLayout( new QVBoxLayout );
    base->layout()->setMargin(5);
    base->layout()->setSpacing(10);

    auto hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    base->layout()->addItem( hLayout );

    // tooltip label
    auto label = new QLabel( base );
    label->setText( tooltip.isEmpty() ? title:tooltip );
    label->setMargin(5);

    // update font
    label->setFont( QtUtil::titleFont( label->font() ) );

    // add to layout
    hLayout->addWidget( label, 1 );

    // create scroll area
    auto scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable ( true );
    scrollArea->setFrameStyle( QFrame::NoFrame );

    base->layout()->addWidget( scrollArea );

    // create main widget
    auto main = new QWidget;
    scrollArea->setWidget( main );

    // add to stack
    stackedWidget_->addWidget( base );

    // add to item
    Item item( title, base );
    if( !icon.isNull() ) item.setIcon( icon );
    model_.add( item );

    // set current index
    if( (!list_->selectionModel()->currentIndex().isValid()) && model_.hasIndex(0,0) )
    { list_->selectionModel()->setCurrentIndex( model_.index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows ); }

    auto layout = new QVBoxLayout;
    layout->setSpacing( 5 );
    layout->setMargin( 0 );
    main->setLayout( layout );

    // in expanded mode, the main widget is returned directly
    if( expand ) return *main;
    else {

        // in non-expanded mode (the default)
        // a widget is created inside main, and a stretch is added at the bottom
        // the created widget is return
        auto contents = new QWidget( main );
        contents->setLayout( new QVBoxLayout );
        contents->layout()->setSpacing(5);
        contents->layout()->setMargin(0);

        layout->addWidget( contents );
        layout->addStretch(1);
        return *contents;
    }

}

//__________________________________________________
QWidget* TabbedDialog::_findPage( const QModelIndex& index ) const
{ return index.isValid() ? model_.get( index ).widget():nullptr; }

//__________________________________________________
void TabbedDialog::_clear()
{
    Debug::Throw( QStringLiteral("TabbedDialog::_clear.\n") );
    model_.clear();
    while( stackedWidget_->currentWidget() )
    { delete  stackedWidget_->currentWidget(); }
}

//__________________________________________________
void TabbedDialog::_display( const QModelIndex& index )
{
    Debug::Throw( QStringLiteral("TabbedDialog::_display.\n") );
    if( !index.isValid() ) return;
    stackedWidget_->setCurrentWidget( model_.get( index ).widget() );
}

//_______________________________________________________________________________________
QVariant TabbedDialog::Model::data( const QModelIndex& index, int role ) const
{

    // check index
    if( !contains( index ) ) return QVariant();

    // retrieve associated file info
    Item item( get()[index.row()] );

    // return text associated to file and column
    switch( role )
    {
        case Qt::DisplayRole:
        return item.name();

        case Qt::DecorationRole:
        return item.icon().isNull() ? QVariant():item.icon();

        default: return QVariant();
    }

}
