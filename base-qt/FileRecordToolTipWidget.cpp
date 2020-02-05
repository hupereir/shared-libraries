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

#include "FileRecordToolTipWidget.h"

#include "BaseFileInfo.h"
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
FileRecordToolTipWidget::FileRecordToolTipWidget( QWidget* parent ):
    BaseToolTipWidget( parent )
{

    Debug::Throw( QStringLiteral("FileRecordToolTipWidget::FileRecordToolTipWidget.\n") );

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
    ( pathItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Path:" ) );
    ( sizeItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Size:" ) );
    ( lastModifiedItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Modified:" ) );
    ( userItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Owner:" ) );
    ( groupItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Group:" ) );
    ( permissionsItem_ = new GridLayoutItem( this, gridLayout ) )->setKey( tr( "Permissions:" ) );

    // add stretch
    vLayout->addStretch( 1 );

    // configuration
    connect( &Base::Singleton::get(), &Base::Singleton::configurationChanged, this, &FileRecordToolTipWidget::_updateConfiguration );
    _updateConfiguration();

}

//_______________________________________________________
void FileRecordToolTipWidget::setRecord( const FileRecord& record, const QIcon& icon )
{
    Debug::Throw( QStringLiteral("FileRecordToolTipWidget::setRecord.\n") );

    // local storage
    icon_ = icon;
    record_ = record;

    // update icon
    if( !icon.isNull() )
    {

        // get pixmap
        CustomPixmap pixmap( icon.pixmap( QSize( pixmapSize_, pixmapSize_ ) ) );

        // add effects
        const int type( record.flags() );
        if( type & BaseFileInfo::Link ) pixmap = BaseFileIconProvider::linked( pixmap );
        if( type & BaseFileInfo::Hidden ) pixmap = BaseFileIconProvider::hidden( pixmap );

        iconLabel_->setPixmap( pixmap );
        iconLabel_->show();

    } else iconLabel_->hide();

    if( !record.file().isEmpty() )
    {
        // file and separator
        fileLabel_->show();
        fileLabel_->setText( record.file().localName() );
        separator_->show();

        // type
        pathItem_->setText( record.file().path() );

        // size
        if( (mask_&Size) && record.file().fileSize() > 0 && !( record.file().isDirectory() || record.file().isLink() ) )
        {

            sizeItem_->setText( record.file().sizeString() );

        } else sizeItem_->hide();

        // last modified
        TimeStamp lastModified;
        if( (mask_&Modified) && ( lastModified = record.file().lastModified() ).isValid() )
        {

            lastModifiedItem_->setText( lastModified.toString() );

        } else lastModifiedItem_->hide();

        // user
        QString user;
        if( (mask_&User) && !( user = record.file().userName() ).isEmpty() ) userItem_->setText( user );
        else userItem_->hide();

        // group
        QString group;
        if( (mask_&Group) && !( group = record.file().groupName() ).isEmpty() ) groupItem_->setText( group );
        else groupItem_->hide();

        // permissions
        QString permissions;
        if( (mask_&Permissions) && !( permissions = record.file().permissionsString() ).isEmpty() ) permissionsItem_->setText( permissions );
        else permissionsItem_->hide();

    } else {

        // file and separator
        fileLabel_->hide();
        separator_->hide();

        // items
        pathItem_->hide();
        sizeItem_->hide();
        lastModifiedItem_->hide();
        permissionsItem_->hide();

    }

    adjustSize();

}

//_____________________________________________
void FileRecordToolTipWidget::_updateConfiguration()
{
    Debug::Throw( QStringLiteral("FileRecordToolTipWidget::_updateConfiguration.\n") );
    bool modified( false );

    // mask from options
    if( XmlOptions::get().contains( QStringLiteral("TOOLTIPS_MASK") ) )
    {
        auto mask( XmlOptions::get().get<int>( QStringLiteral("TOOLTIPS_MASK") ) );
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
