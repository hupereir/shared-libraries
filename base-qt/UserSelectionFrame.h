#ifndef UserSelectionFrame_h
#define UserSelectionFrame_h

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

#include "Counter.h"
#include "CustomComboBox.h"

#include <QBasicTimer>
#include <QSet>

class UserSelectionFrame: public QWidget, private Base::Counter<UserSelectionFrame>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit UserSelectionFrame( QWidget* );

    //* set user
    void setUser( const QString& );

    //* users
    QStringList users() const;

    //* selected user
    QString user() const;

    //* editor
    CustomComboBox& comboBox() const
    { return *comboBox_; }

    //* update user list
    void updateUsers( QStringList );

    Q_SIGNALS:

    //* emitted when user is changed
    void userChanged( QString );

    protected:

    //* timer event
    void timerEvent( QTimerEvent* ) override;

    private:

    //* selected user changed
    void _userChanged();

    //* selected user changed
    void _delayedUserChanged()
    { timer_.start( delay_, this ); }

    //* delay for userChanged signal emission
    int delay_ = 500;

    //* user changed timer
    QBasicTimer timer_;

    //* user line_edit
    CustomComboBox* comboBox_ = nullptr;

    //* current user
    /*! it is used to avoid emiting signal when user was changed but fall back to the current one*/
    QString user_;

};

#endif
