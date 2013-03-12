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
*******************************************************************************/

#include "PlacesWidget.h"
#include "PlacesWidget_p.h"

#include "BaseFileIconProvider.h"
#include "BaseContextMenu.h"
#include "BaseIcons.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "IconSizeMenu.h"
#include "PathEditor.h"
#include "Singleton.h"
#include "Util.h"
#include "WarningDialog.h"

#include "XmlDocument.h"
#include "XmlOptions.h"

#include <QApplication>
#include <QDrag>
#include <QListView>
#include <QMimeData>
#include <QPainter>
#include <QUrl>

const double PlacesWidgetItem::BorderWidth = 2;

namespace XML
{ static const QString NAME = "name"; };

//___________________________________________________________________
const QString PlacesWidgetItem::MimeType( "internal/places-widget-item" );

//___________________________________________________________________
PlacesWidgetItem::PlacesWidgetItem( QWidget* parent ):
    QAbstractButton( parent ),
    itemView_( 0x0 ),
    isSeparator_( false ),
    valid_( true ),
    mouseOver_( false ),
    hasFocus_( false ),
    dragEnabled_( false ),
    dragInProgress_( false )
{
    Debug::Throw( "PathEditorItem::PathEditorItem.\n" );
    setAttribute( Qt::WA_Hover );

    // configuration
    connect( Singleton::get().application(), SIGNAL(configurationChanged() ), SLOT( _updateConfiguration() ) );
    _updateConfiguration();

}

//___________________________________________________________________
void PlacesWidgetItem::updateMinimumSize( void )
{

    // separators
    if( isSeparator_ )
    {
        setMinimumSize( QSize( 0, 3 ) );
        return;
    }

    // get icon size
    QSize size( 2*BorderWidth, 2*BorderWidth );

    // icon
    if( !icon().isNull() )
    {
        size.rwidth() += iconSize().width();
        size.rheight() = qMax<int>( size.height(), iconSize().height() + 2*BorderWidth );
    }

    // text
    if( !text().isEmpty() )
    {
        QSize textSize = fontMetrics().boundingRect( text() ).size();
        size.rwidth() += textSize.width();
        size.rheight() = qMax<int>( size.height(), textSize.height() + 2*BorderWidth );

        if( !icon().isNull() ) size.rwidth() += 2*BorderWidth;

    }

    // store
    setMinimumSize( size );

}

//___________________________________________________________________
bool PlacesWidgetItem::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::HoverEnter: mouseOver_ = true; break;
        case QEvent::HoverLeave: mouseOver_ = false; break;
        default: break;
    }

    return QAbstractButton::event( event );

}

//_______________________________________________
void PlacesWidgetItem::mousePressEvent( QMouseEvent* event )
{

    Debug::Throw( "PlacesWidgetItem::mousePressEvent.\n" );

    // update focusItem
    if( dragEnabled_ && event->button() == Qt::LeftButton && event->modifiers() == Qt::NoModifier)
    {
        dragInProgress_ = true;
        dragOrigin_ = event->pos();
    }

    QAbstractButton::mousePressEvent( event );

}

//_______________________________________________
void PlacesWidgetItem::mouseMoveEvent( QMouseEvent* event )
{

    Debug::Throw( "PlacesWidgetItem::mouseMoveEvent.\n" );
    if( dragEnabled_ && ((event->buttons() & Qt::LeftButton) && dragInProgress_ ) &&
        ( event->pos() - dragOrigin_ ).manhattanLength() >= QApplication::startDragDistance() )
    {

        // start drag
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        mimeData->setData( PlacesWidgetItem::MimeType, 0x0 );
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

        QAbstractButton::mouseMoveEvent( event );

    }

}

//_______________________________________________
void PlacesWidgetItem::mouseReleaseEvent( QMouseEvent* event )
{
    dragInProgress_ = false;
    QAbstractButton::mouseReleaseEvent( event );
}

//___________________________________________________________________
void PlacesWidgetItem::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );
    _paint( &painter );
    painter.end();
}

//___________________________________________________________________
void PlacesWidgetItem::_paint( QPainter* painter )
{

    // render separators
    if( isSeparator_ )
    {

        QStyleOptionFrameV3 option;
        option.initFrom( this );
        option.frameShape = QFrame::HLine;
        style()->drawControl( QStyle::CE_ShapedFrame, &option, painter, this );
        return;

    }

    // render mouse over
    if( valid_ && ( mouseOver_ || hasFocus_ || dragInProgress_ ) )
    {

        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.showDecorationSelected = true;
        option.rect = rect();
        if( mouseOver_ ) option.state |= QStyle::State_MouseOver;
        if( hasFocus_ || dragInProgress_ ) option.state |= QStyle::State_Selected;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter, itemView_ );

    }

    // save layout direction
    const bool isRightToLeft( qApp->isRightToLeft() );

    // render text
    if( !text().isEmpty() )
    {

        QRect textRect( rect().adjusted( BorderWidth, BorderWidth, -BorderWidth, -BorderWidth ) );
        if( !icon().isNull() )
        {

            if( isRightToLeft ) textRect.setRight( textRect.right() - iconSize().width() - 2*BorderWidth );
            else textRect.setLeft( textRect.left() + iconSize().width() + 2*BorderWidth );

        }

        // draw text
        painter->setFont( font() );

        // change text color if focus
        if( valid_ && ( hasFocus_ || dragInProgress_ ) ) painter->setPen( palette().color( QPalette::HighlightedText ) );
        else painter->setPen( palette().color( valid_ ? QPalette::Normal:QPalette::Disabled, QPalette::WindowText  ) );

        QTextOption textOptions( Qt::AlignVCenter | (isRightToLeft ? Qt::AlignRight : Qt::AlignLeft ) );
        textOptions.setWrapMode( QTextOption::NoWrap );
        painter->drawText( QRectF( textRect ), text(), textOptions );

    }

    // render icon
    if( !icon().isNull() )
    {
        QRect iconRect( rect().adjusted( BorderWidth, BorderWidth, -BorderWidth, -BorderWidth ) );
        if( !text().isEmpty() )
        {
            if( isRightToLeft ) iconRect.setLeft( iconRect.right() - iconSize().width() );
            else iconRect.setRight( iconRect.left() + iconSize().width() );
        }

        // get pixmap
        const QPixmap pixmap( icon().pixmap( iconSize(), isValid() ? QIcon::Normal:QIcon::Disabled ) );
        const QPoint position(
            iconRect.x() + 0.5*(iconRect.width() - pixmap.width()),
            iconRect.y() + 0.5*(iconRect.height() - pixmap.height()) );

        painter->drawPixmap( position, pixmap );

    }

}

//___________________________________________________________________
void PlacesWidgetItem::_updateConfiguration( void )
{

    // icon size
    if( XmlOptions::get().contains( "PLACES_ICON_SIZE" ) )
    {
        int iconSize( XmlOptions::get().get<int>( "PLACES_ICON_SIZE" ) );
        if( iconSize > 0 )
        {
            setIconSize( QSize( iconSize, iconSize ) );
            updateMinimumSize();
        }
    }

}

//___________________________________________________________________
PlacesWidgetItemDialog::PlacesWidgetItemDialog( QWidget* parent ):
    CustomDialog( parent, OkButton|CancelButton|Separator )
{
    // layout
    GridLayout *layout = new GridLayout();
    layout->setMargin(0);
    layout->setMaxCount(2);
    layout->setColumnAlignment( 0, Qt::AlignVCenter|Qt::AlignRight );

    mainLayout().addLayout( layout );

    QLabel* label;
    layout->addWidget( label = new QLabel( tr("Label:"), this ) );
    layout->addWidget( nameEditor_ = new AnimatedLineEditor( this ) );
    label->setBuddy( nameEditor_ );

    nameEditor_->setPlaceholderText( tr("Enter descriptive label here") );

    layout->addWidget( label = new QLabel( tr("Location:"), this ) );
    layout->addWidget( fileEditor_ = new BrowsedLineEditor( this ) );
    label->setBuddy( fileEditor_ );
    fileEditor_->setAcceptMode( QFileDialog::AcceptOpen );
    fileEditor_->setFileMode( QFileDialog::Directory );

    layout->addWidget( remoteCheckBox_ = new QCheckBox( tr("Remote location"), this ), 2, 1, 1, 1 );

}

//___________________________________________________________________
LocalFileInfo::LocalFileInfo( const QDomElement& element ):
    BaseFileInfo( element )
{
    Debug::Throw( "LocalFileInfo::LocalFileInfo (dom).\n" );

    // parse attributes
    QDomNamedNodeMap attributes( element.attributes() );
    for( unsigned int i=0; i<attributes.length(); i++ )
    {

        QDomAttr attribute( attributes.item( i ).toAttr() );
        if( attribute.isNull() ) continue;
        if( attribute.name() == XML::NAME ) name_ = attribute.value();
    }

}

//________________________________________________________________
QDomElement LocalFileInfo::domElement( QDomDocument& document ) const
{

    Debug::Throw( "BaseFileInfo::DomElement.\n" );
    QDomElement out( BaseFileInfo::domElement( document ) );
    out.setAttribute( XML::NAME, name_ );
    return out;
}

//___________________________________________________________________
PlacesWidget::PlacesWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "PlacesWidget::PlacesWidget" ),
    iconProvider_( 0x0 ),
    focusItem_( 0x0 ),
    dragItem_( 0x0 ),
    dragInProgress_( false )

{

    Debug::Throw( "PlacesWidget::PlacesWidget.\n" );

    // some styles require an item view passed to painting method to have proper selection rendered in items
    itemView_ = new QListView( this );
    itemView_->hide();

    // accept drops
    setAcceptDrops( true );

    // actions
    _installActions();

    // main layout
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin(2);
    vLayout->setSpacing(0);
    setLayout( vLayout );

    // button layout
    buttonLayout_ = new QVBoxLayout();
    buttonLayout_->setMargin(0);
    buttonLayout_->setSpacing(0);

    vLayout->addLayout( buttonLayout_ );
    vLayout->addStretch( 1 );

    // button group
    group_ = new QButtonGroup( this );
    group_->setExclusive( false );
    connect( group_, SIGNAL(buttonClicked( QAbstractButton* ) ), SLOT( _buttonClicked( QAbstractButton* ) ) );
    connect( group_, SIGNAL(buttonPressed( QAbstractButton* ) ), SLOT( _updateFocus( QAbstractButton* ) ) );

    // context menu
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( _updateContextMenu( const QPoint& ) ) );

    // icon sizes
    iconSizeMenu_ = new IconSizeMenu( this );
    connect( iconSizeMenu_, SIGNAL(iconSizeSelected( IconSize::Size ) ), SLOT( _updateIconSize( IconSize::Size ) ) );

    // configuration
    connect( Singleton::get().application(), SIGNAL(configurationChanged() ), SLOT( _updateConfiguration() ) );
    connect( qApp, SIGNAL(aboutToQuit() ), SLOT( _saveConfiguration() ) );
    _updateConfiguration();

}

//______________________________________________________________________
void PlacesWidget::setIconProvider( BaseFileIconProvider* provider )
{

    Debug::Throw( "PlacesWidget::setIconProvider.\n" );

    // set provider
    iconProvider_ = provider;

    // update icons for existing items
    foreach( PlacesWidgetItem* item, items_ )
    {
        if( item->icon().isNull() )
        { item->setIcon( iconProvider_->icon( item->fileInfo() ) ); }
    }

}

//______________________________________________________________________
QList<BaseFileInfo> PlacesWidget::items( void ) const
{
    Debug::Throw( "PlacesWidget::items.\n" );
    QList<BaseFileInfo> out;
    foreach( PlacesWidgetItem* item, items_ )
    { out.append( item->fileInfo() );  }

    return out;
}

//______________________________________________________________________
void PlacesWidget::setItemIsValid( const BaseFileInfo& fileInfo, bool value )
{
    Debug::Throw() << "PlacesWidget::setItemIsValid - fileInfo: " << fileInfo << " value: " << value << endl;
    foreach( PlacesWidgetItem* item, items_ )
    {

        if( item->fileInfo().file() == fileInfo.file() && item->fileInfo().location() == fileInfo.location() )
        { item->setIsValid( value ); }

    }
}

//______________________________________________________________________
bool PlacesWidget::read( File file )
{
    Debug::Throw( "PlacesWidget::read.\n" );

    // clear existing entries
    clear();

    if( file.isEmpty() ) file = dbFile_;
    if( file.isEmpty() || !file.exists() ) return false;

    // dom document
    XmlDocument document;
    {

        QFile qfile( file );
        if( !document.setContent( &qfile ) )
        {
            Debug::Throw() << document.error() << endl;
            return false;
        }
    }

    // look for relevant element
    QDomNodeList topNodes = document.elementsByTagName( XML::FILEINFO_LIST );
    if( topNodes.isEmpty() ) return false;

    // read records
    for(QDomNode node = topNodes.at(0).firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == XML::FILEINFO )
        {

            LocalFileInfo fileInfo( element );
            if( !fileInfo.file().isEmpty() ) add( fileInfo.name(), fileInfo );
            else _addSeparator();

        }
    }

    return true;
}

//______________________________________________________________________
bool PlacesWidget::write( File file )
{
    Debug::Throw( "PlacesWidget::write.\n" );
    if( file.isEmpty() ) file = dbFile_;
    if( file.isEmpty() )
    {
        Debug::Throw( "PlacesWidget::write - no file.\n" );
        return false;
    }

    // create document
    XmlDocument document;
    {
        QFile qtfile( file );
        document.setContent( &qtfile );
    }

    // get list of items
    QList<PlacesWidgetItem*> items( items_ );

    // create main element
    QDomElement top = document.createElement( XML::FILEINFO_LIST );

    // loop over records
    foreach( PlacesWidgetItem* item, items )
    {

        const BaseFileInfo& fileInfo( item->fileInfo() );
        Debug::Throw() << "PlacesWidget::write - " << fileInfo << endl;

        top.appendChild( LocalFileInfo( fileInfo, item->text() ).domElement( document ) );
    }


    // append top node to document and write
    document.replaceChild( top );
    {
        QFile qfile( file );
        if( !qfile.open( QIODevice::WriteOnly ) ) return false;
        qfile.write( document.toByteArray() );
    }

    return true;
}

//______________________________________________________________________
void PlacesWidget::clear( void )
{
    // delete all items
    foreach( PlacesWidgetItem* item, items_ )
    {
        // remove from group and delete later
        group_->removeButton( item );
        item->hide();
        item->deleteLater();
    }

    // clear items
    items_.clear();

    // emit signal
    emit contentsChanged();

}

//______________________________________________________________________
void PlacesWidget::add( const BaseFileInfo& fileInfo )
{ add( iconProvider_ ? iconProvider_->icon( fileInfo ):QIcon(), fileInfo.file().localName(), fileInfo ); }

//______________________________________________________________________
void PlacesWidget::add( const QString& name, const BaseFileInfo& fileInfo )
{ add( iconProvider_ ? iconProvider_->icon( fileInfo ):QIcon(), name, fileInfo ); }

//___________________________________________________________________
void PlacesWidget::add( const QIcon& icon, const QString& name, const BaseFileInfo& fileInfo )
{

    Debug::Throw( "PlacesWidget::add.\n" );

    // create new item
    PlacesWidgetItem* item = new PlacesWidgetItem( this );
    item->setIcon( icon );
    item->setText( name );
    item->setFileInfo( fileInfo );
    item->setItemView( itemView_ );

    // add to button group, list of items and layout
    group_->addButton( item );
    buttonLayout_->addWidget( item );
    items_.append( item );

    // emit signal
    _updateDragState();
    emit contentsChanged();

}

//______________________________________________________________________
void PlacesWidget::insert( int position, const BaseFileInfo& fileInfo )
{
    const QString alias( fileInfo.hasAlias() ? fileInfo.alias():fileInfo.file().localName() );
    insert( position, iconProvider_ ? iconProvider_->icon( fileInfo ):QIcon(), alias, fileInfo );
}

//______________________________________________________________________
void PlacesWidget::insert( int position, const QString& name, const BaseFileInfo& fileInfo )
{ insert( position, iconProvider_ ? iconProvider_->icon( fileInfo ):QIcon(), name, fileInfo ); }

//___________________________________________________________________
void PlacesWidget::insert( int position, const QIcon& icon, const QString& constName, const BaseFileInfo& fileInfo )
{

    Debug::Throw( "PlacesWidget::insert.\n" );

    QString name( constName );
    if( name.isEmpty() ) name = "Root";

    if( position >= items_.size() )
    {
        add( icon, name, fileInfo );
        return;
    }

    if( position < 0 ) position = 0;

    // create new item
    PlacesWidgetItem* item = new PlacesWidgetItem( this );
    item->setIcon( icon );
    item->setText( name );
    item->setFileInfo( fileInfo );
    item->setItemView( itemView_ );

    // add to button group, list of items and layout
    group_->addButton( item );
    buttonLayout_->insertWidget( position, item );
    items_.insert( position, item );

    // emit signal
    _updateDragState();
    emit contentsChanged();

}

//___________________________________________________________________
void PlacesWidget::_buttonClicked( QAbstractButton* button )
{

    Debug::Throw( "PlacesWidget::_buttonClicked.\n" );

    PlacesWidgetItem* currentItem( qobject_cast<PlacesWidgetItem*>( button ) );
    if( currentItem && !currentItem->isSeparator() ) emit itemSelected( currentItem->fileInfo() );

}

//___________________________________________________________________
void PlacesWidget::_updateFocus( QAbstractButton* button )
{

    Debug::Throw( "PlacesWidget::_updateFocus.\n" );

    // cas button to item
    PlacesWidgetItem* currentItem( qobject_cast<PlacesWidgetItem*>( button ) );
    if( currentItem ) currentItem->setFocus( true );

    // disable focus for all other buttons
    foreach( PlacesWidgetItem* item, items_ )
    { if( item != currentItem ) item->setFocus( false ); }

}

//______________________________________________________________________
void PlacesWidget::_updateContextMenu( const QPoint& position )
{

    Debug::Throw( "PlacesWidget::_updateContextMenu.\n" );

    // store drag target, for new item insertion
    dragTarget_ = _updateDragTarget( position );

    // create menu
    BaseContextMenu menu( this );
    menu.setHideDisabledActions( true );

    // add entry
    menu.addAction( addItemAction_ );
    menu.addAction( addSeparatorAction_ );
    menu.addSeparator();

    if( ( focusItem_ = _focusItem() ) )
    {
        if( !focusItem_->isSeparator() )
        {
            editItemAction_->setText( QString( tr( "Edit '%1'" ) ).arg( focusItem_->text() ) );
            menu.addAction( editItemAction_ );
        }

        {
            if( focusItem_->isSeparator() ) removeItemAction_->setText( tr( "Remove Separator" ) );
            else removeItemAction_->setText( QString( tr( "Remove '%1'" ) ).arg( focusItem_->text() ) );
            menu.addAction( removeItemAction_ );
        }

        menu.addSeparator();

    }

    // icon sizes
    menu.addMenu( iconSizeMenu_ );

    // execute
    menu.exec( mapToGlobal( position ) );
}

//______________________________________________________________________
void PlacesWidget::_updateIconSize( IconSize::Size size )
{
    Debug::Throw() << "PlacesWidget::_updateIconSize - size: " << size << endl;

    // save in options
    XmlOptions::get().set<int>( "PLACES_ICON_SIZE", size );

    // update button sizes
    QSize iconSize = size==IconSize::Default ? PlacesWidgetItem().iconSize() : IconSize( size );
    foreach( PlacesWidgetItem* item, items_ )
    {
        item->setIconSize( iconSize );
        item->updateMinimumSize();
    }

}

//______________________________________________________________________
void PlacesWidget::_addItem( void )
{
    Debug::Throw( "PlacesWidget::_addItem.\n" );

    PlacesWidgetItemDialog dialog( this );
    dialog.setWindowTitle( QString( tr( "Add Places Entry - %1" ) ).arg( qApp->applicationName() ) );
    dialog.setOptionName( "EDIT_PLACES_ITEM_DIALOG" );

    // assign starting point
    BaseFileInfo currentFileInfo;
    if( focusItem_ ) currentFileInfo = focusItem_->fileInfo();
    else {

        currentFileInfo = BaseFileInfo( Util::home(), BaseFileInfo::Folder );
        currentFileInfo.update();

    }

    dialog.setFile( currentFileInfo );

    if( !dialog.exec() ) return;

    // create new item
    const File file( dialog.file() );
    BaseFileInfo fileInfo( file );
    if( dialog.isRemote() )
    {
        fileInfo.setRemote();
        fileInfo.setIsFolder();

    } else {

        fileInfo.update();
        fileInfo.setLocal();

        if( file.isDirectory() ) fileInfo.setIsFolder();
        else fileInfo.setIsDocument();
        if( file.isLink() ) fileInfo.setIsLink();
        if( file.isBrokenLink() ) fileInfo.setIsBrokenLink();
        if( file.isHidden() ) fileInfo.setIsHidden();

    }

    // check file info
    if( fileInfo.isDocument() || fileInfo.isBrokenLink() || (fileInfo.isLocal() && !fileInfo.file().exists() ) )
    {
        WarningDialog( this, tr( "Places item is invalid. <Add Entry> canceled." ) ).exec();
        return;
    }

    // item name
    QString name( dialog.name() );
    if( name.isEmpty() ) name = fileInfo.file().localName();

    // add
    insert( _index( dragTarget_ ), name, fileInfo );

    return;

}

//______________________________________________________________________
void PlacesWidget::_addSeparator( void )
{
    Debug::Throw( "PlacesWidget::_addSeparator.\n" );

    // add new item
    PlacesWidgetItem* item = new PlacesWidgetItem( this );
    item->setIsSeparator( true );

    // add to button group, list of items and layout
    group_->addButton( item );
    buttonLayout_->addWidget( item );
    items_.append( item );

    // emit signal
    _updateDragState();
    emit contentsChanged();

}

//______________________________________________________________________
void PlacesWidget::_insertSeparator( void )
{
    Debug::Throw( "PlacesWidget::_insertSeparator.\n" );

    // add new item
    PlacesWidgetItem* item = new PlacesWidgetItem( this );
    item->setIsSeparator( true );

    // add to button group, list of items and layout
    group_->addButton( item );

    int position( _index( dragTarget_ ) );
    if( position > items_.size() )
    {

        buttonLayout_->addWidget( item );
        items_.append( item );

    } else {

        if( position < 0 ) position = 0;
        buttonLayout_->insertWidget( position, item );
        items_.insert( position, item );

    }

    // emit signal
    _updateDragState();
    emit contentsChanged();

}

//______________________________________________________________________
void PlacesWidget::_editItem( void )
{
    Debug::Throw( "PlacesWidget::_editItem.\n" );
    if( !focusItem_ ) return;

    PlacesWidgetItemDialog dialog( this );
    dialog.setWindowTitle( QString( tr( "Edit Places Entry - %1" ) ).arg( qApp->applicationName() ) );
    dialog.setOptionName( "EDIT_PLACES_ITEM_DIALOG" );
    dialog.setName( focusItem_->text() );
    dialog.setFile( focusItem_->fileInfo() );

    if( !dialog.exec() ) return;

    // remote flag
    BaseFileInfo fileInfo( focusItem_->fileInfo() );
    const bool locationChanged( fileInfo.isRemote() != dialog.isRemote() );
    if( locationChanged )
    {
        if( dialog.isRemote() ) fileInfo.setRemote();
        else fileInfo.setLocal();
    }

    // file
    const File file( dialog.file() );
    const bool fileChanged( fileInfo.file() != file );
    if( fileChanged ) fileInfo.setFile( dialog.file() );

    // check
    if( locationChanged || fileChanged )
    {

        if( fileInfo.isLocal() )
        {
            fileInfo.update();
            if( file.isDirectory() ) fileInfo.setIsFolder();
            else fileInfo.setIsDocument();
            if( file.isLink() ) fileInfo.setIsLink();
            if( file.isBrokenLink() ) fileInfo.setIsBrokenLink();
            if( file.isHidden() ) fileInfo.setIsHidden();

            // check file info
            if( fileInfo.isDocument() || fileInfo.isBrokenLink() || (fileInfo.isLocal() && !fileInfo.file().exists() ) )
            {
                WarningDialog( this, tr( "Places item is invalid. <Edit Entry> canceled." ) ).exec();
                return;
            }

        }

        // update fileInfo
        focusItem_->setFileInfo( fileInfo );

    }

    // name
    const bool nameChanged( focusItem_->text() != dialog.name() );
    if( nameChanged ) focusItem_->setText( dialog.name() );

    // emit signal
    if( nameChanged || locationChanged || fileChanged )
    { emit contentsChanged(); }

}

//______________________________________________________________________
void PlacesWidget::_removeItem( void )
{
    Debug::Throw( "PlacesWidget::_removeItem.\n" );
    if( !focusItem_ ) return;

    // remove from button group
    group_->removeButton( focusItem_ );
    items_.removeOne( focusItem_ );
    focusItem_->hide();
    focusItem_->deleteLater();
    focusItem_ = 0x0;

    // emit signal
    _updateDragState();
    emit contentsChanged();

}

//_______________________________________________
void PlacesWidget::dragEnterEvent( QDragEnterEvent* event )
{
    Debug::Throw( "PlacesWidget::dragEnterEvent.\n" );
    if( _canDecode( event->mimeData() ) )
    {
        event->accept();
        dragInProgress_ = true;
        dragTarget_ = _updateDragTarget( event->pos() );
        update();
    }

}

//_______________________________________________
void PlacesWidget::dragMoveEvent( QDragMoveEvent* event )
{
    Debug::Throw( "PlacesWidget::dragMoveEvent.\n" );
    QPoint dragTarget( _updateDragTarget( event->pos() ) );
    if( dragTarget != dragTarget_ )
    {
        dragTarget_ = dragTarget;
        update();
    }
}

//_______________________________________________
void PlacesWidget::dragLeaveEvent( QDragLeaveEvent* )
{
    Debug::Throw( "PlacesWidget::dragLeaveEvent.\n" );
    dragInProgress_ = false;
    update();
}

//_______________________________________________
void PlacesWidget::dropEvent( QDropEvent* event )
{
    Debug::Throw( "PlacesWidget::dropEvent.\n" );

    // reset dragInProgress flag
    dragInProgress_ = false;
    update();

    if( event->mimeData()->hasFormat( PlacesWidgetItem::MimeType ) )
    {

        // internal dragging. Try re-order items
        PlacesWidgetItem* dragItem( 0x0 );
        foreach( PlacesWidgetItem* item, items_ )
        {
            if( item->isDragged() )
            { dragItem = item; break; }

        }
        if( !dragItem ) return;
        int sourceIndex = items_.indexOf( dragItem );

        // find insertion index based on target
        int insertionIndex( _index( dragTarget_ ) );

        // compare
        if( insertionIndex == sourceIndex || insertionIndex == sourceIndex+1 ) return;

        // remove from list and layout
        items_.takeAt( sourceIndex );
        buttonLayout_->takeAt( sourceIndex );

        // re-insert at the correct position
        if( sourceIndex < insertionIndex ) --insertionIndex;
        items_.insert( insertionIndex, dragItem );
        buttonLayout_->insertWidget( insertionIndex, dragItem );

    } else {

        // external source. Try copy items to list of favorites
        QList<BaseFileInfo> fileInfoList( _decode( event->mimeData() ) );
        if( fileInfoList.isEmpty() ) return;

        // find insertion index based on target
        int insertionIndex( _index( dragTarget_ ) );
        foreach( const BaseFileInfo& fileInfo, fileInfoList )
        {
            insert( insertionIndex, fileInfo );
            ++insertionIndex;
        }

    }

}

//_________________________________________________________________________________
void PlacesWidget::mousePressEvent( QMouseEvent* event )
{
    Debug::Throw( "PlacesWidget::mousePressEvent.\n" );

    const QPoint position( event->pos() );
    foreach( PlacesWidgetItem* item, items_ )
    {  item->setFocus( item->rect().translated( item->pos() ).contains( position ) ); }

    QWidget::mousePressEvent( event );
}

//_________________________________________________________________________________
void PlacesWidget::_updateDragState( void ) const
{
    foreach( PlacesWidgetItem* item, items_ )
    { item->setDragEnabled( items_.size()>1 ); }
}

//_________________________________________________________________________________
QPoint PlacesWidget::_updateDragTarget( const QPoint& position ) const
{
    int y(0);
    foreach( PlacesWidgetItem* item, items_ )
    {
        const QRect rect( item->rect().translated( item->pos() ) );
        if( rect.center().y() < position.y() ) y = rect.bottom();
        else {

            if( item->hasFocus() ) --y;
            break;

        }
    }

    return QPoint( 0, y );
}

//_________________________________________________________________________________
int PlacesWidget::_index( const QPoint& position ) const
{

    int index = 0;
    foreach( PlacesWidgetItem* item, items_ )
    {
        if( position.y()+1 >= item->rect().translated( item->pos() ).bottom() ) ++index;
        else break;
    }

    return index;

}

//_________________________________________________________________________________
bool PlacesWidget::_canDecode( const QMimeData* data ) const
{
    return
        data->hasFormat( PlacesWidgetItem::MimeType ) ||
        data->hasFormat( PathEditor::MimeType ) ||
        !_decode( data ).isEmpty();
}

//_________________________________________________________________________________
QList<BaseFileInfo> PlacesWidget::_decode( const QMimeData* mimeData ) const
{
    QList<BaseFileInfo> fileInfoList;
    if( !mimeData ) return fileInfoList;

    QString format;
    if( mimeData->hasFormat( BaseFileInfo::MimeType ) ) format = BaseFileInfo::MimeType;
    else if( mimeData->hasFormat( PathEditor::MimeType ) ) format = PathEditor::MimeType;
    if( !format.isEmpty() )
    {

        // get dropped file info (use XML)
        // dom document
        QDomDocument document;
        if( !document.setContent( mimeData->data( format ), false ) ) return fileInfoList;

        QDomElement docElement = document.documentElement();
        QDomNode node = docElement.firstChild();
        for(QDomNode node = docElement.firstChild(); !node.isNull(); node = node.nextSibling() )
        {
            QDomElement element = node.toElement();
            if( element.isNull() ) continue;

            // special options
            if( element.tagName() == XML::FILEINFO )
            {

                BaseFileInfo fileInfo( element );
                if( fileInfo.file().isEmpty() || fileInfo.isNavigator() || !fileInfo.isFolder() ) continue;
                fileInfoList << fileInfo;

            }
        }

    } else if( mimeData->hasUrls() ) {

        const QList<QUrl> urls( mimeData->urls() );
        foreach( const QUrl& url, urls )
        {

            #if QT_VERSION >= 0x040800
            // check that local file
            if( !url.isLocalFile() ) continue;
            #endif

            // get file and check existence
            const File file( url.path() );
            if( !( file.exists() && file.isDirectory() ) ) continue;

            BaseFileInfo fileInfo( file );
            fileInfo.update();

            fileInfo.setLocal();
            if( file.isDirectory() ) fileInfo.setIsFolder();
            else fileInfo.setIsDocument();
            if( file.isLink() ) fileInfo.setIsLink();
            if( file.isBrokenLink() ) fileInfo.setIsBrokenLink();
            if( file.isHidden() ) fileInfo.setIsHidden();

            fileInfoList << fileInfo;

        }

    }

    return fileInfoList;

}

//_________________________________________________________________________________
PlacesWidgetItem* PlacesWidget::_focusItem( void ) const
{
    foreach( PlacesWidgetItem* item, items_ )
    { if( item->hasFocus() ) return item; }

    return 0x0;
}

//_________________________________________________________________________________
void PlacesWidget::paintEvent( QPaintEvent* event )
{
    QWidget::paintEvent( event );
    if( !dragInProgress_ ) return;

    // paint line
    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.setPen( QPen( palette().color( foregroundRole() ), 1 ) );
    painter.drawLine( dragTarget_ + QPoint( 0, 1 ), dragTarget_ + QPoint( width(), 1 ) );
    painter.end();

}

//_________________________________________________________________________________
bool PlacesWidget::_setDBFile( const File& file )
{

    Debug::Throw() << "PlacesWidget::_setDBFile - file: " << file << endl;

    // check file
    if( dbFile_ == file && !items_.isEmpty() ) return false;

    // store file and read
    dbFile_ = file;

    // make sure file is hidden (windows only)
    if( dbFile_.localName().startsWith( '.' ) )
    { dbFile_.setHidden(); }

    read();

    return true;

}

//_________________________________________________________________________________
void PlacesWidget::_updateConfiguration( void )
{
    Debug::Throw( "PlacesWidget::_updateConfiguration.\n" );

    // DB file
    _setDBFile( File( XmlOptions::get().raw("RC_FILE") ) );

    // icon size
    if( XmlOptions::get().contains( "PLACES_ICON_SIZE" ) )
    {

        iconSizeMenu_->select( (IconSize::Size) XmlOptions::get().get<int>( "PLACES_ICON_SIZE" ) );

    } else iconSizeMenu_->select( IconSize::Default );

    return;

}

//_________________________________________________________________________________
void PlacesWidget::_saveConfiguration( void )
{
    Debug::Throw( "PlacesWidget::_saveConfiguration.\n" );
    write();
}

//_________________________________________________________________________________
void PlacesWidget::_installActions( void )
{
    Debug::Throw( "PlacesWidget::_installActions.\n" );

    addAction( addItemAction_ = new QAction( IconEngine::get( ICONS::ADD ), tr( "Add Entry..." ), this ) );
    connect( addItemAction_, SIGNAL(triggered( void ) ), SLOT( _addItem( void ) ) );

    addAction( addSeparatorAction_ = new QAction( IconEngine::get( ICONS::ADD ), tr( "Add Separator" ), this ) );
    connect( addSeparatorAction_, SIGNAL(triggered( void ) ), SLOT( _insertSeparator( void ) ) );

    addAction( editItemAction_ = new QAction( IconEngine::get( ICONS::EDIT ), tr( "Edit Entry..." ), this ) );
    editItemAction_->setShortcut( Qt::Key_F2 );
    connect( editItemAction_, SIGNAL(triggered( void ) ), SLOT( _editItem( void ) ) );

    addAction( removeItemAction_ = new QAction( IconEngine::get( ICONS::REMOVE ), tr( "Remove Entry" ), this ) );
    connect( removeItemAction_, SIGNAL(triggered( void ) ), SLOT( _removeItem( void ) ) );

}
