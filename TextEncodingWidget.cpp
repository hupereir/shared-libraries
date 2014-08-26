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
#include "TextEncodingWidget.moc"

#include "AnimatedLineEditor.h"
#include "AnimatedTreeView.h"
#include "ListModel.h"

#include <QLayout>
#include <QTextCodec>

//! model
class TextEncodingModel: public ListModel<QByteArray>
{

    public:

    //! constructor
    TextEncodingModel( QObject* parent ):
        ListModel<QByteArray>( parent )
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

            return QString( get()[index.row()] );

        } else return QVariant();

    }

    //! header data
    virtual QVariant headerData( int, Qt::Orientation, int = Qt::DisplayRole ) const
    { return QVariant(); }


    //! number of columns for a given index
    virtual int columnCount( const QModelIndex& = QModelIndex() ) const
    { return nColumns; }

    protected:

    //! sort
    virtual void _sort( int column, Qt::SortOrder order = Qt::AscendingOrder )
    {}

    //@}

};

//______________________________________________________________________
TextEncodingWidget::TextEncodingWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "TextEncodingWidget" ),
    model_( new TextEncodingModel( this ) )
{
    // layout
    QVBoxLayout* layout( new QVBoxLayout() );
    layout->setMargin( 0 );
    setLayout( layout );

    layout->addWidget( list_ = new AnimatedTreeView( this ) );
    list_->setSelectionMode( QAbstractItemView::SingleSelection );
    list_->setModel( model_ );

    layout->addWidget( editor_ = new AnimatedLineEditor( this ) );

    _loadTextCodecs();

}

//______________________________________________________________________
void TextEncodingWidget::_loadTextCodecs( void )
{ model_->set( QTextCodec::availableCodecs() ); }
