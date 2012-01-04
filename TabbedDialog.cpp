
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

#include "AnimatedStackedWidget.h"
#include "IconSize.h"
#include "ScrollObject.h"
#include "TabbedDialog.h"

#include <QtGui/QApplication>
#include <QtGui/QScrollArea>
#include <QtGui/QShortcut>
#include <QtGui/QToolTip>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLayout>
#include <QtGui/QLabel>
#include <QtGui/QPainter>

//_________________________________________________________
TabbedDialog::Delegate::Delegate( QObject *parent ):
    QAbstractItemDelegate( parent ),
    Counter( "TabbedDialog::Delegate" ),
    iconSize_( 32 )
{ Debug::Throw( "TabbedDialog::Delegate::Delegate.\n" ); }

//_________________________________________________________
int TabbedDialog::Delegate::_layoutText(QTextLayout *layout, int maxWidth) const
{
    qreal height = 0;
    int textWidth = 0;
    layout->beginLayout();
    while( true )
    {
        QTextLine line = layout->createLine();
        if( !line.isValid() ) break;
        line.setLineWidth(maxWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        textWidth = qMax(textWidth, qRound(line.naturalTextWidth() + 0.5));
    }

    layout->endLayout();
    return textWidth;
}

//_________________________________________________________
void TabbedDialog::Delegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    // check index
    if ( !index.isValid() ) return;

    const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
    const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const QPixmap pixmap = icon.pixmap( iconSize_, iconSize_ );

    QFontMetrics fm = painter->fontMetrics();
    int wp = pixmap.width();
    int hp = pixmap.height();

    QTextLayout iconTextLayout( text, option.font );
    QTextOption textOption( Qt::AlignHCenter );
    iconTextLayout.setTextOption( textOption );
    int maxWidth = qMax( 3 * wp, 8 * fm.height() );
    _layoutText( &iconTextLayout, maxWidth );

    QPen pen = painter->pen();
    QPalette::ColorGroup cg = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;

    if( cg == QPalette::Normal && !(option.state & QStyle::State_Active) )
    { cg = QPalette::Inactive; }

    QStyleOptionViewItemV4 opt(option);
    opt.showDecorationSelected = true;
    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

    if ( option.state & QStyle::State_Selected )
    {
        painter->setPen( option.palette.color( cg, QPalette::HighlightedText ) );

    } else {

        painter->setPen( option.palette.color( cg, QPalette::Text ) );

    }

    if( !pixmap.isNull() )
    { painter->drawPixmap( option.rect.x() + (option.rect.width()/2)-(wp/2), option.rect.y() + 5, pixmap ); }

    if ( !text.isEmpty() )
    { iconTextLayout.draw( painter, QPoint( option.rect.x() + (option.rect.width()/2)-(maxWidth/2), option.rect.y() + hp+7 ) ); }

    painter->setPen( pen );

    _drawFocus( painter, option, option.rect );
}

//_________________________________________________________
QSize TabbedDialog::Delegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    if ( !index.isValid() ) return QSize();

    const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
    const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const QPixmap pixmap = icon.pixmap( iconSize_, iconSize_ );

    QFontMetrics fm = option.fontMetrics;
    int gap = fm.height();
    int hp = 0;
    int wp = iconSize_;

    if( !pixmap.isNull() )
    {
        hp = pixmap.height();
        wp = pixmap.width();
    }

    QTextLayout iconTextLayout( text, option.font );
    int wt = _layoutText( &iconTextLayout, qMax( 3 * wp, 8 * fm.height() ) );
    int ht = iconTextLayout.boundingRect().height();

    int width, height;
    if ( text.isEmpty() ) height = hp;
    else height = hp + ht + 10;

    width = qMax( wt, wp ) + gap;

    return QSize( width, height );
}

//_________________________________________________________
void TabbedDialog::Delegate::_drawFocus( QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const
{
    if (option.state & QStyle::State_HasFocus)
    {

        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(option);
        o.rect = rect;
        o.state |= QStyle::State_KeyboardFocusChange;
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ? QPalette::Normal : QPalette::Disabled;
        o.backgroundColor = option.palette.color( cg, (option.state & QStyle::State_Selected) ? QPalette::Highlight : QPalette::Background );

        QApplication::style()->drawPrimitive( QStyle::PE_FrameFocusRect, &o, painter );

    }

}

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

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(2);
    layout->addLayout( hLayout );

    // add widgets
    hLayout->addWidget( list_ = new QListView( this ), 0 );
    hLayout->addWidget( stack_ = new AnimatedStackedWidget(0), 1 );

    // configure list
    _list().setModel( &model_ );
    _list().setItemDelegate( &delegate_ );
    _list().setFlow( QListView::TopToBottom );


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
    connect( &model_, SIGNAL( layoutChanged() ), this, SLOT( _updateWidth() ) );
    connect( new QShortcut( Qt::CTRL+Qt::Key_Q, this ), SIGNAL( activated() ), SLOT( close() ) );

    _updateWidth();

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
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable ( true );
    scroll->setFrameStyle( QFrame::NoFrame );

    base->layout()->addWidget( scroll );

    // add smooth scrolling object
    new ScrollObject( scroll );

    // create main widget
    QWidget* main( new QWidget() );
    scroll->setWidget( main );

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
void TabbedDialog::_updateWidth()
{

    int width = 0;
    for( int i = 0; i < model_.rowCount(); ++i )
    { width = qMax( width, _list().sizeHintForIndex( model_.index( i, 0 ) ).width() ); }
    _list().setFixedWidth( width + 25 );

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
