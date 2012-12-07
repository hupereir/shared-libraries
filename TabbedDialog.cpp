
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

#include "AnimatedStackedWidget.h"
#include "IconSize.h"
#include "ScrollObject.h"
#include "SimpleListView.h"
#include "TabbedDialog.h"

#include <QtGui/QApplication>
#include <QtGui/QScrollArea>
#include <QtGui/QShortcut>
#include <QtGui/QToolTip>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPainter>


//_________________________________________________________
TabbedDialog::TabbedDialog( QWidget* parent ):
    BaseDialog( parent ),
    Counter( "TabbedDialog" )
{

    Debug::Throw( "TabbedDialog::TabbedDialog.\n" );

    QVBoxLayout* layout( new QVBoxLayout() );
    layout->setMargin(0);
    layout->setSpacing(2);
    setLayout( layout );

    // horizontal layout
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    layout->addLayout( hLayout );

    // add widgets
    hLayout->addWidget( list_ = new SimpleListView( this ), 0 );
    hLayout->addWidget( stack_ = new AnimatedStackedWidget(this), 1 );

    // configure list
    _list().setModel( &model_ );

    // change font
    QFont boldFont( _list().font() );
    boldFont.setBold( true );
    _list().setFont( boldFont );

    // button layout
    buttonLayout_ = new QBoxLayout( QBoxLayout::LeftToRight );
    buttonLayout_->setMargin(5);
    buttonLayout_->setSpacing(5);
    layout->addLayout( buttonLayout_, 0 );

    // connections
    connect( _list().selectionModel(), SIGNAL( currentRowChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( _display( const QModelIndex& ) ) );
    connect( new QShortcut( QKeySequence::Quit, this ), SIGNAL( activated() ), SLOT( close() ) );


}

//______________________________________________________________
TabbedDialog::~TabbedDialog( void )
{
    Debug::Throw( "TabbedDialog::~TabbedDialog.\n" );
    return;
}

//_________________________________________________________
QWidget& TabbedDialog::addPage( const QIcon& icon, const QString& title, const QString& tooltip, const bool& expand )
{

    // base widget
    QWidget* base = new QWidget();
    base->setLayout( new QVBoxLayout() );
    base->layout()->setMargin(0);
    base->layout()->setSpacing(0);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    base->layout()->addItem( hLayout );

    // tooltip label
    QLabel *label( new QLabel( base ) );
    label->setText( tooltip.isEmpty() ? title:tooltip );
    QFont font( label->font() );
    font.setWeight( QFont::Bold );
    label->setFont( font );
    label->setMargin(5);
    hLayout->addWidget( label, 1 );

    if( !icon.isNull() )
    {

        // change label alignment
        label->setAlignment( Qt::AlignLeft|Qt::AlignTop );

        // add icon
        label = new QLabel( base );
        label->setMargin(5);
        label->setPixmap( icon.pixmap( IconSize( IconSize::Medium ) ) );
        label->setAlignment( Qt::AlignRight|Qt::AlignTop );
        hLayout->addWidget( label, 0 );

    }

    // create scroll area
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable ( true );
    scrollArea->setFrameStyle( QFrame::NoFrame );

    base->layout()->addWidget( scrollArea );

    // add smooth scrolling object
    new ScrollObject( scrollArea );

    // create main widget
    QWidget* main( new QWidget() );
    scrollArea->setWidget( main );

    // add to stack
    _stack().addWidget( base );

    // add to item
    Item item( title, base );
    if( !icon.isNull() ) item.setIcon( icon );
    _model().add( item );

    // set current index
    if( (!_list().selectionModel()->currentIndex().isValid()) && _model().hasIndex(0,0) )
    { _list().selectionModel()->setCurrentIndex( _model().index(0,0), QItemSelectionModel::SelectCurrent | QItemSelectionModel::Rows ); }

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
    _stack().setCurrentWidget( _model().get( index ).widget() );
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
    if( index.column() == NAME )
    {

        switch( role )
        {
            case Qt::DisplayRole:
            return item.name();

            case Qt::DecorationRole:
            return item.icon().isNull() ? QVariant():item.icon();

            default: return QVariant();
        }

    } else return QVariant();

}
