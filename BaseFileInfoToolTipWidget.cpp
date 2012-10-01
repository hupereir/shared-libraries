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

#include "BaseFileInfoToolTipWidget.h"
#include "Debug.h"
#include "GridLayout.h"
#include "Singleton.h"
#include "TimeStamp.h"
#include "XmlOptions.h"

#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFrame>
#include <QtGui/QLayout>
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionFrame>
#include <QtGui/QToolTip>

class Item: public QObject, public Counter
{
    public:

    //! constructor
    Item( QWidget* parent, GridLayout* layout ):
        QObject( parent ),
        Counter( "BaseFileInfoToolTipWidget::Item" )
    {
        layout->addWidget( key_ = new QLabel( parent ) );
        layout->addWidget( value_ = new QLabel( parent ) );
    }

    //! destructor
    virtual ~Item( void )
    {}

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

//_______________________________________________________
BaseFileInfoToolTipWidget::BaseFileInfoToolTipWidget( QWidget* parent ):
    QWidget( parent, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint ),
    Counter( "BaseFileInfoToolTipWidget" ),
    enabled_( false ),
    pixmapSize_( 96 ),
    mask_( Default )
{

    Debug::Throw( "BaseFileInfoToolTipWidget::BaseFileInfoToolTipWidget.\n" );
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
    ( userItem_ = new Item( this, gridLayout ) )->setKey( "Owner:" );
    ( groupItem_ = new Item( this, gridLayout ) )->setKey( "Group:" );
    ( permissionsItem_ = new Item( this, gridLayout ) )->setKey( "Permissions:" );

    // add stretch
    vLayout->addStretch( 1 );

    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged( void ) ), SLOT( _updateConfiguration( void ) ) );
    _updateConfiguration();

}

//_____________________________________________
void BaseFileInfoToolTipWidget::setEnabled( bool value )
{
    Debug::Throw( "BaseFileInfoToolTipWidget::setEnabled.\n" );
    if( enabled_ == value ) return;
    enabled_ = value;
    if( !enabled_ )
    {
        if( isVisible() ) hide();
        timer_.stop();
    }

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
        const QString permissions( fileInfo.permissionsString() );
        if( (mask_&Permissions) && !permissions.isEmpty() ) permissionsItem_->setText( permissions );
        else permissionsItem_->hide();

    } else {

        // file and separator
        fileLabel_->hide();
        separator_->hide();

        // items
        typeItem_->hide();
        sizeItem_->hide();
        lastModifiedItem_->hide();
        permissionsItem_->hide();
    }

    adjustSize();

}

//_______________________________________________________
void BaseFileInfoToolTipWidget::adjustPosition( const QRect& rect )
{

    // get tooltip size
    const QSize size( sizeHint() );

    // desktop size
    QDesktopWidget* desktop( qApp->desktop() );
    QRect desktopGeometry( desktop->screenGeometry( desktop->screenNumber( parentWidget() ) ) );

    // set geometry
    int left = rect.left() + ( rect.width() - size.width() )/2;
    left = qMax( left, desktopGeometry.left() );
    left = qMin( left, desktopGeometry.right() - size.width() );

    // first try placing widget below item
    const int margin = 5;
    int top = rect.bottom() + margin;
    if( top > desktopGeometry.bottom() - size.height() ) top = rect.top() - margin - size.height();

    move( QPoint( left, top ) );

}

//_______________________________________________________
void BaseFileInfoToolTipWidget::paintEvent( QPaintEvent* event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    QStyleOptionFrame opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_PanelTipLabel, &opt, &painter, this );
    return QWidget::paintEvent( event );
}


//_____________________________________________
void BaseFileInfoToolTipWidget::timerEvent( QTimerEvent* event )
{
    if( event->timerId() == timer_.timerId() )
    {

        timer_.stop();
        show();
        return;

    } else return QWidget::timerEvent( event );
}

//_____________________________________________
void BaseFileInfoToolTipWidget::_updateConfiguration( void )
{
    Debug::Throw( "BaseFileInfoToolTipWidget::_updateConfiguration.\n" );
    if( XmlOptions::get().contains( "SHOW_TOOLTIPS" ) ) setEnabled( XmlOptions::get().get<bool>( "SHOW_TOOLTIPS" ) );
    if( XmlOptions::get().contains( "TOOLTIPS_PIXMAP_SIZE" ) ) setPixmapSize( XmlOptions::get().get<unsigned int>( "TOOLTIPS_PIXMAP_SIZE" ) );
    if( XmlOptions::get().contains( "TOOLTIPS_MASK" ) ) setMask( XmlOptions::get().get<unsigned int>( "TOOLTIPS_MASK" ) );
}
