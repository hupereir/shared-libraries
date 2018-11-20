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

#include "InformationWidget.h"

#include "Color.h"
#include "Debug.h"
#include "IconSize.h"

#include <QPropertyAnimation>
#include <QPainter>

//___________________________________________________________
class ContentWidget: public QWidget, private Base::Counter<ContentWidget>
{

    public:

    //* constructor
    ContentWidget( QWidget* );

    protected:

    //* paint
    void paintEvent( QPaintEvent* ) override;

};

//___________________________________________________________
class InformationWidgetPrivate: public QObject, private Base::Counter<InformationWidgetPrivate>
{

    Q_OBJECT

    public:

    //* constructor
    InformationWidgetPrivate( QWidget* );

    //* setup animation
    void setupAnimation();

    //* get contents preferred height
    int preferredHeight() const;

    //* parent
    QWidget* parent_ = nullptr;

    //* content
    QWidget* content_ = nullptr;

    //* icon
    QLabel* iconLabel_ = nullptr;

    //* label
    QLabel* textLabel_ = nullptr;

    //* buttons layout
    QBoxLayout* buttonLayout_ = nullptr;

    //* animation
    QPropertyAnimation* animation_ = nullptr;

    public Q_SLOTS:

    //* animation finished
    void animationFinished();

};

//___________________________________________________________
InformationWidget::InformationWidget( QWidget* parent, const QString& text ):
QWidget( parent ),
Counter( "InformationWidget" ),
private_( new InformationWidgetPrivate( this ) )
{

    Debug::Throw( "InformationWidget::InformationWidget.\n" );

    setLayout( new QVBoxLayout );
    layout()->setSpacing(0);
    layout()->setMargin(0);
    layout()->addWidget( private_->content_ = new ContentWidget( this ) );

    // create vbox layout
    auto layout=new QVBoxLayout;
    layout->setSpacing(10);
    layout->setMargin(10);
    private_->content_->setLayout( layout );

    auto hLayout = new QHBoxLayout;
    hLayout->setSpacing(10);
    hLayout->setMargin(0);
    layout->addLayout( hLayout, 1 );

    // icon label
    hLayout->addWidget( private_->iconLabel_ = new QLabel( private_->content_ ), 0 );
    private_->iconLabel_->hide();

    // create message
    hLayout->addWidget( private_->textLabel_ = new QLabel( private_->content_ ), 1 );
    private_->textLabel_->setTextInteractionFlags( Qt::TextSelectableByMouse );
    private_->textLabel_->setWordWrap( true );
    private_->textLabel_->setText( text );

    // button layout
    private_->buttonLayout_ = new QHBoxLayout;
    private_->buttonLayout_->setMargin(0);
    private_->buttonLayout_->setSpacing( 5 );
    layout->addLayout( private_->buttonLayout_, 0 );
    private_->buttonLayout_->addStretch( 1 );

}

//___________________________________________________________
InformationWidget::~InformationWidget() = default;

//___________________________________________________________
void InformationWidget::setIcon( const QIcon& icon )
{

    Debug::Throw( "InformationWidget::setIcon.\n" );
    if( icon.isNull() )
    {

        private_->iconLabel_->clear();
        private_->iconLabel_->hide();

    } else {

        private_->iconLabel_->setPixmap( icon.pixmap( IconSize( IconSize::Large ).get() ) );
        private_->iconLabel_->show();

    }

}

//___________________________________________________________
void InformationWidget::setText( const QString& text )
{ private_->textLabel_->setText( text ); }

//___________________________________________________________
QPushButton* InformationWidget::addButton( const QIcon& icon, const QString& text )
{
    auto button = new QPushButton( icon, text, private_->content_ );
    private_->buttonLayout_->addWidget( button );
    return button;
}

//___________________________________________________________
void InformationWidget::setDirection( QBoxLayout::Direction direction )
{ static_cast<QBoxLayout*>( private_->content_->layout() )->setDirection( direction ); }

//___________________________________________________________
void InformationWidget::showAnimated()
{

    Debug::Throw( "InformationWidget::showAnimated.\n" );

    // check animation
    if( private_->animation_ &&
        private_->animation_->state() == QPropertyAnimation::Running )
    {

        if( private_->animation_->direction() == QPropertyAnimation::Backward )
        { private_->animation_->setDirection( QPropertyAnimation::Forward ); }

        return;
    }

    // check visibility
    if( isVisible() ) return;

    // setup animation
    private_->setupAnimation();
    private_->animation_->setDirection( QPropertyAnimation::Forward );

    // changed policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    show();

    // animate
    private_->animation_->start();

}

//___________________________________________________________
void InformationWidget::hideAnimated()
{

    Debug::Throw( "InformationWidget::hideAnimated.\n" );

    // check animation
    if( private_->animation_ &&
        private_->animation_->state() == QPropertyAnimation::Running )
    {

        if( private_->animation_->direction() == QPropertyAnimation::Forward )
        { private_->animation_->setDirection( QPropertyAnimation::Backward ); }

        return;
    }

    // check visibility
    if( isHidden() ) return;

    // setup animation
    private_->setupAnimation();
    private_->animation_->setDirection( QPropertyAnimation::Backward );

    // change policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    // animate
    private_->animation_->start();

}

//___________________________________________________________
InformationWidgetPrivate::InformationWidgetPrivate( QWidget* parent ):
    QObject( parent ),
    Counter( "InformationWidgetPrivate" ),
    parent_( parent )
{ Debug::Throw( "InformationWidgetPrivate::InformationWidgetPrivate.\n" ); }

//___________________________________________________________
int InformationWidgetPrivate::preferredHeight() const
{ return content_->sizeHint().height(); }

//___________________________________________________________
void InformationWidgetPrivate::setupAnimation()
{
    if( !animation_ )
    {
        animation_ = new QPropertyAnimation( parent_, "height", this );
        connect( animation_, SIGNAL(finished()), SLOT(animationFinished()) );
    }

    // setup
    content_->ensurePolished();
    animation_->setStartValue(0);
    animation_->setEndValue( preferredHeight() );
    animation_->setDuration( 100 );
}

//___________________________________________________________
void InformationWidgetPrivate::animationFinished()
{
    if( animation_ && animation_->direction() == QPropertyAnimation::Backward )
    { parent_->hide(); }

    parent_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
}

//___________________________________________________________
ContentWidget::ContentWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "ContentWidget" )
{ Debug::Throw( "ContentWidget::ContentWidget.\n" ); }

//___________________________________________________________
void ContentWidget::paintEvent( QPaintEvent* event )
{

    const auto baseColor = palette().color( QPalette::Active, QPalette::Window );
    const auto outlineColor = palette().color( QPalette::Active, QPalette::Highlight );
    Base::Color backgroundColor( outlineColor );

    const qreal alpha = 0.2;
    backgroundColor.merge( baseColor, alpha );

    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.setRenderHints( QPainter::Antialiasing );

    const qreal penWidth = 2;
    painter.setPen( QPen( outlineColor, penWidth ) );
    painter.setBrush( backgroundColor.get() );

    const qreal margin = penWidth/2 + 1;
    const qreal radius = 4;
    const auto rect = QRectF( this->rect() ).adjusted( margin, margin, -margin, -margin );
    painter.drawRoundedRect( rect, radius, radius );

}

#include "InformationWidget.moc"
