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

#include "Util.h"
#include "UserSelectionFrame.h"

#include <QLayout>
#include <QLabel>

//____________________________________________________________
UserSelectionFrame::UserSelectionFrame( QWidget* parent ):
    QWidget( parent ),
    Counter( QStringLiteral("UserSelectionFrame") )
{

    Debug::Throw( QStringLiteral("UserSelectionFrame::UserSelectionFrame\n") );
    setLayout( new QHBoxLayout );
    layout()->setMargin(0);
    layout()->setSpacing(5);
    layout()->addWidget( comboBox_ = new ComboBox( this ) );

    comboBox_->setMinimumSize( QSize( 100, 0 ) );
    comboBox_->setToolTip( tr( "User selection" ) );

    // send appropriate signal directly
    connect( comboBox_, QOverload<const QString&>::of( &QComboBox::activated ), this, [this](const QString&){ _userChanged(); } );

    // one must add a timer here so that the signal gets
    // emitted only after some delay, to avoid to many signals are sent when typing
    connect( comboBox_, &QComboBox::editTextChanged,  this, &UserSelectionFrame::_delayedUserChanged );

}

//____________________________________________________________
QStringList UserSelectionFrame::users() const
{

    QStringList out;
    for( int i=0; i< comboBox_->QComboBox::count(); i++ )
    {
        const auto& user( comboBox_->itemText( i ) );
        if( !user.isNull() ) out.append( user );
    }

    return out;

}

//____________________________________________________________
void UserSelectionFrame::setUser( const QString& user )
{
    Debug::Throw() << "UserSelectionFrame::set - user: " << user << endl;
    if( user.isNull() || user.isEmpty() ) return;

    for( int i=0; i< comboBox_->QComboBox::count(); i++ )
    {
        if( user == comboBox_->itemText( i ) )
        {
            comboBox_->setCurrentIndex(i);
            return;
        }
    }

    comboBox_->addItem( user );
    comboBox_->setEditText( user );

    return;
}

//________________________________________________________________
void UserSelectionFrame::updateUsers( QStringList users )
{
    Debug::Throw( QStringLiteral("UserSelectionFrame::updateUsers.\n") );

    // make sure default user and current user are in the list
    const QString currentUser( user() );
    if( !( currentUser.isEmpty() || users.contains( currentUser ) ) )
    { users.prepend( currentUser ); }

    // take all items except current
    for( int index = 0; index < comboBox_->QComboBox::count(); )
    {
        if( !currentUser.isEmpty() && comboBox_->itemText( index ) == currentUser ) index++;
        else comboBox_->removeItem( index );
    }

    // insert items, respecting order
    for( int index = 0; index < users.size(); ++index )
    { if( users[index] != currentUser ) comboBox_->insertItem( index, users[index] ); }

}

//____________________________________________________________
QString UserSelectionFrame::user() const
{
    Debug::Throw( QStringLiteral("UserSelectionFrame::user.\n") );
    QString user(  comboBox_->currentText() );
    if( user.isNull() || user.isEmpty() ) return QLatin1String("");
    return user.trimmed();
}

//____________________________________________________________
void UserSelectionFrame::_userChanged()
{
    QString new_user( user() );
    if( new_user == user_ ) return;
    user_ = new_user;
    emit userChanged( user_ );
}

//____________________________________________________________
void UserSelectionFrame::timerEvent( QTimerEvent* event )
{

    if( event->timerId() == timer_.timerId() )
    {
        timer_.stop();
        _userChanged();
    } else QWidget::timerEvent( event );

}
