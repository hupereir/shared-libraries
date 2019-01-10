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

#include "MessageWidget.h"

#include "BaseIconNames.h"
#include "Color.h"
#include "Debug.h"
#include "IconEngine.h"
#include "IconSize.h"

#include <QPropertyAnimation>
#include <QPainter>

//___________________________________________________________
class MessageWidgetPrivate: public QObject, private Base::Counter<MessageWidgetPrivate>
{

    Q_OBJECT

    public:

    //* constructor
    MessageWidgetPrivate( MessageWidget* );

    //* setup animation
    void setupAnimation();

    //* get contents preferred height
    int preferredHeight() const;

    //* parent
    MessageWidget* parent_ = nullptr;

    //* content
    QWidget* content_ = nullptr;

    //* icon
    QLabel* iconLabel_ = nullptr;

    //* label
    QLabel* textLabel_ = nullptr;

    //* text layout
    QBoxLayout* textLayout_ = nullptr;

    //* buttons layout
    QBoxLayout* buttonLayout_ = nullptr;

    //* animation
    QPropertyAnimation* animation_ = nullptr;

    //* message type
    MessageWidget::MessageType messageType_ = MessageWidget::MessageType::Information;

    //* animation duration
    int duration_ = 150;

    public Q_SLOTS:

    //* animation finished
    void animationFinished();

};

//___________________________________________________________
MessageWidget::MessageWidget( QWidget* parent, MessageType type, const QString& text ):
    QWidget( parent ),
    Counter( "MessageWidget" ),
    private_( new MessageWidgetPrivate( this ) )
{

    Debug::Throw( "MessageWidget::MessageWidget.\n" );

    // reset policy and constrains
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    setMinimumHeight(0);
    setMaximumHeight( QWIDGETSIZE_MAX );

    setLayout( new QVBoxLayout );
    layout()->setSpacing(0);
    layout()->setMargin(0);
    layout()->addWidget( private_->content_ = new QWidget( this ) );

    // create vbox layout
    auto layout=new QVBoxLayout;
    layout->setSpacing(10);
    layout->setMargin(10);
    private_->content_->setLayout( layout );

    private_->textLayout_ = new QHBoxLayout;
    private_->textLayout_->setSpacing(10);
    private_->textLayout_->setMargin(0);
    layout->addLayout( private_->textLayout_, 1 );

    // icon label
    private_->textLayout_->addWidget( private_->iconLabel_ = new QLabel( private_->content_ ), 0 );
    private_->iconLabel_->hide();

    // create message
    private_->textLayout_->addWidget( private_->textLabel_ = new QLabel( private_->content_ ), 1 );
    private_->textLabel_->setTextInteractionFlags( Qt::TextSelectableByMouse );
    private_->textLabel_->setText( text );

    private_->textLabel_->setWordWrap( true );

    // button layout
    private_->buttonLayout_ = new QHBoxLayout;
    private_->buttonLayout_->setMargin(0);
    private_->buttonLayout_->setSpacing( 5 );
    layout->addLayout( private_->buttonLayout_, 0 );
    private_->buttonLayout_->addStretch( 1 );

    private_->messageType_ = type;

}

//___________________________________________________________
MessageWidget::~MessageWidget() = default;

//___________________________________________________________
MessageWidget::MessageType MessageWidget::messageType() const
{ return private_->messageType_; }

//___________________________________________________________
QString MessageWidget::text() const
{ return private_->textLabel_ ? private_->textLabel_->text() : QString(); }

//___________________________________________________________
void MessageWidget::setIcon( const QIcon& icon )
{

    Debug::Throw( "MessageWidget::setIcon.\n" );
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
void MessageWidget::setText( const QString& text )
{ private_->textLabel_->setText( text ); }


//___________________________________________________________
QToolButton* MessageWidget::addDefaultCloseButton()
{

    Debug::Throw( "InformationWidget::addDefaultCloseButton.\n" );
    auto button = new QToolButton( private_->content_ );
    button->setIcon( IconEngine::get( IconNames::DialogClose ) );
    button->setAutoRaise( true );
    private_->textLayout_->addWidget( button, 0, Qt::AlignTop );

    connect( button, SIGNAL(clicked()), this, SLOT(animatedHide()) );
    return button;
}

//___________________________________________________________
QPushButton* MessageWidget::addButton( const QIcon& icon, const QString& text )
{
    Debug::Throw( "MessageWidget::addButton.\n" );
    auto button = new QPushButton( icon, text, private_->content_ );
    private_->buttonLayout_->addWidget( button );
    return button;
}

//___________________________________________________________
void MessageWidget::clearButtons()
{
    Debug::Throw( "MessageWidget::clearButtons.\n" );
    for( auto&& button:private_->content_->findChildren<QAbstractButton*>() )
    {
        if( button->parentWidget() == private_->content_ )
        {
            button->hide();
            button->deleteLater();
        }
    }

}

//___________________________________________________________
MessageWidget::ButtonList MessageWidget::addButtons( const QStringList& textList )
{
    Debug::Throw( "MessageWidget::addButtons.\n" );
    ButtonList out;
    for( const auto& text:textList )
    { out.append( addButton( text ) ); }
    return out;
}

//___________________________________________________________
MessageWidget::ButtonList MessageWidget::setButtons( const QStringList& textList )
{
    Debug::Throw( "MessageWidget::setButtons.\n" );
    clearButtons();
    return addButtons( textList );
}

//___________________________________________________________
void MessageWidget::setDirection( QBoxLayout::Direction direction )
{ static_cast<QBoxLayout*>( private_->content_->layout() )->setDirection( direction ); }

//___________________________________________________________
void MessageWidget::setDuration( int duration )
{ private_->duration_ = duration; }

//___________________________________________________________
void MessageWidget::animatedShow()
{

    Debug::Throw( "MessageWidget::animatedShow.\n" );

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

    // change policy
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setFixedHeight(0);
    show();

    // setup animation
    private_->setupAnimation();
    private_->animation_->setDirection( QPropertyAnimation::Forward );

    // animate
    private_->animation_->start();
    private_->content_->hide();

}

//___________________________________________________________
void MessageWidget::animatedHide()
{

    Debug::Throw( "MessageWidget::animatedHide.\n" );

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
    private_->content_->hide();

}

//___________________________________________________________
void MessageWidget::paintEvent( QPaintEvent* event )
{

    const auto baseColor = palette().color( QPalette::Active, QPalette::Window );

    QColor outlineColor;
    switch( private_->messageType_ )
    {
        case MessageType::Positive:
        outlineColor.setRgb( 39, 174,  96 );
        break;

        case MessageType::Information:
        outlineColor = palette().color( QPalette::Active, QPalette::Highlight );
        break;

        case MessageType::Warning:
        outlineColor.setRgb( 246, 116, 0 );
        break;

        case MessageType::Error:
        outlineColor.setRgb( 218, 68, 83 );
        break;

    }

    // background color
    Base::Color backgroundColor( outlineColor );

    const qreal alpha = 0.2;
    backgroundColor.merge( baseColor, alpha );

    // paint
    QPainter painter( this );
    painter.setClipRegion( event->region() );
    painter.setRenderHints( QPainter::Antialiasing );

    const qreal penWidth = 1;
    painter.setPen( QPen( outlineColor, penWidth ) );
    painter.setBrush( backgroundColor.get() );

    const qreal margin = penWidth/2 + 1;
    const qreal radius = 2;
    const auto rect = QRectF( this->rect() ).adjusted( margin, margin, -margin, -margin );
    painter.drawRoundedRect( rect, radius, radius );

}

//___________________________________________________________
MessageWidgetPrivate::MessageWidgetPrivate( MessageWidget* parent ):
    QObject( parent ),
    Counter( "MessageWidgetPrivate" ),
    parent_( parent )
{ Debug::Throw( "MessageWidgetPrivate::MessageWidgetPrivate.\n" ); }

//___________________________________________________________
int MessageWidgetPrivate::preferredHeight() const
{
    const auto height = content_->heightForWidth( parent_->width() );
    return height >= 0 ? height : content_->sizeHint().height();
}

//___________________________________________________________
void MessageWidgetPrivate::setupAnimation()
{
    if( !animation_ )
    {
        animation_ = new QPropertyAnimation( parent_, "height", this );
        connect( animation_, SIGNAL(finished()), SLOT(animationFinished()) );
    }

    // setup
    animation_->setStartValue(0);
    animation_->setEndValue( preferredHeight() );
    animation_->setDuration( duration_ );
}

//___________________________________________________________
void MessageWidgetPrivate::animationFinished()
{
    if( animation_ && animation_->direction() == QPropertyAnimation::Backward )
    {

        parent_->hide();
        emit parent_->hideAnimationFinished();

    } else {

        content_->show();
        emit parent_->showAnimationFinished();

    }

    // reset policy and constrains
    parent_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum );
    parent_->setMinimumHeight(0);
    parent_->setMaximumHeight( QWIDGETSIZE_MAX );

}

#include "MessageWidget.moc"
