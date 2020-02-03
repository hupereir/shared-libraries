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

#include "BaseFileInfoToolTipWidget.h"
#include "BaseFileIconProvider.h"
#include "Debug.h"
#include "GridLayout.h"
#include "GridLayoutItem.h"
#include "QtUtil.h"
#include "Singleton.h"
#include "TimeStamp.h"
#include "XmlOptions.h"

#include <QLayout>

//_______________________________________________________
BaseFileInfoToolTipWidget::BaseFileInfoToolTipWidget( QWidget* parent ):
    BaseToolTipWidget( parent )
{

    Debug::Throw( "BaseFileInfoToolTipWidget::BaseFileInfoToolTipWidget.\n" );

    // layout
    auto hLayout = new QHBoxLayout;
    hLayout->setMargin( 10 );
    hLayout->setSpacing( 10 );
    setLayout( hLayout );

    hLayout->addWidget( iconLabel_ = new QLabel( this ) );
    iconLabel_->setAlignment( Qt::AlignHCenter|Qt::AlignTop );

    auto vLayout = new QVBoxLayout;
    vLayout->setMargin( 0 );
    vLayout->setSpacing( 5 );
    hLayout->addLayout( vLayout );

    // file
    vLayout->addStretch( 1 );
    vLayout->addWidget( fileLabel_ = new QLabel( this ) );
    fileLabel_->setAlignment( Qt::AlignCenter );
    fileLabel_->setFont( QtUtil::titleFont( fileLabel_->font() ) );
    fileLabel_->setMargin( 1 );

    // separator
    vLayout->addWidget( separator_ = new QFrame( this ) );
    separator_->setFrameStyle( QFrame::HLine );

    // grid layout
    auto gridLayout = new GridLayout;
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignVCenter|Qt::AlignRight );
    gridLayout->setColumnAlignment( 1, Qt::AlignVCenter|Qt::AlignLeft );
    gridLayout->setMargin( 0 );
    gridLayout->setSpacing( 5 );
    vLayout->addLayout( gridLayout );

    // items
    ( typeItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Type:" ) );
    ( sizeItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Size:" ) );
    ( lastModifiedItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Modified:" ) );
    ( userItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Owner:" ) );
    ( groupItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Group:" ) );
    ( permissionsItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Permissions:" ) );

    // add stretch
    vLayout->addStretch( 1 );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &BaseFileInfoToolTipWidget::_updateConfiguration );
    _updateConfiguration();

}

//_______________________________________________________
void BaseFileInfoToolTipWidget::setFileInfo( const BaseFileInfo& fileInfo, const QIcon& icon )
{
    Debug::Throw( "BaseFileInfoToolTipWidget::setFileInfo.\n" );

    // local storage
    icon_ = icon;
    fileInfo_ = fileInfo;

    // update icon
    if( !icon.isNull() )
    {

        // get pixmap
        CustomPixmap pixmap( icon.pixmap( QSize( pixmapSize_, pixmapSize_ ) ) );

        // add effects
        const int type( fileInfo.type() );
        if( type & BaseFileInfo::Link ) pixmap = BaseFileIconProvider::linked( pixmap );
        if( type & BaseFileInfo::Hidden ) pixmap = BaseFileIconProvider::hidden( pixmap );

        iconLabel_->setPixmap( pixmap );
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
        if( (mask_&Size) && fileInfo.size() > 0 && !( fileInfo.type() & (BaseFileInfo::Folder|BaseFileInfo::Link|BaseFileInfo::Navigator) ) )
        {

            sizeItem_->setText( File::sizeString( fileInfo.size() ) );

        } else sizeItem_->hide();

        // last modified
        if( (mask_&Modified) && TimeStamp( fileInfo.lastModified() ).isValid() )
        {

            lastModifiedItem_->setText( TimeStamp( fileInfo.lastModified() ).toString() );

        } else lastModifiedItem_->hide();

        // user
        if( (mask_&User) && !fileInfo.user().isEmpty() ) userItem_->setText( fileInfo.user() );
        else userItem_->hide();

        // group
        if( (mask_&Group) && !fileInfo.group().isEmpty() ) groupItem_->setText( fileInfo.group() );
        else groupItem_->hide();

        // permissions
        QString permissions;
        if( (mask_&Permissions) && !( permissions = fileInfo.permissionsString() ).isEmpty() ) permissionsItem_->setText( permissions );
        else permissionsItem_->hide();

    } else {

        // file and separator
        fileLabel_->hide();
        separator_->hide();

        // items
        typeItem_->hide();
        sizeItem_->hide();
        lastModifiedItem_->hide();
        userItem_->hide();
        groupItem_->hide();
        permissionsItem_->hide();
    }

    adjustSize();

}

//_____________________________________________
void BaseFileInfoToolTipWidget::_updateConfiguration()
{
    Debug::Throw( "BaseFileInfoToolTipWidget::_updateConfiguration.\n" );
    bool modified( false );

    // mask from options
    if( XmlOptions::get().contains( "TOOLTIPS_MASK" ) )
    {
        auto mask( static_cast<Types>( XmlOptions::get().get<int>( "TOOLTIPS_MASK" ) ) );
        if( mask != mask_ )
        {
            mask_ = mask;
            modified = true;
        }
    }

    // get bits from mask
    int lineCount( 1 );
    for( const auto& bit:{ Size, Modified, User, Group, Permissions } )
    { if( mask_ & bit ) ++lineCount; }

    // compute pixmap size
    const int pixmapSize = QFontMetrics( QtUtil::titleFont( font() ) ).height() + lineCount*( fontMetrics().height() + 5 ) + 15;
    if( pixmapSize_ != pixmapSize )
    {
        pixmapSize_ = pixmapSize;
        modified = true;
    }

    if( modified ) _reload();

}
