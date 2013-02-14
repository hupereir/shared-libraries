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
*
*******************************************************************************/

#include "BaseFileInformationDialog.h"

#include "AnimatedTabWidget.h"
#include "BaseIcons.h"
#include "Debug.h"
#include "ElidedLabel.h"
#include "File.h"
#include "FilePermissionsWidget.h"
#include "GridLayout.h"
#include "IconEngine.h"

#include <QShortcut>

//____________________________________________________________________________
BaseFileInformationDialog::Item::Item( QWidget* parent, GridLayout* layout, BaseFileInformationDialog::ItemFlags flags ):
    QObject( parent ),
    Counter( "BaseFileInformationDialog::Item" ),
    flags_( flags )
{
    layout->addWidget( key_ = new QLabel( parent ), layout->currentRow(), layout->currentColumn(), Qt::AlignRight|Qt::AlignTop );

    // create correct value label
    if( flags & BaseFileInformationDialog::Elide ) value_ = new ElidedLabel( parent );
    else value_ = new QLabel( parent );

    // adjust font
    if( flags & BaseFileInformationDialog::Bold )
    {
        QFont font( value_->font() );
        font.setWeight( QFont::Bold );
        value_->setFont( font );
    }

    if( flags & BaseFileInformationDialog::Selectable )
    { value_->setTextInteractionFlags( Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard ); }

    // add to layout
    layout->addWidget( value_, layout->currentRow(), layout->currentColumn(), Qt::AlignLeft|Qt::AlignTop );

}

//____________________________________________________________________________
void BaseFileInformationDialog::Item::setKey( const QString& value )
{ key_->setText( value ); }

//____________________________________________________________________________
void BaseFileInformationDialog::Item::setValue( const QString& value )
{

    // assign text
    ElidedLabel* label( qobject_cast<ElidedLabel*>( value_ ) );
    if( label ) label->ElidedLabel::setText( value );
    else value_->setText( value );

    // update visibility
    setVisible( !value.isEmpty() );

}

//____________________________________________________________________________
void BaseFileInformationDialog::Item::setVisible( bool value )
{
    if( value ) show();
    else hide();
}

//____________________________________________________________________________
void BaseFileInformationDialog::Item::show( void )
{
    key_->show();
    value_->show();
}

//____________________________________________________________________________
void BaseFileInformationDialog::Item::hide( void )
{
    key_->hide();
    value_->hide();
}

//_________________________________________________________
BaseFileInformationDialog::BaseFileInformationDialog( QWidget* parent ):
    CustomDialog( parent, CloseButton )
{

    Debug::Throw( "BaseFileInformationDialog::BaseFileInformationDialog.\n" );
    setOptionName( "FILE_INFORMATION_DIALOG" );

    // customize layout
    layout()->setMargin(0);
    layout()->setSpacing(0);
    buttonLayout().setMargin(5);

    tabWidget_ = new AnimatedTabWidget( this );
    mainLayout().addWidget( tabWidget_ );

    // general information
    tabWidget_->addTab( mainPage_ = new QWidget(), "General" );
    pageLayout_ = new QVBoxLayout();
    pageLayout_->setMargin(5);
    pageLayout_->setSpacing(5);
    mainPage_->setLayout( pageLayout_ );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(0);
    hLayout->setSpacing(5);
    pageLayout_->addLayout( hLayout );
    pageLayout_->addStretch();

    // try load Question icon
    iconLabel_ = new QLabel(mainPage_);
    iconLabel_->setPixmap( IconEngine::get( ICONS::INFORMATION ).pixmap( iconSize() ) );
    hLayout->addWidget( iconLabel_, 0, Qt::AlignTop );
    hLayout->addStretch();

    gridLayout_ = new GridLayout();
    gridLayout_->setSpacing( 5 );
    gridLayout_->setMaxCount( 2 );
    gridLayout_->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    hLayout->addLayout( gridLayout_ );
    hLayout->addStretch();

    // store all items in array, for visibility
    QList<Item*> items;

    items << (fileItem_ = new Item( mainPage_, gridLayout_, Bold|Selectable|Elide ) );
    items << (typeItem_ = new Item( mainPage_, gridLayout_ ) );
    items << (pathItem_ = new Item( mainPage_, gridLayout_, Selectable|Elide ) );
    items << (sizeItem_ = new Item( mainPage_, gridLayout_, Selectable ) );
    items << (createdItem_ = new Item( mainPage_, gridLayout_ ) );
    items << (modifiedItem_ = new Item( mainPage_, gridLayout_ ) );
    items << (accessedItem_ = new Item( mainPage_, gridLayout_ ) );

    fileItem_->setKey( "File name:" );
    typeItem_->setKey( "Type:" );
    pathItem_->setKey( "Location:" );
    sizeItem_->setKey( "Size:" );
    createdItem_->setKey( "Created:" );
    accessedItem_->setKey( "Accessed:" );
    modifiedItem_->setKey( "Modified:" );

    // permissions tab
    QWidget* box;
    QVBoxLayout* layout;
    tabWidget_->addTab( box = new QWidget(), "Permissions" );
    layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->setSpacing( 5 );
    box->setLayout( layout );

    layout->addWidget( permissionsWidget_ = new FilePermissionsWidget( box ) );

    // user and group
    QLabel* label;
    layout->addWidget( label = new QLabel( "Ownership:", box ) );
    {
        QFont font( label->font() );
        font.setWeight( QFont::Bold );
        label->setFont( font );
    }

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setMargin(0);
    gridLayout->setSpacing( 5 );
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    layout->addItem( gridLayout );

    items << (userItem_ = new Item( box, gridLayout ) );
    items << (groupItem_ = new Item( box, gridLayout ) );
    userItem_->setKey( "User:" );
    groupItem_->setKey( "Group:" );

    gridLayout->setColumnStretch( 1, 1 );
    layout->addStretch();

    // hide everything, they are shown when proper values are set
    foreach( Item* item, items )
    { item->hide(); }

    permissionsWidget_->hide();

    // connections
    connect( &closeButton(), SIGNAL( pressed( void ) ), SLOT( close( void ) ) );
    new QShortcut( QKeySequence::Close, this, SLOT( close() ) );

}

//_________________________________________________________
void BaseFileInformationDialog::setIcon( const QIcon& icon)
{ if( !icon.isNull() ) iconLabel_->setPixmap( icon.pixmap( iconSize() ) ); }

//_________________________________________________________
void BaseFileInformationDialog::setFile( const QString& value )
{ fileItem_->setValue( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setPath( const QString& value )
{ pathItem_->setValue( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setType( const QString& value )
{ typeItem_->setValue( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setSize( qint64 size )
{
    if( size > 0 )
    {
        // format size to have space characters every three digits
        QString sizeString = QString().setNum( size );
        int length( sizeString.length() );
        for( int i = 1; i < length; i++ )
        { if( !(i%3) ) sizeString.insert( sizeString.size() - (i + i/3 - 1), ' ' ); }

        QString buffer;
        QTextStream( &buffer ) << File::sizeString( size ) << " (" << sizeString << ")";
        sizeItem_->setValue( buffer );

    } else sizeItem_->setValue( QString() );
}

//_________________________________________________________
void BaseFileInformationDialog::setCreated( TimeStamp timeStamp )
{ createdItem_->setValue( timeStamp.isValid() ? timeStamp.toString():QString() ); }

//_________________________________________________________
void BaseFileInformationDialog::setAccessed( TimeStamp timeStamp )
{ accessedItem_->setValue( timeStamp.isValid() ? timeStamp.toString():QString() ); }

//_________________________________________________________
void BaseFileInformationDialog::setModified( TimeStamp timeStamp )
{ modifiedItem_->setValue( timeStamp.isValid() ? timeStamp.toString():QString() ); }

//_________________________________________________________
void BaseFileInformationDialog::setPermissions( QFile::Permissions permissions )
{
    permissionsWidget_->show();
    permissionsWidget_->setPermissions( permissions );
}

//_________________________________________________________
void BaseFileInformationDialog::setUser( const QString& value )
{ userItem_->setValue( value ); }

//_________________________________________________________
void BaseFileInformationDialog::setGroup( const QString& value )
{ groupItem_->setValue( value ); }

//_________________________________________________________
int BaseFileInformationDialog::addRow( const QString& key, const QString& value, ItemFlags flags )
{

    // setup new item
    Item* item = new Item( mainPage_, gridLayout_, flags );
    item->setKey( key );
    item->setValue( value );

    // append to list
    extraItems_ << item;

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
    { extraItems_[index]->setValue( value ); }
}
