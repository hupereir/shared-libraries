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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "TextEncodingWidget.h"

#include "AnimatedLineEditor.h"
#include "AnimatedTreeView.h"
#include "ListModel.h"
#include "TextEncodingString.h"
#include "TextSelection.h"

#include <QHeaderView>
#include <QLayout>
#include <QTextCodec>

#include <algorithm>


//! model
class TextEncodingModel: public ListModel<TextEncodingString>
{

    public:

    //! constructor
    TextEncodingModel( QObject* parent ):
        ListModel<TextEncodingString>( parent )
        {}

    //! column type enumeration
    enum ColumnType
    {
        Name,
        nColumns
    };

    //!@name methods reimplemented from base class
    //@{

    // return data for a given index
    virtual QVariant data( const QModelIndex& index, int role ) const
    {

        // check index, role and column
        if( index.isValid() && role == Qt::DisplayRole )
        {

            return get()[index.row()];

        } else return QVariant();

    }

    //! header data
    virtual QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const
    { return QVariant(); }


    //! number of columns for a given index
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const
    { return nColumns; }

    //! used to sort FileInfo objects
    class SortFTor: public ItemModel::SortFTor
    {

        public:

        //! constructor
        SortFTor( const int& type, Qt::SortOrder order = Qt::AscendingOrder ):
            ItemModel::SortFTor( type, order )
            {}

        //! destructor
        virtual ~SortFTor( void )
        {}

        //! prediction
        virtual bool operator() ( const TextEncodingString& constFirst, const TextEncodingString& constSecond ) const
        {
            TextEncodingString first( constFirst );
            TextEncodingString second( constSecond );
            if( order_ == Qt::DescendingOrder ) std::swap( first, second );
            return first < second;
        }

    };

    protected:

    //! sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    { std::sort( _get().begin(), _get().end(), SortFTor( (ColumnType) column, order ) ); }

};

//______________________________________________________________________
TextEncodingWidget::TextEncodingWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "TextEncodingWidget" ),
    model_( new TextEncodingModel( this ) ),
    locked_( false )
{
    // layout
    QVBoxLayout* layout( new QVBoxLayout() );
    layout->setMargin( 0 );
    setLayout( layout );

    layout->addWidget( list_ = new AnimatedTreeView( this ) );
    list_->setSelectionMode( QAbstractItemView::SingleSelection );
    list_->setModel( model_ );
    list_->header()->hide();

    layout->addWidget( editor_ = new AnimatedLineEditor( this ) );

    // connections
    connect( editor_, SIGNAL(textChanged(QString)), SLOT(_find(QString)) );
    connect( list_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(_updateSelection()) );
    connect( list_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(_updateSelection()) );
    connect( list_, SIGNAL(clicked(QModelIndex)), SLOT(_updateSelection()) );

    _loadTextCodecs();

}

//______________________________________________________________________
void TextEncodingWidget::setEncoding( const QByteArray& constValue )
{

    Debug::Throw() << "TextEncodingMenu::setEncoding - encoding: " << constValue << endl;

    // find codec matching value and check
    QTextCodec* codec( QTextCodec::codecForName( constValue ) );
    if( !codec ) return;

    // get 'standard name'
    QByteArray value( codec->name() );

    QModelIndex index( model_->index( value ) );
    list_->selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
    list_->selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    editor_->setText( value );

}

//______________________________________________________________________
void TextEncodingWidget::_loadTextCodecs( void )
{

    // use MIBs to get rid of aliases
    const QList<int> codecIds( QTextCodec::availableMibs() );
    QList<TextEncodingString> codecStrings;
    foreach( const int& codecId, codecIds )
    {
        QTextCodec* codec( QTextCodec::codecForMib( codecId ) );
        if( codec ) codecStrings.append( codec->name() );
    }

    model_->set( codecStrings );

}

//______________________________________________________________________
void TextEncodingWidget::_updateSelection( void )
{

    // do nothing if locked
    if( locked_ ) return;

    // get current index
    QModelIndex current( list_->selectionModel()->currentIndex() );
    if( !current.isValid() ) return;
    const TextEncodingString& codec( model_->get(current) );
    editor_->setText( codec );

}

//______________________________________________________________________
void TextEncodingWidget::_find(QString text )
{
    TextSelection selection( text );
    selection.setFlag( TextSelection::BeginOfWord, true );
    selection.setFlag( TextSelection::NoIncrement, true );
    locked_ = true;
    list_->find( selection );
    locked_ = false;
}
