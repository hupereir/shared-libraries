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
#include "FilePermissionsWidget.h"
#include "GridLayout.h"
#include "IconEngine.h"

#include <QtGui/QShortcut>

//_____________________________________________________________
class BaseInformationDialogItem: public QObject, public Counter
{
    public:

    //! flags
    enum Flag
    {
        None = 0,
        Elide = 1<<0,
        Bold = 1<<1,
        Selectable = 1<<2,
        All = Elide|Bold|Selectable
    };

    Q_DECLARE_FLAGS( Flags, Flag );

    //! constructor
    BaseInformationDialogItem( QWidget* parent, GridLayout* layout, Flags flags = None ):
        QObject( parent ),
        Counter( "BaseInformationDialogItem" )
    {
        layout->addWidget( key_ = new QLabel( parent ), layout->currentRow(), layout->currentColumn(), Qt::AlignRight|Qt::AlignTop );

        // create correct value label
        if( flags & Elide ) value_ = new ElidedLabel( parent );
        else value_ = new QLabel( parent );

        // adjust font
        if( flags & Bold )
        {
            QFont font( value_->font() );
            font.setWeight( QFont::Bold );
            value_->setFont( font );
        }

        if( flags & Selectable )
        { label->setTextInteractionFlags( Qt::TextSelectableByMouse|Qt::TextSelectableByKeyboard ); }

        // add to layout
        layout->addWidget( value_, layout->currentRow(), layout->currentColumn(), Qt::AlignLeft|Qt::AlignTop );

    }

    //! destructor
    virtual ~BaseInformationDialogItem( void )
    {}

    //! set visible
    void setVisible( bool value )
    {
        if( value ) show();
        else hide();
    }

    //! show
    void show( void )
    {
        key_->show();
        value_->show();
    }

    //! hide
    void hide( void )
    {
        key_->hide();
        value_->hide();
    }

    //! set key
    void setKey( const QString& value )
    { key_->setText( value ); }

    //! set text
    void setText( const QString& value )
    {
        value_->setText( value );
        if( value.isEmpty() ) hide();
        else show();
    }

    private:

    QLabel* key_;
    QLabel* value_;

};

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
    QWidget *box;
    tabWidget_->addTab( box = new QWidget(), "General" );

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setMargin(5);
    hLayout->setSpacing(5);
    box->setLayout( hLayout );

    // try load Question icon
    iconLabel_ = new QLabel(box);
    iconLabel_->setPixmap( IconEngine::get( ICONS::INFORMATION ).pixmap( iconSize() ) );
    hLayout->addWidget( iconLabel_, 0, Qt::AlignTop );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing( 5 );
    hLayout->addLayout( layout, 1 );

    GridLayout* gridLayout = new GridLayout();
    gridLayout->setSpacing( 5 );
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    layout->addLayout( gridLayout );

    // store all items in array, for visibility
    QList<BaseInformationDialogItem*> items;

    items << (fileItem_ = new BaseInformationDialogItem( box, gridLayout, BaseInformationDialogItem::All ) );
    items << (pathItem_ = new BaseInformationDialogItem( box, gridLayout, BaseInformationDialogItem::Elide|BaseInformationDialogItem::Selectable ) );
    items << (typeItem_ = new BaseInformationDialogItem( box, gridLayout ) );
    items << (sizeItem_ = new BaseInformationDialogItem( box, gridLayout ) );
    items << (createdItem_ = new BaseInformationDialogItem( box, gridLayout ) );
    items << (accessedItem_ = new BaseInformationDialogItem( box, gridLayout ) );
    items << (modifiedItem_ = new BaseInformationDialogItem( box, gridLayout ) );

    // permissions tab
    tabWidget_->addTab( box = new QWidget(), "Permissions" );
    layout = new QVBoxLayout();
    layout->setMargin(5);
    layout->setSpacing( 5 );
    box->setLayout( layout );

    layout->addWidget( permissionsWidget_ = new FilePermissionsWidget( box, fileInfo.permissions() ) );

    layout->addWidget( new QLabel( "<b>Ownership: </b>", box ) );

    gridLayout = new GridLayout();
    gridLayout->setMargin(0);
    gridLayout->setSpacing( 5 );
    gridLayout->setMaxCount( 2 );
    gridLayout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );
    layout->addItem( gridLayout );

    items << (userItem_ = new BaseInformationDialogItem( box, gridLayout ) );
    items << (groupItem_ = new BaseInformationDialogItem( box, gridLayout ) );

    // hide everything, they are shown when proper values are set
    foreach( BaseInformationDialogItem* item, items )
    { item->hide(); }

    permissionsWidget_->hide();

}

//_________________________________________________________
void BaseFileInformationDialog::setIcon( const QIcon& icon)
{ if( !icon.isNull() ) iconLabel_->setPixmap( icon.pixmap( iconSize() ) )

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
    if( size > 0 )
    {


    } else sizeItem_->setText( QString() );
}

//_________________________________________________________
void BaseFileInformationDialog::setCreated( TimeStamp );

//_________________________________________________________
void BaseFileInformationDialog::setAccessed( TimeStamp );

//_________________________________________________________
void BaseFileInformationDialog::setModified( TimeStamp );

//_________________________________________________________
void BaseFileInformationDialog::setPermissions( const QString& );

//_________________________________________________________
void BaseFileInformationDialog::setUser( const QString& );

//_________________________________________________________
void BaseFileInformationDialog::setGroup( const QString& );
