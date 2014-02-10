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

#include "PathEditor.h"
#include "PathEditor.moc"
#include "PathEditor_p.h"

#include "BaseIconNames.h"
#include "BaseFileInfo.h"
#include "Debug.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QEvent>
#include <QTimer>

#include <QApplication>
#include <QDrag>
#include <QLayout>
#include <QListView>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionViewItemV4>
#include <QTextOption>
#include <QToolButton>

//___________________________________________________________________
const double PathEditorButton::BorderWidth = 1.5;
const QString PathEditor::MimeType( "internal/path-editor-item" );

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
void PathEditorItem::setPath( const File& path, const QString& name )
{

    Debug::Throw( "PathEditorItem::setPath.\n" );

    path_ = path;

    if( name.isEmpty() )
    {

        // get local name
        File localName( path.localName() );
        if( localName.endsWith( "/" ) ) localName = localName.left( localName.size()-1 );
        Q_ASSERT( !localName.isEmpty() );

        setText( localName );

    } else setText( name );

    updateMinimumSize();

}

//____________________________________________________________________________
void PathEditorItem::updateMinimumSize( void )
{
    Debug::Throw( "PathEditor::updateMinimumSize.\n" );
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


//_______________________________________________
void PathEditorItem::mousePressEvent( QMouseEvent* event )
{

    Debug::Throw( "PathEditorItem::mousePressEvent.\n" );

    // update focusItem
    if( dragEnabled_ && event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
    {
        dragInProgress_ = true;
        dragOrigin_ = event->pos();
    }

    PathEditorButton::mousePressEvent( event );

}

//_______________________________________________
void PathEditorItem::mouseMoveEvent( QMouseEvent* event )
{

    Debug::Throw( "PathEditorItem::mouseMoveEvent.\n" );
    if( dragEnabled_ && ((event->buttons() & Qt::LeftButton) && dragInProgress_ ) &&
        ( event->pos() - dragOrigin_ ).manhattanLength() >= QApplication::startDragDistance() )
    {

        // start drag
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // fill Drag data. Use XML
        QDomDocument document;
        QDomElement top = document.appendChild( document.createElement( Xml::FILEINFO_LIST ) ).toElement();

        BaseFileInfo fileInfo( path_ );
        fileInfo.setAlias( text() );
        fileInfo.setIsFolder();
        if( isLocal_ )
        {

            fileInfo.setLocal();
            fileInfo.update();
            if( path_.isLink() ) fileInfo.setIsLink();
            if( path_.isBrokenLink() ) fileInfo.setIsBrokenLink();
            if( path_.isHidden() ) fileInfo.setIsHidden();

        } else fileInfo.setRemote();

        top.appendChild( fileInfo.domElement( document ) );

        const QString value( prefix_.isEmpty() ? path_ : prefix_ + "://" + path_ );
        mimeData->setText( value );
        mimeData->setData( PathEditor::MimeType, document.toByteArray() );
        drag->setMimeData( mimeData );

        // create drag pixmap
        QPixmap pixmap( size() );
        pixmap.fill( Qt::transparent );
        QPainter painter( &pixmap );
        _paint( &painter );

        drag->setPixmap( pixmap );
        drag->setHotSpot( dragOrigin_-rect().topLeft() );

        drag->exec( Qt::MoveAction );
        dragInProgress_ = false;

    } else {

        PathEditorButton::mouseMoveEvent( event );

    }

}

//_______________________________________________
void PathEditorItem::mouseReleaseEvent( QMouseEvent* event )
{
    dragInProgress_ = false;
    PathEditorButton::mouseReleaseEvent( event );
}

//____________________________________________________________________________
void PathEditorItem::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    _paint( &painter );
    painter.end();
}

//____________________________________________________________________________
void PathEditorItem::_paint( QPainter* painter )
{

    // render mouse over
    if( _mouseOver() && isSelectable() )
    {

        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.showDecorationSelected = true;
        option.rect = rect();
        option.state |= QStyle::State_MouseOver;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter, _itemView() );

    }

    // save layout direction
    const bool isRightToLeft( qApp->isRightToLeft() );

    // render text
    QRect textRect( rect().adjusted( 0, 2*BorderWidth, 0, -2*BorderWidth ) );
    if( !isLast_ )
    {
        if( isRightToLeft ) textRect.adjust(_arrowWidth()-2*BorderWidth, 0, 0, 0 );
        else textRect.adjust( 0, 0, -_arrowWidth()-2*BorderWidth, 0 );
    }

    QFont adjustedFont(font());
    adjustedFont.setBold( isLast_ );
    painter->setFont( adjustedFont );
    painter->drawText( QRectF( textRect ), text(), QTextOption( Qt::AlignHCenter|Qt::AlignBottom ) );

    // render arrow
    if( !isLast_ )
    {
        QStyleOption option;
        option.initFrom(this);

        if( isRightToLeft ) option.rect = QRect( 0, 0, textRect.left()+BorderWidth, rect().height() );
        else option.rect = QRect( textRect.width(), 0, rect().width()-textRect.width()-BorderWidth, rect().height() );

        option.palette = palette();
        style()->drawPrimitive( isRightToLeft ? QStyle::PE_IndicatorArrowLeft:QStyle::PE_IndicatorArrowRight, &option, painter, this );
    }

}

//____________________________________________________________________________
void PathEditorMenuButton::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    if( _mouseOver() )
    {
        // mouse over
        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.showDecorationSelected = true;
        option.rect = rect();
        option.state |= QStyle::State_MouseOver;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, &painter, _itemView() );

    }

    {
        // arrow
        QStyleOption option;
        option.initFrom(this);
        option.rect = rect();
        option.palette = palette();
        const bool isRightToLeft( qApp->isRightToLeft() );
        style()->drawPrimitive( isRightToLeft ? QStyle::PE_IndicatorArrowLeft:QStyle::PE_IndicatorArrowRight, &option, &painter, this);
    }

    painter.end();

}

//____________________________________________________________________________
void PathEditorMenuButton::updateMinimumSize( void )
{
    Debug::Throw( "PathEditorMenuButton::updateMinimumSize.\n" );
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

    if( _mouseOver() && isEnabled() )
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
    usePrefix_( true ),
    isLocal_( true ),
    truncate_( true ),
    dragEnabled_( false )
{
    Debug::Throw( "PathEditor::PathEditor.\n" );

    // size policy
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );

    // browser
    {

        // some styles require an item view passed to painting method to have proper selection rendered in items
        itemView_ = new QListView( this );
        itemView_->hide();

        browserContainer_ = new QWidget();
        browserContainer_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

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
        hLayout->addWidget( menuButton_ = new PathEditorMenuButton( browserContainer_ ) );
        menuButton_->setItemView( itemView_ );
        menuButton_->hide();
        connect( menuButton_, SIGNAL(clicked()), SLOT(_menuButtonClicked()) );

        // button layout
        hLayout->addLayout( buttonLayout_ = new QHBoxLayout() );
        buttonLayout_->setSpacing(0);
        buttonLayout_->setMargin(0);

        // switch
        PathEditorSwitch* editorSwitch = new PathEditorSwitch( browserContainer_ );
        hLayout->addWidget( editorSwitch, 1 );
        connect( editorSwitch, SIGNAL(clicked()), SLOT(_showEditor()) );

        // button group
        group_ = new QButtonGroup( browserContainer_ );
        group_->setExclusive( false );
        connect( group_, SIGNAL(buttonClicked(QAbstractButton*)), SLOT(_buttonClicked(QAbstractButton*)) );

        addWidget( browserContainer_ );
    }

    // editor
    {
        editorContainer_ = new QWidget();
        editorContainer_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setSpacing(0);
        hLayout->setMargin(0);
        editorContainer_->setLayout( hLayout );

        editor_ = new CustomComboBox( editorContainer_ );
        editor_->setEditable( true );
        hLayout->addWidget( editor_ );

        // ok button
        QToolButton* button = new QToolButton( editorContainer_ );
        button->setIcon( IconEngine::get( IconNames::DialogOk ) );
        button->setAutoRaise( true );
        button->setToolButtonStyle( Qt::ToolButtonIconOnly );
        button->setIconSize( IconSize( IconSize::Small ) );
        button->setFixedSize( QSize( editor_->height(), editor_->height() ) );
        hLayout->addWidget( button );

        button->setFocusPolicy( Qt::StrongFocus );

        connect( editor_->lineEdit(), SIGNAL(returnPressed()), SLOT(_returnPressed()) );
        connect( editor_, SIGNAL(activated(int)), SLOT(_returnPressed()) );
        connect( button, SIGNAL(clicked()), SLOT(_showBrowser()) );

        addWidget( editorContainer_ );
    }

    // current widget
    setCurrentWidget( browserContainer_ );

    // configuration
    connect( Singleton::get().application(), SIGNAL(configurationChanged()), SLOT(_updateConfiguration()) );
    _updateConfiguration();

}

//____________________________________________________________________________
void PathEditor::setPrefix( const QString& value )
{
    Debug::Throw( "PathEditor::setPrefix.\n" );
    if( prefix_ == value ) return;
    prefix_ = value;
    prefixLabel_->setText( prefix_ );

    _updatePrefix();
}

//____________________________________________________________________________
void PathEditor::setHomePath( const File& value )
{
    Debug::Throw( "PathEditor::setHomePath.\n" );
    if( home_ == value ) return;
    home_ = value;
    _reload();
}

//____________________________________________________________________________
void PathEditor::setRootPathList( const File::List& value )
{
    Debug::Throw( "PathEditor::setRootPathList.\n" );
    if( rootPathList_ == value ) return;
    rootPathList_ = value;
    _reload();
}

//____________________________________________________________________________
void PathEditor::setIsLocal( bool value )
{
    if( value == isLocal_ ) return;

    // assign to this widget, and children
    isLocal_ = value;
    foreach( PathEditorItem* item, items_ )
    { item->setIsLocal( value ); }

}

//____________________________________________________________________________
void PathEditor::setDragEnabled( bool value )
{
    if( value == dragEnabled_ ) return;

    // assign to this widget, and children
    dragEnabled_ = value;
    foreach( PathEditorItem* item, items_ )
    { item->setDragEnabled( value ); }

}

//____________________________________________________________________________
void PathEditor::setPath( const File& constPath, const File& file )
{

    Debug::Throw() << "PathEditor::setPath - " << constPath << endl;

    // upbate browser
    {

        // search proper root path
        File root( "/" );
        File path( constPath );

        foreach( const File& file, rootPathList_ )
        {
            if( path.startsWith( file ) )
            {
                // store root file and truncate
                root = file;
                path = path.mid( root.size() );
                break;
            }
        }

        // need to keep focus
        const bool hasFocus( !items_.isEmpty() && items_.back()->hasFocus() );

        // check if home directory is to be used
        const bool hasHome( truncate_ && !home_.isEmpty() );

        // create root item
        int index = 0;
        PathEditorItem* item(0);
        if( index < items_.size() ) {

            item = items_[index];
            item->setIsLast( false );

        } else {

            item = new PathEditorItem( browserContainer_ );
            item->setPrefix( prefix_ );
            item->setIsLocal( isLocal_ );
            item->setItemView( itemView_ );
            item->setDragEnabled( dragEnabled_ );
            group_->addButton( item );
            buttonLayout_->addWidget( item );
            items_ << item;

        }

        if( hasHome && root == home_ ) item->setPath( root, "Home" );
        else item->setPath( root, "Root" );
        index++;

        // create path items
        const int sectionCount( path.split( '/', QString::SkipEmptyParts ).size() );
        for( int i=0; i < sectionCount; i++ )
        {

            // setup section
            QString section = root + path.section( '/', 0, i, QString::SectionSkipEmpty );

            if( index < items_.size() )
            {

                item = items_[index];
                item->setIsLast( false );

            } else {

                item = new PathEditorItem( browserContainer_ );
                item->setPrefix( prefix_ );
                item->setIsLocal( isLocal_ );
                item->setItemView( itemView_ );
                item->setDragEnabled( dragEnabled_ );
                group_->addButton( item );
                buttonLayout_->addWidget( item );
                items_ << item;

            }

            if( hasHome && section == home_ ) item->setPath( section, "Home" );
            else item->setPath( section );
            index++;
        }

        // add file as last item
        if( !file.isEmpty() )
        {

            if( index < items_.size() )
            {

                item = items_[index];
                item->setIsLast( false );

            } else {

                item = new PathEditorItem( browserContainer_ );
                item->setPrefix( prefix_ );
                item->setIsLocal( isLocal_ );
                item->setItemView( itemView_ );
                item->setDragEnabled( dragEnabled_ );
                group_->addButton( item );
                buttonLayout_->addWidget( item );
                items_ << item;

            }

            item->setPath( file.addPath( path ) );
            item->setIsSelectable( false );
            index++;

        }

        // set last item and restore focus
        if( item )
        {
            item->setIsLast( true );
            if( hasFocus ) item->setFocus();
        }

        // delete remaining index
        while( items_.size() > index )
        {
            item = items_.back();
            item->hide();
            group_->removeButton( item );
            item->deleteLater();
            items_.removeLast();
        }

        // update buttons visibility
        _updateButtonVisibility();

    }

    // update editor
    {
        File path( file.isEmpty() ? constPath:file.addPath( constPath ) );
        if( editor_->currentText() != path )
        {

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
{
    if( items_.isEmpty() ) return false;
    else if( items_.back()->isSelectable() ) return items_.size() >= 2;
    else return items_.size() >= 3;
}

//____________________________________________________________________________
QSize PathEditor::minimumSizeHint( void ) const
{
    int minWidth( 0 );

    if( usePrefix_ && !prefix_.isEmpty() )
    { minWidth += prefixLabel_->width(); }

    if( rootPathList_.size() > 1 || items_.size() > 1 || ( truncate_ && !home_.isEmpty() ) )
    { minWidth += menuButton_->width(); }

    if( !items_.empty() )
    { minWidth += items_.back()->width(); }

    return QSize( minWidth, QStackedWidget::minimumSizeHint().height() );
}

//____________________________________________________________________________
void PathEditor::selectParent( void )
{
    if( !hasParent() ) return;
    const File path( (items_.back()->isSelectable() ? items_[items_.size()-2]:items_[items_.size()-3])->path() );
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
    QTimer::singleShot( 0, this, SLOT(_updateButtonVisibility()) );
    QWidget::resizeEvent( event );
}

//____________________________________________________________________________
void PathEditor::_updatePrefix( void )
{

    Debug::Throw( "PathEditor::_updatePrefix.\n" );

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
void PathEditor::_setUseTruncation( bool value )
{
    Debug::Throw( "PathEditor::_setUseTruncation.\n" );
    if( truncate_ == value ) return;
    truncate_ = value;
    _reload();
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

    // get list of hidden buttons
    File::List pathList;
    foreach( PathEditorItem* item, items_ )
    {
        if( item->isHidden() ) pathList << item->path();
        else {

            // also add first non visible path
            pathList << item->path();
            break;

        }
    }

    // check list
    if( pathList.empty() && rootPathList_.size() <= 1 ) return;

    // get copy of current path
    const File currentPath( path() );

    // create menu and fill
    QMenu* menu = new QMenu(browserContainer_);

    if( rootPathList_.size() > 1 )
    {
        // add root path list
        foreach( const File& path, rootPathList_ )
        {

            QAction* action = menu->addAction( path );

            // mark current disk as bold, in case there is no other item hidden
            if( pathList.isEmpty() && currentPath.startsWith( path ) )
            {
                QFont font( action->font() );
                font.setBold( true );
                action->setFont( font );
            }

        }

        // add separator
        if( !pathList.isEmpty() ) menu->addSeparator();
    }

    // add path
    foreach( const File& path, pathList )
    { menu->addAction( path ); }

    connect( menu, SIGNAL(triggered(QAction*)), SLOT(_updatePath(QAction*)) );
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

    // cast button
    PathEditorItem* item( static_cast<PathEditorItem*>( button ) );

    // check selectable test
    if( !item->isSelectable() ) return;

    // retrieve path and emit signal
    const File path( item->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_updateButtonVisibility( void )
{

    Debug::Throw() << "PathEditor::_updateButtonVisibility - path: " << path() << endl;

    // get widget width
    int maxWidth( this->width() );

    // check if prefix must be shown
    if( usePrefix_ && !prefix_.isEmpty() )
    { maxWidth -= prefixLabel_->width(); }

    // check if menu button must be shown for root path list
    bool menuButtonVisible( false );
    if( rootPathList_.size() > 1 )
    {
        menuButtonVisible = true;
        maxWidth -= menuButton_->width();
    }

    // see if some buttons needs hiding
    bool hasHiddenButtons( false );
    bool hasHomePath( false );
    if( truncate_ && !home_.isEmpty() )
    {
        foreach( PathEditorItem* item, items_ )
        {
            if( item->path() == home_ )
            {
                hasHomePath = true;
                hasHiddenButtons = (item != items_.front());
                break;
            }
        }

    }

    // check item width
    if( !hasHiddenButtons )
    {
        int width( 0 );
        foreach( PathEditorItem* item, items_ )
        {
            width += item->sizeHint().width();
            if( width > maxWidth )
            {
                hasHiddenButtons = true;
                break;
            }
        }
    }

    // toggle menu button visibility
    if( hasHiddenButtons && !menuButtonVisible )
    {
        maxWidth -= menuButton_->width();
        menuButtonVisible = true;
    }

    // show menu button if required
    menuButton_->setVisible( menuButtonVisible );

    // effectively hide buttons
    {
        int width = 0;
        bool homeFound( false );
        PathEditorItem::ListIterator iterator( items_ );
        iterator.toBack();
        while( iterator.hasPrevious() )
        {
            // get item
            PathEditorItem* item( iterator.previous() );

            // update width
            width += item->sizeHint().width();
            if( ( width >= maxWidth || homeFound ) && !item->isLast() ) item->hide();
            else item->show();

            // check against home path
            if( hasHomePath && item->path() == home_ ) homeFound = true;

        }
    }

}

//____________________________________________________________________________
void PathEditor::_updateConfiguration( void )
{
    Debug::Throw( "PathEditor::_updateConfiguration.\n" );

    if( XmlOptions::get().contains( "USE_PREFIX" ) )
    { _setUsePrefix( XmlOptions::get().get<bool>( "USE_PREFIX" ) ); }

    if( XmlOptions::get().contains( "USE_TRUNCATION" ) )
    { _setUseTruncation( XmlOptions::get().get<bool>( "USE_TRUNCATION" ) ); }

}
