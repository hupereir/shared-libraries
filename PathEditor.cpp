// $Id$

/*******************************************************************************
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

#include "PathEditor.h"
#include "PathEditor_p.h"

#include "BaseIcons.h"
#include "Debug.h"
#include "IconEngine.h"
#include "IconSize.h"

#include <QtCore/QDir>
#include <QtCore/QEvent>

#include <QtGui/QLayout>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QTextOption>
#include <QtGui/QToolButton>

#include <cassert>

//____________________________________________________________________________
void PathEditorItem::setPath( const File& path )
{

    Debug::Throw( "PathEditorItem::setPath.\n" );

    path_ = path;
    if( QDir( path_ ).isRoot() )
    {

        isRoot_ = true;
        setText( "Root" );

    } else {

        // get local name
        File localName( path.localName() );
        if( localName.endsWith( "/" ) ) localName = localName.left( localName.size()-1 );
        assert( !localName.isEmpty() );

        setText( localName );

    }

    _updateMinimumSize();

}

//____________________________________________________________________________
void PathEditorItem::_updateMinimumSize( void )
{
    Debug::Throw( "PathEditor::_updateMinimumSize.\n" );
    QFont adjustedFont(font());
    adjustedFont.setBold( isLast_ );

    // text size
    QSize size( QFontMetrics( adjustedFont ).boundingRect( text() ).size() );

    // margins
    size.rwidth() += 2*BorderWidth;
    size.rheight() += 4*BorderWidth;

    // arrow width
    const int arrowWidth( _arrowWidth() );
    if( arrowWidth > 0 ) size.rwidth() += arrowWidth + 2*BorderWidth;

    // update
    setMinimumSize( size );
}

//____________________________________________________________________________
bool PathEditorItem::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::HoverEnter: mouseOver_ = true; break;
        case QEvent::HoverLeave: mouseOver_ = false; break;
        default: break;
    }

    return QAbstractButton::event( event );

}

//____________________________________________________________________________
void PathEditorItem::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    // render text
    QRect textRect( rect() );
    if( !isLast_ ) textRect.adjust( 0, 0, -_arrowWidth()-2*BorderWidth, 0 );

    QFont adjustedFont(font());
    adjustedFont.setBold( isLast_ );
    painter.setFont( adjustedFont );
    painter.drawText( QRectF( textRect ), text(), QTextOption( Qt::AlignCenter ) );

    // render arrow
    if( !isLast_ )
    {
        QStyleOption option;
        option.initFrom(this);
        option.rect = QRect( textRect.width(), 0, rect().width()-textRect.width()-BorderWidth, rect().height() );
        option.palette = palette();
        style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &option, &painter, this);
    }

    // render mouse over
    if( mouseOver_ )
    {

        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.rect = rect();
        option.state |= QStyle::State_MouseOver;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, &painter, this );

    }

    painter.end();

}

//____________________________________________________________________________
bool PathEditorSwitch::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::HoverEnter: mouseOver_ = true; break;
        case QEvent::HoverLeave: mouseOver_ = false; break;
        default: break;
    }

    return QAbstractButton::event( event );

}

//____________________________________________________________________________
void PathEditorSwitch::paintEvent( QPaintEvent* event )
{

    if( mouseOver_ )
    {
        QPainter painter( this );
        painter.setClipRegion( event->region() );

        painter.setPen( QPen( palette().color( foregroundRole() ), 2 ) );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( rect().topLeft() + QPoint( 1, 2*PathEditorItem::BorderWidth ), rect().bottomLeft() + QPoint( 1, -2*PathEditorItem::BorderWidth ) );
    }

}

//____________________________________________________________________________
PathEditor::PathEditor( QWidget* parent ):
    QStackedWidget( parent ),
    Counter( "PathEditor" )
{
    Debug::Throw( "PathEditor::PathEditor.\n" );

    // size policy
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    // browser
    {
        browserContainer_ = new QWidget();
        browserContainer_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setSpacing(0);
        hLayout->setMargin(0);
        browserContainer_->setLayout( hLayout );

        // button layout
        hLayout->addLayout( buttonLayout_ = new QHBoxLayout() );
        buttonLayout_->setSpacing(0);
        buttonLayout_->setMargin(0);

        // switch
        PathEditorSwitch* editorSwitch = new PathEditorSwitch( browserContainer_ );
        hLayout->addWidget( editorSwitch, 1 );
        connect( editorSwitch, SIGNAL( clicked( void ) ), SLOT( _showEditor( void ) ) );

        // button group
        group_ = new QButtonGroup( browserContainer_ );
        group_->setExclusive( false );
        connect( group_, SIGNAL( buttonClicked( QAbstractButton* ) ), SLOT( _buttonClicked( QAbstractButton* ) ) );

        addWidget( browserContainer_ );
    }

    // editor
    {
        editorContainer_ = new QWidget();
        editorContainer_->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setSpacing(0);
        hLayout->setMargin(0);
        editorContainer_->setLayout( hLayout );

        editor_ = new CustomComboBox( editorContainer_ );
        editor_->setEditable( true );
        hLayout->addWidget( editor_ );

        // ok button
        QToolButton* button = new QToolButton( editorContainer_ );
        button->setIcon( IconEngine::get( ICONS::DIALOG_OK ) );
        button->setAutoRaise( true );
        button->setToolButtonStyle( Qt::ToolButtonIconOnly );
        button->setIconSize( IconSize( IconSize::Small ) );
        button->setFixedSize( QSize( editor_->height(), editor_->height() ) );
        hLayout->addWidget( button );

        connect( editor_->lineEdit(), SIGNAL( returnPressed( void ) ), SLOT( _returnPressed( void ) ) );
        connect( editor_, SIGNAL( activated( int ) ), SLOT( _returnPressed( void ) ) );
        connect( button, SIGNAL( clicked( void ) ), SLOT( _showBrowser( void ) ) );

        addWidget( editorContainer_ );
    }

    // current widget
    setCurrentWidget( browserContainer_ );

}

//____________________________________________________________________________
void PathEditor::setPath( const File& path )
{

    Debug::Throw( "PathEditor::setPath.\n" );


    // upbate browser
    {

        browserContainer_->setUpdatesEnabled( false );
        PathEditorItem::List items( browserContainer_->findChildren<PathEditorItem*>() );

        int index = 0;
        PathEditorItem* item(0);
        if( index < items.size() ) item = items[index];
        else {
            item = new PathEditorItem( browserContainer_ );
            group_->addButton( item );
            buttonLayout_->addWidget( item );
        }

        item->setPath( File("/") );
        index++;

        const int sectionCount( path.split( '/', QString::SkipEmptyParts ).size() );
        for( int i=0; i < sectionCount; i++ )
        {

            // setup section
            QString section = QString( "/" ) + path.section( '/', 0, i, QString::SectionSkipEmpty );

            if( index < items.size() ) item = items[index];
            else {
                item = new PathEditorItem( browserContainer_ );
                group_->addButton( item );
                buttonLayout_->addWidget( item );
            }

            item->setPath( section );
            item->setIsLast( i == sectionCount - 1 );
            index++;
        }

        // delete remaining index
        while( items.size() > index )
        {
            item = items.back();
            item->hide();
            group_->removeButton( item );
            item->deleteLater();
            items.removeLast();
        }

        // update buttons visibility
        _updateButtonVisibility();
        browserContainer_->setUpdatesEnabled( true );

    }

    // update editor
    if( editor_->currentText() != path )
    {
        int id( editor_->findText( path ) );
        if( id < 0 )
        {
            editor_->addItem( path );
            id = editor_->findText( path );
        }

        editor_->setCurrentIndex( id );

    }

}

//____________________________________________________________________________
File PathEditor::path( void ) const
{
    Debug::Throw( "PathEditor::path.\n" );
    return editor_->currentText();
}

//____________________________________________________________________________
void PathEditor::resizeEvent( QResizeEvent* event )
{
    resizeTimer_.start( 0, this );
    QWidget::resizeEvent( event );
}

//____________________________________________________________________________
void PathEditor::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == resizeTimer_.timerId() )
    {
        resizeTimer_.stop();
        _updateButtonVisibility();
    } else return QWidget::timerEvent( event );
}

//____________________________________________________________________________
void PathEditor::_returnPressed( void )
{
    const File path( editor_->currentText() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_buttonClicked( QAbstractButton* button )
{
    const File path( static_cast<PathEditorItem*>( button )->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_updateButtonVisibility( void )
{
    Debug::Throw( "PathEditor::_updateButtonVisibility.\n" );

    // get widget width
    int maxWidth( this->width() );
    int width( 0 );

    // loop over items backward
    PathEditorItem::ListIterator iterator( browserContainer_->findChildren<PathEditorItem*>() );
    iterator.toBack();
    while( iterator.hasPrevious() )
    {
        PathEditorItem* item( iterator.previous() );
        width += item->sizeHint().width();
        if( width >= maxWidth ) item->hide();
        else item->show();
    }

}
