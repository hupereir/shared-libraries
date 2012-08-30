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

#include "SimpleListView.h"

#include <QtGui/QAbstractItemDelegate>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionViewItem>
#include <QtGui/QTextLayout>

//! item delegate
class Delegate : public QAbstractItemDelegate, public Counter
{

    public:

    //! constructor
    Delegate( QObject* = 0 );

    //! destructor
    virtual ~Delegate( void )
    {}

    //! paint
    virtual void paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;

    //! size
    virtual QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    private:

    //! layout text
    int _layoutText(QTextLayout*, int) const;

    //! focus
    void _drawFocus( QPainter*, const QStyleOptionViewItem&, const QRect& ) const;

    //! icon size
    int iconSize_;

};


#include <QtGui/QApplication>

//_________________________________________________________
Delegate::Delegate( QObject *parent ):
    QAbstractItemDelegate( parent ),
    Counter( "Delegate" ),
    iconSize_( 32 )
{ Debug::Throw( "Delegate::Delegate.\n" ); }

//_________________________________________________________
int Delegate::_layoutText(QTextLayout *layout, int maxWidth) const
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
void Delegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{

    // check index
    if ( !index.isValid() ) return;

    const QString text = index.model()->data( index, Qt::DisplayRole ).toString();
    const QIcon icon = index.model()->data( index, Qt::DecorationRole ).value<QIcon>();
    const QPixmap pixmap = icon.pixmap( iconSize_, iconSize_ );
    const QPen oldPen = painter->pen();

    QFontMetrics fm = painter->fontMetrics();
    int wp = pixmap.width();
    int hp = pixmap.height();

    QTextLayout iconTextLayout( text, option.font );
    QTextOption textOption( Qt::AlignHCenter );
    iconTextLayout.setTextOption( textOption );
    int maxWidth = qMax( 3 * wp, 8 * fm.height() );
    _layoutText( &iconTextLayout, maxWidth );

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

    // restore pen
    painter->setPen( oldPen );

    _drawFocus( painter, option, option.rect );
}

//_________________________________________________________
QSize Delegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
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

    width = qMax( wt, wp ) + 2*gap;

    return QSize( width, height );
}

//_________________________________________________________
void Delegate::_drawFocus( QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect ) const
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
SimpleListView::SimpleListView( QWidget* parent ):
    TreeView( parent )
{
    setSortingEnabled( false );
    setItemDelegate( new Delegate( this ) );

    // change font
    QFont boldFont( font() );
    boldFont.setBold( true );
    setFont( boldFont );
}

//_________________________________________________________
void SimpleListView::setModel( QAbstractItemModel* model )
{
    TreeView::setModel( model );
    if( model )
    {
        // show only first column and hide header
        setMask( 1 );
        header()->hide();

        // connect model modification to resize
        connect( model, SIGNAL( layoutChanged() ), this, SLOT( _adjustWidth() ) );
        _adjustWidth();
    }
}

//_______________________________________________
void SimpleListView::_adjustWidth()
{
    if( !model() ) return;

    int width = 0;
    for( int i = 0; i < model()->rowCount(); ++i )
    { width = qMax( width, sizeHintForIndex( model()->index( i, 0 ) ).width() ); }
    setFixedWidth( width + 25 );

}
