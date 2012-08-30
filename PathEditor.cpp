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
#include <QtGui/QMenu>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOption>
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QTextOption>
#include <QtGui/QToolButton>

#include <cassert>

//____________________________________________________________________________
bool PathEditorButton::event( QEvent* event )
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
void PathEditorItem::setPath( const File& path )
{

    Debug::Throw( "PathEditorItem::setPath.\n" );

    path_ = path;
    if( QDir( path_ ).isRoot() )
    {

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
void PathEditorItem::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    // render text
    QRect textRect( rect().adjusted( 0, 2*BorderWidth, 0, -2*BorderWidth ) );
    if( !isLast_ ) textRect.adjust( 0, 0, -_arrowWidth()-2*BorderWidth, 0 );

    QFont adjustedFont(font());
    adjustedFont.setBold( isLast_ );
    painter.setFont( adjustedFont );
    painter.drawText( QRectF( textRect ), text(), QTextOption( Qt::AlignHCenter|Qt::AlignBottom ) );

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
    if( _mouseOver() )
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
void PathEditorMenuButton::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOption option;
    option.initFrom(this);
    option.rect = rect();
    option.palette = palette();
    style()->drawPrimitive(QStyle::PE_IndicatorArrowLeft, &option, &painter, this);

    // render mouse over
    if( _mouseOver() )
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
void PathEditorMenuButton::_updateMinimumSize( void )
{
    Debug::Throw( "PathEditorMenuButton::_updateMinimumSize.\n" );
    QFont adjustedFont(font());
    adjustedFont.setBold( true );

    QFontMetrics metrics( adjustedFont );
    QSize size( metrics.height(), metrics.height() );
    size.rwidth() += 2*BorderWidth;
    size.rheight() += 4*BorderWidth;

    // update
    setMinimumSize( size );
}

//________________________________________________________________________
void PathEditorSwitch::paintEvent( QPaintEvent* event )
{

    if( _mouseOver() )
    {
        QPainter painter( this );
        painter.setClipRegion( event->region() );

        painter.setPen( QPen( palette().color( foregroundRole() ), 2 ) );
        painter.setBrush( Qt::NoBrush );
        painter.drawLine( rect().topLeft() + QPoint( 1, 2*BorderWidth ), rect().bottomLeft() + QPoint( 1, -2*BorderWidth ) );
    }

}

//____________________________________________________________________________
PathEditor::PathEditor( QWidget* parent ):
    QStackedWidget( parent ),
    Counter( "PathEditor" ),
    usePrefix_( true )
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

        // prefix label
        prefixLabel_ = new QLabel( browserContainer_ );
        prefixLabel_->setMargin( 2*PathEditorButton::BorderWidth );
        hLayout->addWidget( prefixLabel_ );
        prefixLabel_->hide();

        // menu button
        menuButton_ = new PathEditorMenuButton( browserContainer_ );
        hLayout->addWidget( menuButton_ );
        menuButton_->hide();
        connect( menuButton_, SIGNAL( clicked( void ) ), SLOT( _menuButtonClicked( void ) ) );

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

        button->setFocusPolicy( Qt::StrongFocus );

        connect( editor_->lineEdit(), SIGNAL( returnPressed( void ) ), SLOT( _returnPressed( void ) ) );
        connect( editor_, SIGNAL( activated( int ) ), SLOT( _returnPressed( void ) ) );
        connect( button, SIGNAL( clicked( void ) ), SLOT( _showBrowser( void ) ) );

        addWidget( editorContainer_ );
    }

    // current widget
    setCurrentWidget( browserContainer_ );

}

//____________________________________________________________________________
void PathEditor::setPrefix( const QString& prefix )
{
    if( prefix_ == prefix ) return;
    prefix_ = prefix;
    prefixLabel_->setText( prefix_ );

    _updatePrefix();

}

//____________________________________________________________________________
void PathEditor::setPath( const File& constPath )
{

    Debug::Throw() << "PathEditor::setPath - " << constPath << endl;

    // upbate browser
    {

        PathEditorItem::List items( browserContainer_->findChildren<PathEditorItem*>() );

        // need to keep focus
        const bool hasFocus( !items.isEmpty() && items.back()->hasFocus() );

        int index = 0;
        PathEditorItem* item(0);
        if( index < items.size() ) {

            item = items[index];
            item->setIsLast( false );

        } else {

            item = new PathEditorItem( browserContainer_ );
            group_->addButton( item );
            buttonLayout_->addWidget( item );

        }

        item->setPath( File("/") );
        index++;

        const int sectionCount( constPath.split( '/', QString::SkipEmptyParts ).size() );
        for( int i=0; i < sectionCount; i++ )
        {

            // setup section
            QString section = QString( "/" ) + constPath.section( '/', 0, i, QString::SectionSkipEmpty );

            if( index < items.size() )
            {

                item = items[index];
                item->setIsLast( false );

            } else {

                item = new PathEditorItem( browserContainer_ );
                group_->addButton( item );
                buttonLayout_->addWidget( item );

            }

            item->setPath( section );
            index++;
        }

        // set last item and restore focus
        if( item )
        {
            item->setIsLast( true );
            if( hasFocus ) item->setFocus();
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
        resizeTimer_.start( 0, this );

    }

    // update editor
    if( editor_->currentText() != constPath )
    {

        File path( constPath );
        const bool usePrefix( usePrefix_ && !prefix_.isEmpty() );
        const QString prefix( prefix_+"//" );
        if( usePrefix && !path.startsWith( prefix ) ) path.prepend( prefix );

        int id( editor_->findText( path ) );
        if( id < 0 )
        {
            editor_->addItem( path );
            id = editor_->findText( path );
        }

        editor_->setCurrentIndex( id );

    }

    // add to history
    history_.add( constPath );

}

//____________________________________________________________________________
File PathEditor::path( void ) const
{
    Debug::Throw( "PathEditor::path.\n" );
    QString path( editor_->currentText() );
    if( !prefix_.isEmpty() )
    {
        const QString prefix( prefix_+"//" );
        if( path.startsWith( prefix ) )
        { path = path.mid( prefix.size() ); }
    }

    return path;
}

//____________________________________________________________________________
bool PathEditor::hasParent( void ) const
{ return browserContainer_->findChildren<PathEditorItem*>().size() >= 2; }

//____________________________________________________________________________
void PathEditor::selectParent( void )
{
    PathEditorItem::List items( browserContainer_->findChildren<PathEditorItem*>() );
    if( items.size() < 2 ) return;
    const File path( items[items.size()-2]->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::selectPrevious( void )
{
    Debug::Throw( "PathEditor::selectPrevious.\n" );
    if( !hasPrevious() ) return;
    const File path( history_.previous() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::selectNext( void )
{
    Debug::Throw( "PathEditor::selectNext.\n" );
    if( !hasNext() ) return;
    const File path( history_.next() );
    setPath( path );
    emit pathChanged( path );
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
void PathEditor::_updatePrefix( void )
{

    // use prefix
    const bool usePrefix( usePrefix_ && !prefix_.isEmpty() );

    // update label visibility
    prefixLabel_->setVisible( usePrefix );

    // update combobox
    // get a copy of current path
    const File path( this->path() );
    if( !path.isEmpty() || editor_->QComboBox::count() > 0 )
    {

        const QString prefix( prefix_+"//" );

        // get list of editor items
        QStringList items;
        for( int index = 0; index < editor_->QComboBox::count(); ++index )
        {
            QString item( editor_->itemText( index ) );
            if( item.trimmed().isEmpty() ) continue;

            if( usePrefix ) { if( !item.startsWith( prefix ) ) item.prepend( prefix ); }
            else if( item.startsWith( prefix ) ) item = item.mid( prefix.size() );

            if( item.trimmed().isEmpty() ) continue;
            items << item;
        }

        editor_->clear();
        editor_->addItems( items );
        setPath( path );
    }

}

//____________________________________________________________________________
void PathEditor::_returnPressed( void )
{
    const File path( this->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_menuButtonClicked( void )
{

    // list of path
    QStringList pathList;

    // get list of hidden buttons
    foreach( PathEditorItem* item, browserContainer_->findChildren<PathEditorItem*>() )
    { if( item->isHidden() ) pathList << item->path(); }

    // check list
    if( pathList.empty() ) return;

    // create menu and fill
    QMenu* menu = new QMenu(browserContainer_);
    foreach( const QString& path, pathList )
    { menu->addAction( path ); }

    connect( menu, SIGNAL( triggered( QAction* ) ), SLOT( _updatePath( QAction* ) ) );
    menu->exec( menuButton_->mapToGlobal( menuButton_->rect().bottomLeft() ) );
    static_cast<PathEditorButton*>(menuButton_)->setMouseOver( false );

}

//____________________________________________________________________________
void PathEditor::_updatePath( QAction* action )
{
    File path( action->text() );
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

    Debug::Throw() << "PathEditor::_updateButtonVisibility - path: " << path() << endl;

    // get widget width
    // int maxWidth( this->width() );
    int maxWidth( browserContainer_->width() );

    if( usePrefix_ && !prefix_.isEmpty() )
    { maxWidth -= prefixLabel_->width(); }

    // loop over items backward
    PathEditorItem::ListIterator iterator( browserContainer_->findChildren<PathEditorItem*>() );

    // see if some buttons needs hiding
    bool hasHiddenButton( false );
    int width( 0 );
    while( iterator.hasNext() )
    {
        width += iterator.next()->sizeHint().width();
        if( width > maxWidth )
        {
            hasHiddenButton = true;
            maxWidth -= menuButton_->width();
            break;
        }
    }

    // toggle menu button visibility
    menuButton_->setVisible( hasHiddenButton );

    // effectively hide buttons
    width = 0;
    iterator.toBack();
    while( iterator.hasPrevious() )
    {
        PathEditorItem* item( iterator.previous() );
        width += item->sizeHint().width();
        if( width >= maxWidth ) item->hide();
        else item->show();
    }

}
