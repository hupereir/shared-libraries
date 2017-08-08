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

#include "BaseFileInformationDialog.h"

#include "BaseIconNames.h"
#include "CppUtil.h"
#include "Debug.h"
#include "ElidedLabel.h"
#include "File.h"
#include "FilePermissionsWidget.h"
#include "GridLayout.h"
#include "IconEngine.h"
#include "IconSize.h"

#include <QShortcut>

//_________________________________________________________
BaseFileInformationDialog::BaseFileInformationDialog( QWidget* parent ):
    CustomDialog( parent, CloseButton )
{

    Debug::Throw( "BaseFileInformationDialog::BaseFileInformationDialog.\n" );
    setOptionName( "FILE_INFORMATION_DIALOG" );
    setIconSize( IconSize::get( IconSize::Oversized ) );

    // customize layout
    layout()->setMargin(0);
    layout()->setSpacing(0);
    buttonLayout().setMargin(5);

    tabWidget_ = new QTabWidget( this );
    mainLayout().addWidget( tabWidget_ );

    // general information
    tabWidget_->addTab( mainPage_ = new QWidget, tr( "General" ) );
    pageLayout_ = new QVBoxLayout;
    pageLayout_->setMargin(5);
    pageLayout_->setSpacing(5);
    mainPage_->setLayout( pageLayout_ );

    auto hLayout = new QHBoxLayout;
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    pageLayout_->addLayout( hLayout );
    pageLayout_->addStretch();

    // try load Question icon
    iconLabel_ = new QLabel(mainPage_);
    iconLabel_->setPixmap( IconEngine::get( IconNames::DialogInformation ).pixmap( iconSize() ) );
    hLayout->addWidget( iconLabel_, 0, Qt::AlignTop );
    hLayout->addStretch();

    auto vLayout= new QVBoxLayout;
    vLayout->setMargin(0);
    vLayout->setSpacing(5);
    hLayout->addLayout( vLayout );
    hLayout->addStretch();

    gridLayout_ = new GridLayout;
    gridLayout_->setSpacing( 5 );
    gridLayout_->setMaxCount( 2 );
    gridLayout_->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    vLayout->addLayout( gridLayout_ );
    vLayout->addStretch();

    // store all items in array, for visibility
    using GridLayoutItemList = QList<GridLayoutItem*>;
    auto items = Base::makeT<GridLayoutItemList>({
        fileItem_ = new GridLayoutItem( mainPage_, gridLayout_ ),
        typeItem_ = new GridLayoutItem( mainPage_, gridLayout_ ),
        pathItem_ = new GridLayoutItem( mainPage_, gridLayout_, GridLayoutItem::Flag::Selectable|GridLayoutItem::Flag::Elide ),
        sizeItem_ = new GridLayoutItem( mainPage_, gridLayout_, GridLayoutItem::Flag::Selectable ),
        createdItem_ = new GridLayoutItem( mainPage_, gridLayout_ ),
        modifiedItem_ = new GridLayoutItem( mainPage_, gridLayout_ ),
        accessedItem_ = new GridLayoutItem( mainPage_, gridLayout_ )
    });

    fileItem_->setKey( tr( "File name:" ) );
    typeItem_->setKey( tr( "Type:" ) );
    pathItem_->setKey( tr( "Location:" ) );
    sizeItem_->setKey( tr( "Size:" ) );
    createdItem_->setKey( tr( "Created:" ) );
    accessedItem_->setKey( tr( "Accessed:" ) );
    modifiedItem_->setKey( tr( "Modified:" ) );

    // permissions tab
    QWidget* box;
    tabWidget_->addTab( box = new QWidget, tr( "Permissions" ) );

    auto layout = new QVBoxLayout;
    layout->setMargin(5);
    layout->setSpacing( 5 );
    box->setLayout( layout );

    layout->addWidget( permissionsWidget_ = new FilePermissionsWidget( box ) );

    // user and group
    layout->addWidget( new QLabel( tr( "Ownership:" ), box ) );

    auto gridLayout = new GridLayout;
    gridLayout->setMargin(0);
    gridLayout->setSpacing( 5 );
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    layout->addItem( gridLayout );

    items.append( Base::makeT<GridLayoutItemList>({
        userItem_ = new GridLayoutItem( box, gridLayout ),
        groupItem_ = new GridLayoutItem( box, gridLayout )
    }) );

    userItem_->setKey( tr( "User:" ) );
    groupItem_->setKey( tr( "Group:" ) );

    gridLayout->setColumnStretch( 1, 1 );
    layout->addStretch();

    // hide everything, they are shown when proper values are set
    for( const auto& item:items )
    { item->hide(); }

    permissionsWidget_->hide();

    // connections
    connect( &closeButton(), SIGNAL(pressed()), SLOT(close()) );
    new QShortcut( QKeySequence::Close, this, SLOT(close()) );

}

//_________________________________________________________
void BaseFileInformationDialog::setIcon( const QIcon& icon)
{ if( !icon.isNull() ) iconLabel_->setPixmap( icon.pixmap( iconSize() ) ); }

//_________________________________________________________
void BaseFileInformationDialog::setFile( const QString& value )
{ fileItem_->setText( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setPath( const QString& value )
{ pathItem_->setText( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setType( const QString& value )
{ typeItem_->setText( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setSize( qint64 size )
{
    if( size > 0 ) sizeItem_->setText( QString( "%1 (%2)" ).arg( File::sizeString( size ) ).arg( File::rawSizeString( size ) ) );
    else sizeItem_->setText( QString() );
}

//_________________________________________________________
void BaseFileInformationDialog::setCreated( TimeStamp timeStamp )
{ createdItem_->setText( timeStamp.isValid() ? timeStamp.toString():QString() ); }

//_________________________________________________________
void BaseFileInformationDialog::setAccessed( TimeStamp timeStamp )
{ accessedItem_->setText( timeStamp.isValid() ? timeStamp.toString():QString() ); }

//_________________________________________________________
void BaseFileInformationDialog::setModified( TimeStamp timeStamp )
{ modifiedItem_->setText( timeStamp.isValid() ? timeStamp.toString():QString() ); }

//_________________________________________________________
void BaseFileInformationDialog::setPermissions( QFile::Permissions permissions )
{
    permissionsWidget_->show();
    permissionsWidget_->setPermissions( permissions );
}

//_________________________________________________________
void BaseFileInformationDialog::setUser( const QString& value )
{ userItem_->setText( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setGroup( const QString& value )
{ groupItem_->setText( value ); }

//_________________________________________________________
int BaseFileInformationDialog::addRow( const QString& key, const QString& value, GridLayoutItem::Flags flags )
{

    // setup new item
    auto item = new GridLayoutItem( mainPage_, gridLayout_, flags );
    item->setKey( key );
    item->setText( value );

    // append to list
    extraItems_.append( item );

    // return id
    return extraItems_.size() - 1;
}

//_________________________________________________________
void BaseFileInformationDialog::setCustomKey( int index, const QString& value )
{
    if( index >= 0 && index < extraItems_.size() )
    { extraItems_[index]->setKey( value ); }
}

//_________________________________________________________
void BaseFileInformationDialog::setCustomValue( int index, const QString& value )
{
    if( index >= 0 && index < extraItems_.size() )
    { extraItems_[index]->setText( value ); }
}
