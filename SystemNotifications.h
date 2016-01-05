#ifndef SystemNotifications_h
#define SystemNotifications_h

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
#include "Notification.h"

#include <QIcon>
#include <QBasicTimer>
#include <QObject>
#include <QStringList>
#include <QTimerEvent>

class SystemNotificationsP;

class SystemNotifications: public QObject, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    SystemNotifications( QObject* = nullptr, const QString& = QString(), const QIcon& = QIcon() );

    //* destructor
    virtual ~SystemNotifications( void );

    //* @name modifiers
    //@{

    //* applicatino name
    void setApplicationName( const QString& );

    //* set icon
    void setApplicationIcon( const QIcon& );

    //* process message
    virtual void processMessage( const QString& summary, const QString& body )
    { sendNotification( Notification( summary, body ) ); }

    //* process notification
    virtual void sendNotification( const Notification& );

    //@}

    //* @name accessors
    //@{

    //* true if available
    static bool isSupported( void );

    //@}

    Q_SIGNALS:

    //* action invoked
    void actionInvoked( quint32, QString );

    protected:

    //* event handler
    virtual void timerEvent( QTimerEvent* );

    //* process message queue
    void _sendPendingNotification( void );

    private:

    //* timer
    QBasicTimer timer_;

    //* running notification
    Notification notification_;

    //* private
    SystemNotificationsP* d_;

};

#endif
