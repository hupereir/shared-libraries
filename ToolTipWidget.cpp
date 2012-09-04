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

#include "ToolTipWidget.h"
#include "Debug.h"
#include "GridLayout.h"
#include "TimeStamp.h"

#include <QtGui/QFrame>
#include <QtGui/QLayout>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionFrame>
#include <QtGui/QToolTip>

//_______________________________________________________
ToolTipWidget::ToolTipWidget( QWidget* parent ):
    QWidget( parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint ),
    Counter( "ToolTipWidget" ),
    pixmapSize_( 96 )
{

    Debug::Throw( "ToolTipWidget::ToolTipWidget.\n" );
    setAttribute( Qt::WA_TranslucentBackground, true );

    // change palete
    setPalette( QToolTip::palette() );
    setBackgroundRole( QPalette::ToolTipBase );
    setForegroundRole( QPalette::ToolTipText );

    // layout
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin( 10 );
    hLayout->setSpacing( 10 );
    setLayout( hLayout );

    hLayout->addWidget( iconLabel_ = new QLabel( this ) );
    iconLabel_->setAlignment( Qt::AlignHCenter|Qt::AlignTop );

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setMargin( 0 );
    vLayout->setSpacing( 5 );
    hLayout->addLayout( vLayout );

    // file
    vLayout->addWidget( fileLabel_ = new QLabel( this ) );
    fileLabel_->setAlignment( Qt::AlignCenter );
    QFont font( fileLabel_->font() );
    font.setBold( true );
    fileLabel_->setFont( font );
    fileLabel_->setMargin( 1 );

    // separator
    vLayout->addWidget( separator_ = new QFrame( this ) );
    separator_->setFrameStyle( QFrame::HLine );

    // grid layout
    GridLayout* gridLayout = new GridLayout();
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignVCenter|Qt::AlignRight );
    gridLayout->setColumnAlignment( 1, Qt::AlignVCenter|Qt::AlignLeft );
    gridLayout->setMargin( 0 );
    gridLayout->setSpacing( 5 );
    vLayout->addLayout( gridLayout );

    // items
    ( typeItem_ = new Item( this, gridLayout ) )->setKey( "Type:" );
    ( sizeItem_ = new Item( this, gridLayout ) )->setKey( "Size:" );
    ( lastModifiedItem_ = new Item( this, gridLayout ) )->setKey( "Modified:" );
    ( userItem_ = new Item( this, gridLayout ) )->setKey( "User:" );
    ( groupItem_ = new Item( this, gridLayout ) )->setKey( "Group:" );

    // add stretch
    vLayout->addStretch( 1 );
}

//_______________________________________________________
void ToolTipWidget::setBaseFileInfo( const BaseFileInfo& fileInfo, const QIcon& icon )
{
    Debug::Throw( "ToolTipWidget::setBaseFileInfo.\n" );
    if( !icon.isNull() )
    {

        iconLabel_->setPixmap( icon.pixmap( QSize( pixmapSize_, pixmapSize_ ) ) );
        iconLabel_->show();

    } else iconLabel_->hide();

    if( !fileInfo.file().isEmpty() )
    {
        // file and separator
        fileLabel_->show();
        fileLabel_->setText( fileInfo.file().localName() );
        separator_->show();

        // type
        typeItem_->setText( fileInfo.typeString() );

        // size
        if( fileInfo.size() > 0 && !( fileInfo.type() & (BaseFileInfo::FOLDER|BaseFileInfo::LINK|BaseFileInfo::NAVIGATOR) ) )
        {

            sizeItem_->setText( File::sizeString( fileInfo.size() ) );

        } else sizeItem_->hide();

        // last modified
        if( TimeStamp( fileInfo.lastModified() ).isValid() )
        {

            lastModifiedItem_->setText( TimeStamp( fileInfo.lastModified() ).toString() );

        } else lastModifiedItem_->hide();

        // user
        if( !fileInfo.user().isEmpty() ) userItem_->setText( fileInfo.user() );
        else userItem_->hide();
        
        // group
        if( !fileInfo.group().isEmpty() ) groupItem_->setText( fileInfo.group() );
        else groupItem_->hide();
        
    } else {

        // file and separator
        fileLabel_->hide();
        separator_->hide();

        // items
        typeItem_->hide();
        sizeItem_->hide();
        lastModifiedItem_->hide();

    }

    adjustSize();

}

//_______________________________________________________
void ToolTipWidget::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOptionFrame opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &painter, this );
    return QWidget::paintEvent( event );
}

//_______________________________________________________
ToolTipWidget::Item::Item( QWidget* parent, GridLayout* layout ):
    QObject( parent ),
    Counter( "ToolTipWidget::Item" )
{
    layout->addWidget( key_ = new QLabel( parent ) );
    layout->addWidget( value_ = new QLabel( parent ) );
}
