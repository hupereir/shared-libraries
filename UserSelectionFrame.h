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

class UserSelectionFrame: public QWidget, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    UserSelectionFrame( QWidget* parent );

    //! set user
    void setUser( const QString& user );

    //! users
    QStringList users( void ) const;

    //! selected user
    QString user( void ) const;

    //! editor
    CustomComboBox& comboBox( void ) const
    { return *comboBox_; }

    Q_SIGNALS:

    //! emitted when user is changed
    void userChanged( QString );

    public Q_SLOTS:

    //! update user list
    void updateUsers( QStringList );

    protected:

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    private Q_SLOTS:

    //! selected user changed
    void _userChanged( void );

    //! selected user changed
    void _delayedUserChanged( void )
    { timer_.start( delay_, this ); }

    private:

    //! delay for userChanged signal emission
    unsigned int delay_;

    //! user changed timer
    QBasicTimer timer_;

    //! user line_edit
    CustomComboBox* comboBox_;

    //! current user
    /*! it is used to avoid emmitting signal when user was changed but fall back to the current one*/
    QString user_;

};

#endif
