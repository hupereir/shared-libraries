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

#include "LineEditor.h"
#include "ListModel.h"
#include "TextEncodingString.h"
#include "TextSelection.h"
#include "TreeView.h"

#include <QHeaderView>
#include <QLayout>
#include <QTextCodec>

#include <algorithm>

class TextEncodingModel: public ListModel<TextEncodingString>
{

    Q_OBJECT

    public:

    //! constructor
    explicit TextEncodingModel( QObject* parent ):
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

            return get()[index.row()].get();

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
        explicit SortFTor( int type, Qt::SortOrder order = Qt::AscendingOrder ):
            ItemModel::SortFTor( type, order )
            {}

        //! destructor
        virtual ~SortFTor()
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
    Counter( QStringLiteral("TextEncodingWidget") ),
    model_( new TextEncodingModel( this ) )
{
    // layout
    QVBoxLayout* layout( new QVBoxLayout );
    layout->setMargin( 0 );
    setLayout( layout );

    layout->addWidget( list_ = new TreeView( this ) );
    list_->setSelectionMode( QAbstractItemView::SingleSelection );
    list_->setModel( model_ );
    list_->header()->hide();

    layout->addWidget( editor_ = new LineEditor( this ) );

    // connections
    connect( editor_, &QLineEdit::textChanged, this, &TextEncodingWidget::_find );
    connect( list_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &TextEncodingWidget::_updateSelection );
    connect( list_->selectionModel(), &QItemSelectionModel::currentChanged, this, &TextEncodingWidget::_updateSelection );
    connect( list_, &QAbstractItemView::clicked, this, &TextEncodingWidget::_updateSelection );

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

    QModelIndex index( model_->index( TextEncodingString( value ) ) );
    list_->selectionModel()->select( index,  QItemSelectionModel::Clear|QItemSelectionModel::Select|QItemSelectionModel::Rows );
    list_->selectionModel()->setCurrentIndex( index,  QItemSelectionModel::Current|QItemSelectionModel::Rows );
    editor_->setText( value );

}

//______________________________________________________________________
void TextEncodingWidget::_loadTextCodecs()
{

    // use MIBs to get rid of aliases
    QList<TextEncodingString> codecStrings;
    for( const auto& codecId:QTextCodec::availableMibs() )
    {
        if( auto codec = QTextCodec::codecForMib( codecId ) )
        { codecStrings.append( TextEncodingString( codec->name() ) ); }
    }

    model_->set( codecStrings );

}

//______________________________________________________________________
void TextEncodingWidget::_updateSelection()
{

    // do nothing if locked
    if( locked_ ) return;

    // get current index
    QModelIndex current( list_->selectionModel()->currentIndex() );
    if( !current.isValid() ) return;
    const TextEncodingString& codec( model_->get(current) );
    editor_->setText( codec.get() );

}

//______________________________________________________________________
void TextEncodingWidget::_find(const QString &text )
{
    TextSelection selection( text );
    selection.setFlag( TextSelection::BeginOfWord, true );
    selection.setFlag( TextSelection::NoIncrement, true );
    locked_ = true;
    list_->find( selection );
    locked_ = false;
}

#include "TextEncodingWidget.moc"
