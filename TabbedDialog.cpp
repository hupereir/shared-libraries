
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
*
*******************************************************************************/

/*!
  \file TabbedDialog.cpp
  \brief base configuration dialog
  \author Hugo Pereira
  \version $Revision$
  \date $Date$
*/
#include <QApplication>
#include <QScrollArea>
#include <QShortcut>
#include <QToolTip>
#include <QGroupBox>
#include <QHeaderView>
#include <QLayout>
#include <QLabel>

#include "AnimatedStackedWidget.h"
#include "ScrollObject.h"
#include "TabbedDialog.h"
#include "TreeView.h"



//_________________________________________________________
TabbedDialog::TabbedDialog( QWidget* parent ):
  BaseDialog( parent ),
  Counter( "TabbedDialog" )
{

  Debug::Throw( "TabbedDialog::TabbedDialog.\n" );

  QVBoxLayout* layout( new QVBoxLayout() );
  layout->setSpacing(5);
  layout->setMargin(0);
  setLayout( layout );

  QHBoxLayout* h_layout = new QHBoxLayout();
  h_layout->setMargin(0);
  h_layout->setSpacing(5);
  layout->addLayout( h_layout );

  // add widgets
  h_layout->addWidget( list_ = new TreeView( this ), 0 );
  h_layout->addWidget( stack_ = new AnimatedStackedWidget(0), 1 );

  // configure list
  _list().setMaximumWidth(150);
  _list().header()->hide();
  _list().setSortingEnabled( false );
  _list().setModel( &model_ );

  // connections
  connect( _list().selectionModel(), SIGNAL( currentRowChanged( const QModelIndex&, const QModelIndex& ) ), SLOT( _display( const QModelIndex& ) ) );

  // button layout
  button_layout_ = new QBoxLayout( QBoxLayout::LeftToRight );
  button_layout_->setMargin(5);
  button_layout_->setSpacing(5);
  layout->addLayout( button_layout_, 0 );

  // close window shortcut
  connect( new QShortcut( Qt::CTRL+Qt::Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );

}

//______________________________________________________________
TabbedDialog::~TabbedDialog( void )
{
  Debug::Throw( "TabbedDialog::~TabbedDialog.\n" );
  return;
}

//_________________________________________________________
QWidget& TabbedDialog::addPage( const QString& title, const QString& tooltip, const bool& expand )
{

  // base widget
  QWidget* base = new QWidget();
  base->setLayout( new QVBoxLayout() );
  base->layout()->setMargin(0);
  base->layout()->setSpacing(5);
  base->setObjectName( title );

  if( !tooltip.isEmpty() ) {

    QLabel *label( new QLabel( tooltip, base ) );
    QFont font( label->font() );
    font.setWeight( QFont::Bold );
    label->setFont( font );
    label->setMargin(5);
    base->layout()->addWidget( label );

  }

  // create scroll area
  QScrollArea* scroll = new QScrollArea();
  scroll->setWidgetResizable ( true );
  scroll->setFrameStyle( QFrame::NoFrame );

  base->layout()->addWidget( scroll );

  // add smooth scrolling object
  new ScrollObject( scroll );

  // create main widget
  QWidget* main( new QWidget() );
  scroll->setWidget( main );

  // add to stack and model
  _stack().addWidget( base );
  _model().add( base );

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
    layout->addStretch();
    return *contents;
  }

}

//__________________________________________________
void TabbedDialog::_display( const QModelIndex& index )
{
  Debug::Throw( "TabbedDialog::_display.\n" );
  if( !index.isValid() ) return;
  _stack().setCurrentWidget( _model().get( index ) );
}

//_______________________________________________
const QString TabbedDialog::Model::column_titles_[ TabbedDialog::Model::n_columns ] =
{ "" };


//_______________________________________________________________________________________
QVariant TabbedDialog::Model::data( const QModelIndex& index, int role ) const
{

  // check index, role and column
  if( !index.isValid() ) return QVariant();

  // retrieve associated file info
  QWidget& widget( *get()[index.row()] );

  // return text associated to file and column
  if( role == Qt::DisplayRole )
  {

    switch( index.column() )
    {

      case NAME: return widget.objectName();
      default: return QVariant();
    }
  }

  return QVariant();

}
