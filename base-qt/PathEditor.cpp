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
#include "BaseFileInfo.h"
#include "BaseIconNames.h"
#include "Debug.h"
#include "DragMonitor.h"
#include "IconEngine.h"
#include "IconSize.h"
#include "PathEditor_p.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "XmlOptions.h"
#include "XmlPathHistory.h"


#include <QApplication>
#include <QDrag>
#include <QLayout>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QStyleOptionViewItem>
#include <QTextOption>
#include <QToolButton>

//___________________________________________________________________
const QString PathEditor::MimeType( QStringLiteral("internal/path-editor-item") );

namespace Private
{

    class Widget: public QWidget
    {

        public:
        Widget( QWidget* parent = nullptr ):
            QWidget(parent)
            {}

        protected:
        void paintEvent( QPaintEvent* event )
        {
            QPainter painter( this );
            painter.setClipRegion( event->region() );
            QStyleOption o;
            o.initFrom(this);
            o.state &=~(QStyle::State_MouseOver|QStyle::State_HasFocus);
            QApplication::style()->drawPrimitive( QStyle::PE_FrameLineEdit, &o, &painter, this );
        }
    };

    //___________________________________________________________________
    const qreal PathEditorButton::BorderWidth = 1.5;

    //___________________________________________________________________
    PathEditorButton::PathEditorButton( QWidget* parent ):
        QAbstractButton( parent )
    {
        Debug::Throw( QStringLiteral("PathEditorButton::PathEditorButton.\n") );
        setAttribute( Qt::WA_Hover );
        setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Fixed );
        setMinimumHeight(parent->minimumHeight());
    }

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
    PathEditorItem::PathEditorItem( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( QStringLiteral("PathEditorItem") ),
        isLocal_( true ),
        isSelectable_( true ),
        isLast_( false )
    {
        Debug::Throw( QStringLiteral("PathEditorItem::PathEditorItem.\n") );
        dragMonitor_ = new DragMonitor( this );
        dragMonitor_->setDragEnabled( false );
        connect( dragMonitor_, &DragMonitor::dragStarted, this, &PathEditorItem::_startDrag);
    }

    //____________________________________________________________________________
    void PathEditorItem::setPath( const File& path, const QString& name )
    {

        Debug::Throw( QStringLiteral("PathEditorItem::setPath.\n") );

        path_ = path;

        if( name.isEmpty() )
        {

            // get local name
            File localName( path.localName().removeTrailingSlash() );
            setText( localName );

        } else setText( name );

        updateMinimumSize();

    }

    //____________________________________________________________________________
    QSize PathEditorItem::sizeHint() const
    {
        if( isLast_ )
        {
            auto size = minimumSize();
            size.rwidth() += fontMetrics().boundingRect( text() ).width() + 4;
            return size;
        } else {
            return minimumSize();
        }
    }

    //____________________________________________________________________________
    QSize PathEditorItem::minimumSizeHint() const
    { return minimumSize(); }

    //____________________________________________________________________________
    void PathEditorItem::updateMinimumSize()
    {
        Debug::Throw( QStringLiteral("PathEditorItem::updateMinimumSize.\n") );

        // text size
        QSize size( fontMetrics().boundingRect( text() ).size() );
        if( isLast_ ) size.setWidth( 0 );

        // margins
        size.rwidth() += 6*BorderWidth;
        size.rheight() += 4*BorderWidth;

        // separator width
        const int separatorWidth( _separatorWidth() );
        if( separatorWidth > 0 ) size.rwidth() += separatorWidth + 2*BorderWidth;

        // update
        setMinimumSize( size );
    }

    //_______________________________________________
    void PathEditorItem::_startDrag( QPoint dragOrigin )
    {

        Debug::Throw( QStringLiteral("PathEditorItem::_startDrag.\n") );

        // start drag
        auto drag = new QDrag(this);
        auto mimeData = new QMimeData;

        // fill Drag data. Use XML
        QDomDocument document;
        QDomElement top = document.appendChild( document.createElement( Xml::FileInfoList ) ).toElement();

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

        const QString value( prefix_.isEmpty() ? path_ : prefix_ + "//" + path_ );
        mimeData->setText( value );
        mimeData->setData( PathEditor::MimeType, document.toByteArray() );
        drag->setMimeData( mimeData );

        // create drag pixmap
        QPixmap pixmap( size() );
        pixmap.fill( Qt::transparent );
        QPainter painter( &pixmap );
        _paint( &painter );

        drag->setPixmap( pixmap );
        drag->setHotSpot( QPoint( (dragOrigin-rect().topLeft()).x(), 0 ) );

        drag->exec();

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

            QStyleOptionViewItem option;
            option.initFrom( this );
            option.showDecorationSelected = true;
            option.rect = rect();
            option.state |= QStyle::State_MouseOver;
            style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, painter, _itemView() );

        }

        const bool isRightToLeft( qApp->isRightToLeft() );

        // render text
        auto textRect( rect().adjusted( 3*BorderWidth, 2*BorderWidth, -3*BorderWidth, -2*BorderWidth ) );
        const auto separatorWidth( _separatorWidth() + 2*BorderWidth );
        if( !isLast_ )
        {
            if( isRightToLeft ) textRect.adjust( separatorWidth, 0, 0, 0 );
            else textRect.adjust( 0, 0, -separatorWidth, 0 );
        }

        const auto text = isLast_ ?
            fontMetrics().elidedText( this->text(), Qt::ElideRight, textRect.width() ):
            this->text();

        painter->drawText( QRectF( textRect ), Qt::AlignLeft|Qt::AlignVCenter|Qt::TextHideMnemonic, text );

        // render arrow
        if( !isLast_ )
        {
            painter->setPen( palette().color(QPalette::Text) );
            const auto rect( this->rect() );

            QStyleOptionFrame option;
            option.initFrom( this );
            option.rect = isRightToLeft ?
                QRect( rect.topLeft(), QSize(1,rect.height() )):
                QRect( rect.topRight(), QSize(1,rect.height() ));
            option.frameShape = QFrame::VLine;

            style()->drawControl(QStyle::CE_ShapedFrame, &option, painter, this );
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
            QStyleOptionViewItem option;
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
    void PathEditorMenuButton::updateMinimumSize()
    {
        Debug::Throw( QStringLiteral("PathEditorMenuButton::updateMinimumSize.\n") );

        const auto metrics( fontMetrics() );
        QSize size( metrics.height(), metrics.height() );
        size.rwidth() += 2*BorderWidth;
        size.rheight() += 4*BorderWidth;

        // update
        setMinimumSize( size );
    }

    //________________________________________________________________________
    PathEditorSwitch::PathEditorSwitch( QWidget* parent ):
        PathEditorButton( parent ),
        Counter( QStringLiteral("PathEditorSwitch") )
    {
        Debug::Throw( QStringLiteral("PathEditorItem::PathEditorItem.\n") );
        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
        setCursor( Qt::IBeamCursor );
        setIcon(IconEngine::get(IconNames::Edit));
    }

    //________________________________________________________________________
    void PathEditorSwitch::paintEvent( QPaintEvent* event )
    {

        if( icon().isNull() ) return;
        if( _mouseOver() && isEnabled() )
        {
            QStyleOptionButton option;
            option.initFrom( this );

            const int iconWidth( style()->pixelMetric( QStyle::PM_ToolBarIconSize, &option, this ) );
            const QSize iconSize( iconWidth, iconWidth );
            const auto pixmap( icon().pixmap( iconSize ) );

            const bool isRightToLeft( qApp->isRightToLeft() );

            const auto rect( this->rect() );
            const auto iconRect = isRightToLeft ?
                QRect(QPoint( rect.left(), rect.y() + (rect.height() - iconWidth)/2 ), iconSize ):
                QRect(QPoint( rect.right() - iconWidth, rect.y() + (rect.height() - iconWidth)/2 ), iconSize );

            QPainter painter( this );
            painter.setClipRegion( event->region() );
            painter.drawPixmap( iconRect, pixmap );

        }

    }

}

//____________________________________________________________________________
PathEditor::PathEditor( QWidget* parent ):
    QStackedWidget( parent ),
    Counter( QStringLiteral("PathEditor") ),
    usePrefix_( true ),
    isLocal_( true ),
    truncate_( true ),
    dragEnabled_( false ),
    history_( new XmlPathHistory( this ) )
{
    Debug::Throw( QStringLiteral("PathEditor::PathEditor.\n") );

    // size policy
    setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );

    {
        // browser
        // some styles require an item view passed to painting method to have proper selection rendered in items
        itemView_ = new QListView( this );
        itemView_->hide();

        browserContainer_ = new Private::Widget;
        browserContainer_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

        auto hLayout = new QHBoxLayout;
        hLayout->setSpacing(0);
        QtUtil::setMargin( hLayout, 0 );
        browserContainer_->setLayout( hLayout );

        // prefix label
        prefixLabel_ = new QLabel( browserContainer_ );
        prefixLabel_->setMargin( 2*Private::PathEditorButton::BorderWidth );
        hLayout->addWidget( prefixLabel_ );
        prefixLabel_->hide();

        // menu button
        hLayout->addWidget( menuButton_ = new Private::PathEditorMenuButton( browserContainer_ ) );
        menuButton_->setItemView( itemView_ );
        menuButton_->hide();
        connect( menuButton_, &QAbstractButton::clicked, this, &PathEditor::_menuButtonClicked );

        // button layout
        hLayout->addLayout( buttonLayout_ = new QHBoxLayout );
        buttonLayout_->setSpacing(0);
        QtUtil::setMargin(buttonLayout_, 0);

        // switch
        Private::PathEditorSwitch* editorSwitch = new Private::PathEditorSwitch( browserContainer_ );
        hLayout->addWidget( editorSwitch, 1 );
        connect( editorSwitch, &QAbstractButton::clicked, this, &PathEditor::_showEditor );

        // button group
        group_ = new QButtonGroup( browserContainer_ );
        group_->setExclusive( false );
        connect( group_, QOverload<QAbstractButton*>::of( &QButtonGroup::buttonClicked ), this, &PathEditor::_buttonClicked );

        addWidget( browserContainer_ );
    }

    {
        // editor
        editorContainer_ = new QWidget;
        editorContainer_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

        auto hLayout = new QHBoxLayout;
        hLayout->setSpacing(0);
        QtUtil::setMargin(hLayout, 0);
        editorContainer_->setLayout( hLayout );

        editor_ = new ComboBox( editorContainer_ );
        editor_->setEditable( true );
        hLayout->addWidget( editor_ );

        // ok button
        auto button = new QToolButton( editorContainer_ );
        button->setIcon( IconEngine::get( IconNames::DialogOk ) );
        button->setAutoRaise( true );
        button->setToolButtonStyle( Qt::ToolButtonIconOnly );
        button->setIconSize( IconSize::get( IconSize::Small ) );
        button->setFixedSize( QSize( editor_->height(), editor_->height() ) );
        hLayout->addWidget( button );

        button->setFocusPolicy( Qt::StrongFocus );

        connect( editor_->lineEdit(), &QLineEdit::returnPressed, this, &PathEditor::_returnPressed );
        connect( editor_, QOverload<int>::of( &QComboBox::activated ), this, [this](int){ _returnPressed(); } );
        connect( button, &QAbstractButton::clicked, this, &PathEditor::_showBrowser );

        addWidget( editorContainer_ );
    }

    {
        // menus
        previousPathMenu_ = new QMenu( this );
        nextPathMenu_ = new QMenu( this );

        connect( previousPathMenu_, &QMenu::triggered, this, &PathEditor::selectFromMenu );
        connect( nextPathMenu_, &QMenu::triggered, this, &PathEditor::selectFromMenu );

    }

    // current widget
    setCurrentWidget( browserContainer_ );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &PathEditor::_updateConfiguration );
    _updateConfiguration();

}

//____________________________________________________________________________
File PathEditor::path() const
{
    Debug::Throw( QStringLiteral("PathEditor::path.\n") );
    QString path( editor_->currentText() );
    if( !prefix_.isEmpty() )
    {
        const QString prefix( prefix_+"//" );
        if( path.startsWith( prefix ) )
        { path.remove( 0, prefix.size() ); }
    }

    return File( path );
}

//____________________________________________________________________________
QString PathEditor::prettyPath() const
{
    auto path = this->path().removeTrailingSlash();
    if( ( truncate_ || path.isEmpty() ) )
    {
        if( path == File( home_ ).removeTrailingSlash() ) return QStringLiteral("Home");
        else if( std::any_of( rootPathList_.begin(), rootPathList_.end(), [&path]( File root )
        { return path == root.removeTrailingSlash(); } ) ) return QStringLiteral("Root");
    }

    return path;

}

//____________________________________________________________________________
bool PathEditor::hasParent() const
{
    if( items_.isEmpty() ) return false;
    else if( items_.back()->isSelectable() ) return items_.size() >= 2;
    else return items_.size() >= 3;
}

//____________________________________________________________________________
bool PathEditor::hasPrevious() const
{ return history_->previousAvailable(); }

//____________________________________________________________________________
bool PathEditor::hasNext() const
{ return history_->nextAvailable(); }

//____________________________________________________________________________
QSize PathEditor::minimumSizeHint() const
{
    int minWidth( 0 );

    if( usePrefix_ && !prefix_.isEmpty() )
    { minWidth += prefixLabel_->width(); }

    if( rootPathList_.size() > 1 || items_.size() > 1 || ( truncate_ && !home_.isEmpty() ) )
    { minWidth += menuButton_->width(); }

    if( !items_.empty() )
    { minWidth += items_.back()->minimumSizeHint().width(); }

    return QSize( minWidth, QStackedWidget::minimumSizeHint().height() );
}

//____________________________________________________________________________
void PathEditor::setPrefix( const QString& value )
{
    Debug::Throw( QStringLiteral("PathEditor::setPrefix.\n") );
    if( prefix_ == value ) return;
    prefix_ = value;
    prefixLabel_->setText( prefix_ );

    _updatePrefix();
}

//____________________________________________________________________________
void PathEditor::setHistoryTagName( const QString& value )
{ static_cast<XmlPathHistory*>(history_)->setTagName( value ); }

//____________________________________________________________________________
void PathEditor::setHomePath( const File& value )
{
    Debug::Throw( QStringLiteral("PathEditor::setHomePath.\n") );
    if( home_ == value ) return;
    home_ = value;
    _reload();
}

//____________________________________________________________________________
void PathEditor::setRootPathList( const File::List& value )
{
    Debug::Throw( QStringLiteral("PathEditor::setRootPathList.\n") );
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
    for( const auto& item:items_ )
    { item->setIsLocal( value ); }

}

//____________________________________________________________________________
void PathEditor::setDragEnabled( bool value )
{
    if( value == dragEnabled_ ) return;

    // assign to this widget, and children
    dragEnabled_ = value;
    for( const auto& item:items_ )
    { item->dragMonitor().setDragEnabled( value ); }

}

//____________________________________________________________________________
void PathEditor::setPath( const File& constPath, const File& file )
{

    Debug::Throw() << "PathEditor::setPath - " << constPath << Qt::endl;

    // upbate browser
    {

        // search proper root path
        QString root( QStringLiteral("/") );
        QString path( constPath );

        for( const auto& file:rootPathList_ )
        {
            if( path.startsWith( file ) )
            {
                // store root file and truncate
                root = file;
                path.remove( 0, root.size() );
                break;
            }
        }

        // need to keep focus
        const bool hasFocus( !items_.isEmpty() && items_.back()->hasFocus() );

        // check if home directory is to be used
        const bool hasHome( truncate_ && !home_.isEmpty() );

        // create root item
        int index = 0;
        Private::PathEditorItem* item = nullptr;
        if( index < items_.size() ) {

            item = items_[index];
            item->setIsLast( false );

        } else {

            item = new Private::PathEditorItem( browserContainer_ );
            item->setPrefix( prefix_ );
            item->setIsLocal( isLocal_ );
            item->setItemView( itemView_ );
            item->dragMonitor().setDragEnabled( dragEnabled_ );
            group_->addButton( item );
            buttonLayout_->addWidget( item );
            items_.append( item );

        }

        if( hasHome && root == home_ ) item->setPath( File( root ), QStringLiteral("Home") );
        else item->setPath( File( root ), QStringLiteral("Root") );
        index++;

        // create path items
        const int sectionCount( path.split( QLatin1Char('/'), Qt::SkipEmptyParts ).size() );
        for( int i=0; i < sectionCount; i++ )
        {

            // setup section
            QString section = root + path.section( '/', 0, i, QString::SectionSkipEmpty );

            if( index < items_.size() )
            {

                item = items_[index];
                item->setIsLast( false );

            } else {

                item = new Private::PathEditorItem( browserContainer_ );
                item->setPrefix( prefix_ );
                item->setIsLocal( isLocal_ );
                item->setItemView( itemView_ );
                item->dragMonitor().setDragEnabled( dragEnabled_ );
                group_->addButton( item );
                buttonLayout_->addWidget( item );
                items_.append( item );

            }

            if( hasHome && section == home_ ) item->setPath( File( section ), QStringLiteral("Home") );
            else item->setPath( File( section ) );
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

                item = new Private::PathEditorItem( browserContainer_ );
                item->setPrefix( prefix_ );
                item->setIsLocal( isLocal_ );
                item->setItemView( itemView_ );
                item->dragMonitor().setDragEnabled( dragEnabled_ );
                group_->addButton( item );
                buttonLayout_->addWidget( item );
                items_.append( item );

            }

            item->setPath( File( file ).addPath( File( path ) ) );
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
        File path( file.isEmpty() ? constPath: File( file ).addPath( constPath ) );
        if( editor_->currentText() != path )
        {

            const bool usePrefix( usePrefix_ && !prefix_.isEmpty() );
            const QString prefix( prefix_+"//" );
            if( usePrefix && !path.startsWith( prefix ) ) path.get().prepend( prefix );

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
    history_->add( constPath );
    _updatePathMenus();

}

//____________________________________________________________________________
void PathEditor::selectParent()
{
    if( !hasParent() ) return;
    const File path( (items_.back()->isSelectable() ? items_[items_.size()-2]:items_[items_.size()-3])->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::selectPrevious()
{
    Debug::Throw( QStringLiteral("PathEditor::selectPrevious.\n") );
    if( !hasPrevious() ) return;
    const File path( history_->previous() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::selectNext()
{
    Debug::Throw( QStringLiteral("PathEditor::selectNext.\n") );
    if( !hasNext() ) return;
    const File path( history_->next() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::selectFromMenu( QAction* action )
{
    Debug::Throw( QStringLiteral("PathEditor::selectFromMenu.\n") );

    const QVariant data( action->data() );
    if( !data.canConvert( QVariant::Int ) ) return;

    const File path( history_->selectPath( data.toInt() ) );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::resizeEvent( QResizeEvent* event )
{
    QMetaObject::invokeMethod( this, &PathEditor::_updateButtonVisibility, Qt::QueuedConnection );
    QWidget::resizeEvent( event );
}

//____________________________________________________________________________
void PathEditor::_updatePrefix()
{

    Debug::Throw( QStringLiteral("PathEditor::_updatePrefix.\n") );

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
            else if( item.startsWith( prefix ) ) item.remove( 0, prefix.size() );

            if( item.trimmed().isEmpty() ) continue;
            items.append( item );
        }

        editor_->clear();
        editor_->addItems( items );
        setPath( path );
    }

}

//____________________________________________________________________________
void PathEditor::_setUseTruncation( bool value )
{
    Debug::Throw( QStringLiteral("PathEditor::_setUseTruncation.\n") );
    if( truncate_ == value ) return;
    truncate_ = value;
    _reload();
}

//____________________________________________________________________________
void PathEditor::_returnPressed()
{
    const File path( this->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_menuButtonClicked()
{

    // get list of hidden buttons
    File::List pathList;
    for( const auto& item:items_ )
    {
        if( item->isHidden() ) pathList.append( item->path() );
        else {

            // also add first non visible path
            pathList.append( item->path() );
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
        for( const auto& path:rootPathList_ )
        {

            QAction* action = menu->addAction( path );
            action->setData( path.get() );

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
    for( const auto& path:pathList )
    {
        Debug::Throw() << "PathEditor::_menuButtonClicked - adding " << path << Qt::endl;
        menu->addAction( path )->setData( path.get() );
    }

    connect( menu, &QMenu::triggered, this, &PathEditor::_updatePath );
    menu->exec( menuButton_->mapToGlobal( menuButton_->rect().bottomLeft() ) );
    static_cast<Private::PathEditorButton*>(menuButton_)->setMouseOver( false );

}

//____________________________________________________________________________
void PathEditor::_updatePath( QAction* action )
{
    File path( action->data().toString() );
    Debug::Throw() << "PathEditor::_updatePath - path: " << path << Qt::endl;
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_buttonClicked( QAbstractButton* button )
{

    // cast button
    Private::PathEditorItem* item( static_cast<Private::PathEditorItem*>( button ) );

    // check selectable test
    if( !item->isSelectable() ) return;

    // retrieve path and emit signal
    const File path( item->path() );
    setPath( path );
    emit pathChanged( path );
}

//____________________________________________________________________________
void PathEditor::_updateButtonVisibility()
{

    Debug::Throw() << "PathEditor::_updateButtonVisibility - path: " << path() << Qt::endl;

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
        auto iter = std::find_if( items_.begin(), items_.end(), [this]( Private::PathEditorItem* item ) { return item->path() == home_; } );
        if( iter != items_.end() )
        {
            hasHomePath = true;
            hasHiddenButtons = (iter != items_.begin() );
        }

    }

    // check item width
    if( !hasHiddenButtons )
    {
        int width( 0 );
        for( const auto& item:items_ )
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
        Private::PathEditorItem::ListIterator iterator( items_ );
        iterator.toBack();
        while( iterator.hasPrevious() )
        {
            // get item
            Private::PathEditorItem* item( iterator.previous() );

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
void PathEditor::_updatePathMenus()
{
    Debug::Throw( QStringLiteral("PathEditor::_updatePathMenus.\n") );

    // previous
    int index = 0;
    previousPathMenu_->clear();
    for( const auto& record:history_->previousPathList() )
    {
        auto action( previousPathMenu_->addAction( record.file() ) );
        action->setData( index );
        ++index;
    }

    // next
    ++index;
    nextPathMenu_->clear();
    for( const auto& record:history_->nextPathList() )
    {
        auto action( nextPathMenu_->addAction( record.file() ) );
        action->setData( index );
        ++index;
    }

}

//____________________________________________________________________________
void PathEditor::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("PathEditor::_updateConfiguration.\n") );

    if( XmlOptions::get().contains( QStringLiteral("USE_PREFIX") ) )
    { _setUsePrefix( XmlOptions::get().get<bool>( QStringLiteral("USE_PREFIX") ) ); }

    if( XmlOptions::get().contains( QStringLiteral("USE_TRUNCATION") ) )
    { _setUseTruncation( XmlOptions::get().get<bool>( QStringLiteral("USE_TRUNCATION") ) ); }

}
