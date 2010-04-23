#ifndef UserSelectionFrame_h
#define UserSelectionFrame_h

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

/*!
\file UserSelectionFrame.h
\brief current user
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <set>

#include <QBasicTimer>

#include "Counter.h"
#include "CustomComboBox.h"

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
    std::set<QString> users( void ) const;

    //! selected user
    QString user( void ) const;

    //! editor
    CustomComboBox& editor( void ) const
    {
        assert( editor_ );
        return *editor_;
    }

    signals:

    //! emitted when user is changed
    void userChanged( QString );

    public slots:

    //! update user list
    void updateUsers( std::set<QString> );

    protected:

    virtual void timerEvent( QTimerEvent* e )
    {

        if( e->timerId() == timer_.timerId() )
        {
            timer_.stop();
            _userChanged();
        } else return QWidget::timerEvent( e );

    }

    private slots:

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
    CustomComboBox* editor_;

    //! current user
    /*! it is used to avoid emmitting signal when user was changed but fall back to the current one*/
    QString user_;

};

#endif
