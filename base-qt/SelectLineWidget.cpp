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

#include "BaseIconNames.h"
#include "Color.h"
#include "IconEngine.h"
#include "LineEditor.h"
#include "QtUtil.h"
#include "SelectLineWidget.h"


#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QToolButton>
#include <QValidator>

//_______________________________________________________
SelectLineWidget::SelectLineWidget( QWidget* parent, bool compact ):
    EmbeddedWidget( parent ),
    Counter( QStringLiteral("SelectLineWidget") )
{

    Debug::Throw( QStringLiteral("SelectLineWidget::SelectLineWidget.\n") );

    // update palette
    _updateNotFoundPalette();

    QBoxLayout* layout;
    if( compact )
    {
        setLayout( layout = new QHBoxLayout );
        QtUtil::setMargin(layout, 2);
        layout->setSpacing(2);

    } else {

        setLayout( layout = new QVBoxLayout );
        QtUtil::setMargin(layout, 0);
        layout->setSpacing(5);

    }

    // insert text editor
    QLabel *label;
    layout->addWidget( label = new QLabel( tr( "Goto line number:" ), this ), 0 );

    layout->addWidget( editor_ = new LineEditor( this ), 1 );
    label->setBuddy( editor_ );

    connect( editor_, &LineEditor::cleared, this, &SelectLineWidget::matchFound );
    connect( editor_, &QLineEdit::returnPressed, this, &SelectLineWidget::_selectLine );
    connect( editor_, &QLineEdit::textChanged, this, &SelectLineWidget::_selectLine );

    QIntValidator *validator = new QIntValidator( this );
    validator->setBottom(0);
    editor_->setValidator( validator );

    if( compact )
    {

        // ok button
        layout->addWidget( okButton_ = new QToolButton( this ), 0 );
        okButton_->setIcon( IconEngine::get( IconNames::Find ) );
        static_cast<QToolButton*>( okButton_ )->setAutoRaise( true );

        // close button
        layout->addWidget( closeButton_ = new QToolButton( this ), 0 );
        closeButton_->setIcon( IconEngine::get( IconNames::DialogClose ) );
        static_cast<QToolButton*>( closeButton_ )->setAutoRaise( true );

        connect( closeButton_, &QAbstractButton::clicked, this, &QWidget::hide );

    } else {

        // separator
        auto frame( new QFrame( this ) );
        frame->setFrameStyle( QFrame::HLine | QFrame::Sunken );
        layout->addWidget( frame, 0 );

        // insert hbox layout for buttons
        QHBoxLayout* hLayout = new QHBoxLayout;
        hLayout->setSpacing(5);
        QtUtil::setMargin(hLayout, 0);
        layout->addLayout( hLayout, 0 );

        hLayout->addStretch( 1 );

        // ok button
        hLayout->addWidget( okButton_ = new QPushButton( this ) );
        okButton_->setText( tr( "Ok" ) );
        okButton_->setIcon( IconEngine::get( IconNames::Find ) );
        static_cast<QPushButton*>(okButton_)->setDefault( true );

        // close button
        hLayout->addWidget( closeButton_ = new QPushButton( IconEngine::get( IconNames::DialogCancel ), tr( "Cancel" ), this ) );
        closeButton_->setShortcut( Qt::Key_Escape );

    }

    // connections
    connect( okButton_, &QAbstractButton::clicked, this, &SelectLineWidget::_selectLine );

}

//________________________________________________________________________
void SelectLineWidget::matchFound()
{ editor_->setPalette( palette() ); }

//________________________________________________________________________
void SelectLineWidget::noMatchFound()
{
    if( !editor_->text().isEmpty() )
    { editor_->setPalette( notFoundPalette_ ); }
}

//________________________________________________________________________
void SelectLineWidget::changeEvent( QEvent* event )
{
    switch( event->type() )
    {
        case QEvent::PaletteChange:
        _updateNotFoundPalette();
        break;

        default: break;
    }

    QWidget::changeEvent( event );
}

//_______________________________________________________
void SelectLineWidget::_selectLine()
{ emit lineSelected( editor_->text().toInt()-1 ); }

//________________________________________________________________________
void SelectLineWidget::_updateNotFoundPalette()
{
    notFoundPalette_ = palette();
    notFoundPalette_.setColor( QPalette::Base,
        Base::Color( palette().color( QPalette::Base ) ).merge(
        Qt::red, 0.95 ) );
}
