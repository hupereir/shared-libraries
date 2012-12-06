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

#include "FavoritesWidget.h"
#include "FavoritesWidget_p.h"

#include "BaseFileIconProvider.h"
#include "Singleton.h"
#include "XmlError.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QPainter>

const double FavoritesWidgetItem::BorderWidth = 2;

namespace XML
{

    static const QString NAME = "name";

};

//___________________________________________________________________
void FavoritesWidgetItem::updateMinimumSize( void )
{

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
bool FavoritesWidgetItem::event( QEvent* event )
{

    switch( event->type() )
    {

        case QEvent::HoverEnter: mouseOver_ = true; break;
        case QEvent::HoverLeave: mouseOver_ = false; break;
        default: break;
    }

    return QAbstractButton::event( event );

}

//___________________________________________________________________
void FavoritesWidgetItem::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    // render mouse over
    if( mouseOver_ )
    {

        QStyleOptionViewItemV4 option;
        option.initFrom( this );
        option.showDecorationSelected = true;
        option.rect = rect();
        option.state |= QStyle::State_MouseOver;
        style()->drawPrimitive( QStyle::PE_PanelItemViewItem, &option, &painter, itemView_ );

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
        painter.setFont( font() );
        painter.drawText( QRectF( textRect ), text(), QTextOption( Qt::AlignVCenter | (isRightToLeft ? Qt::AlignRight : Qt::AlignLeft ) ) );

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
        const QPixmap pixmap( icon().pixmap( iconSize() ) );
        const QPoint position(
            iconRect.x() + 0.5*(iconRect.width() - pixmap.width()),
            iconRect.y() + 0.5*(iconRect.height() - pixmap.height()) );

        painter.drawPixmap( position, pixmap );

    }

    painter.end();

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
FavoritesWidget::FavoritesWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "FavoritesWidget::FavoritesWidget" ),
    iconProvider_( 0x0 )
{

    Debug::Throw( "FavoritesWidget::FavoritesWidget.\n" );

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
    connect( group_, SIGNAL( buttonClicked( QAbstractButton* ) ), SLOT( _buttonClicked( QAbstractButton* ) ) );

    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    connect( qApp, SIGNAL( aboutToQuit() ), SLOT( _saveConfiguration() ) );
    _updateConfiguration();

}

//______________________________________________________________________
void FavoritesWidget::setIconProvider( BaseFileIconProvider* provider )
{

    // set provider
    iconProvider_ = provider;

    // update icons for existing items
    foreach( FavoritesWidgetItem* item, _items() )
    {
        if( item->icon().isNull() )
        { item->setIcon( iconProvider_->icon( item->fileInfo() ) ); }
    }

}

//______________________________________________________________________
void FavoritesWidget::clear( void )
{
    // delete all items
    foreach( QAbstractButton* item, group_->buttons() )
    {
        // remove from group and delete later
        group_->removeButton( item );
        item->hide();
        item->deleteLater();
    }
}

//______________________________________________________________________
void FavoritesWidget::add( const QString& name, const BaseFileInfo& fileInfo )
{
    add(
        iconProvider_ ? iconProvider_->icon( fileInfo ):QIcon(),
        name,
        fileInfo );
}

//___________________________________________________________________
void FavoritesWidget::add( const QIcon& icon, const QString& name, const BaseFileInfo& fileInfo )
{

    Debug::Throw( "FavoritesWidget::add.\n" );

    // create new item
    FavoritesWidgetItem* item = new FavoritesWidgetItem( this );
    item->setIcon( icon );
    item->setText( name );
    item->setFileInfo( fileInfo );

    // add to button group, list of items and layout
    group_->addButton( item );
    buttonLayout_->addWidget( item );

    emit contentsChanged();

}

//___________________________________________________________________
void FavoritesWidget::_buttonClicked( QAbstractButton* button )
{

    Debug::Throw( "FavoritesWidget::_buttonClicked.\n" );

    // try cast
    FavoritesWidgetItem* item = qobject_cast<FavoritesWidgetItem*>( button );
    if( !item ) return;

    emit itemSelected( item->fileInfo() );

}

//_______________________________________________
QList<FavoritesWidgetItem*> FavoritesWidget::_items( void ) const
{
    Debug::Throw( "FavoritesWidget::_items.\n" );
    QList<FavoritesWidgetItem*> out;
    foreach( QAbstractButton* child, group_->buttons() )
    { out.append( static_cast<FavoritesWidgetItem*>( child ) ); }

    return out;
}

//_______________________________________________
bool FavoritesWidget::_setDBFile( const File& file )
{

    Debug::Throw() << "FavoritesWidget::_setDBFile - file: " << file << endl;

    // check file
    if( dbFile_ == file && !_items().isEmpty() ) return false;

    // store file and read
    dbFile_ = file;

    // make sure file is hidden (windows only)
    if( dbFile_.localName().startsWith( '.' ) )
    { dbFile_.setHidden(); }

    _read();

    return true;

}

//_______________________________________________
bool FavoritesWidget::_read( void )
{
    Debug::Throw( "FavoritesWidget::_read.\n" );
    if( dbFile_.isEmpty() || !dbFile_.exists() ) return false;

    // parse the file
    QFile in( dbFile_ );
    if ( !in.open( QIODevice::ReadOnly ) )
    {
        Debug::Throw( "FavoritesWidget::_read - cannot open file.\n" );
        return false;
    }

    // dom document
    QDomDocument document;
    XmlError error( dbFile_ );
    if ( !document.setContent( &in, &error.error(), &error.line(), &error.column() ) )
    {
        in.close();
        Debug::Throw() << error << endl;
        return false;
    }

    QDomElement docElement = document.documentElement();
    QDomNode node = docElement.firstChild();
    for(QDomNode node = docElement.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        QDomElement element = node.toElement();
        if( element.isNull() ) continue;

        // special options
        if( element.tagName() == XML::FILEINFO )
        {

            LocalFileInfo fileInfo( element );
            if( !fileInfo.file().isEmpty() ) add( fileInfo.name(), fileInfo );
            else Debug::Throw(0, "FavoritesWidget::_read - attend to add empty file info. Discarded.\n" );

        } else Debug::Throw() << "FavoritesWidget::_read - unrecognized tag " << element.tagName() << endl;
    }

    emit contentsChanged();

    return true;
}

//_______________________________________________
bool FavoritesWidget::_write( void )
{
    Debug::Throw( "FavoritesWidget::_write.\n" );
    if( dbFile_.isEmpty() )
    {
        Debug::Throw( "FavoritesWidget::_write - no file.\n" );
        return false;
    }

    // output file
    QFile out( dbFile_ );
    if( !out.open( QIODevice::WriteOnly ) ) return false;

    // get records truncated list
    QList<FavoritesWidgetItem*> items( _items() );

    // create document
    QDomDocument document;

    // create main element
    QDomElement top = document.appendChild( document.createElement( XML::FILEINFO_LIST ) ).toElement();

    // loop over records
    foreach( FavoritesWidgetItem* item, items )
    {

        const BaseFileInfo& fileInfo( item->fileInfo() );
        Debug::Throw() << "FavoritesWidget::_write - " << fileInfo << endl;

        if( fileInfo.file().isEmpty() )
        {
            Debug::Throw(0, "FavoritesWidget::_write - attend to write empty file info. Discarded.\n" );
            continue;
        }

        top.appendChild( LocalFileInfo( fileInfo, item->text() ).domElement( document ) );
    }

    out.write( document.toByteArray() );
    out.close();

    return true;
}


//______________________________________
void FavoritesWidget::_updateConfiguration( void )
{
    Debug::Throw( "FavoritesWidget::_updateConfiguration.\n" );

    // DB file
    _setDBFile( File( XmlOptions::get().raw("DB_FILE") ) );
    return;

}

//______________________________________
void FavoritesWidget::_saveConfiguration( void )
{
    Debug::Throw( "FavoritesWidget::_saveConfiguration.\n" );
    _write();
}
