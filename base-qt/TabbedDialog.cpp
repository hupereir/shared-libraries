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
    Counter( "TabbedDialog" )
{

    Debug::Throw( "TabbedDialog::TabbedDialog.\n" );

    QVBoxLayout* layout( new QVBoxLayout() );
    layout->setMargin(0);
    setLayout( layout );

    // horizontal layout
    QHBoxLayout* hLayout = new QHBoxLayout();
    layout->addLayout( hLayout );

    // add widgets
    hLayout->addWidget( list_ = new SimpleListView( this ) );
    hLayout->addWidget( stackedWidget_ = new QStackedWidget(this) );

    // configure list
    list_->setProperty( "_kde_side_panel_view", true );
    list_->setModel( &model_ );

    // button box
    layout->addWidget( buttonBox_ = new QDialogButtonBox( this ), 0 );
    buttonBox_->layout()->setMargin(5);

    // connections
    connect( list_->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), SLOT(_display(QModelIndex)) );
    connect( new QShortcut( QKeySequence::Quit, this ), SIGNAL(activated()), SLOT(close()) );


}

//_________________________________________________________
QWidget& TabbedDialog::addPage( const QIcon& icon, const QString& title, const QString& tooltip, bool expand )
{

    // base widget
    QWidget* base = new QWidget();
    base->setLayout( new QVBoxLayout() );
    base->layout()->setMargin(0);
    base->layout()->setSpacing(10);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    base->layout()->addItem( hLayout );

    // tooltip label
    QLabel *label( new QLabel( base ) );
    label->setText( tooltip.isEmpty() ? title:tooltip );
    label->setMargin(5);

    // update font
    label->setFont( QtUtil::titleFont( label->font() ) );

    // add to layout
    hLayout->addWidget( label, 1 );

    // create scroll area
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable ( true );
    scrollArea->setFrameStyle( QFrame::NoFrame );

    base->layout()->addWidget( scrollArea );

    // create main widget
    QWidget* main( new QWidget() );
    scrollArea->setWidget( main );

    // add to stack
    stackedWidget_->addWidget( base );

    // add to item
    Item item( title, base );
    if( !icon.isNull() ) item.setIcon( icon );
    _model().add( item );

    // set current index
    if( (!list_->selectionModel()->currentIndex().isValid()) && _model().hasIndex(0,0) )
    { list_->selectionModel()->setCurrentIndex( _model().index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows ); }

    QVBoxLayout* layout( new QVBoxLayout() );
    layout->setSpacing( 5 );
    layout->setMargin( 0 );
    main->setLayout( layout );

    // in expanded mode, the main widget is returned directly
    if( expand ) return *main;
    else {

        // in non-expanded mode (the default)
        // a widget is created inside main, and a stretch is added at the bottom
        // the created widget is return
        QWidget* contents( new QWidget( main ) );
        contents->setLayout( new QVBoxLayout() );
        contents->layout()->setSpacing(5);
        contents->layout()->setMargin(0);

        layout->addWidget( contents );
        layout->addStretch(1);
        return *contents;
    }

}

//__________________________________________________
void TabbedDialog::_display( const QModelIndex& index )
{
    Debug::Throw( "TabbedDialog::_display.\n" );
    if( !index.isValid() ) return;
    stackedWidget_->setCurrentWidget( _model().get( index ).widget() );
}

//_______________________________________________
const QString TabbedDialog::Model::columnTitles_[ TabbedDialog::Model::nColumns ] =
{ "" };


//_______________________________________________________________________________________
QVariant TabbedDialog::Model::data( const QModelIndex& index, int role ) const
{

    // check index, role and column
    if( !index.isValid() ) return QVariant();

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