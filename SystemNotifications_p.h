#ifndef SystemNotifications_p_h
#define SystemNotifications_p_h

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
#include "ImageData.h"

#include <QIcon>
#include <QBasicTimer>
#include <QObject>
#include <QStringList>
#include <QTimerEvent>

class SystemNotificationsP: public QObject, public Counter
{

    public:

    //* constructor
    SystemNotificationsP( QObject* = nullptr );

    //* destructor
    virtual ~SystemNotificationsP( void ) = default;

    //* @name modifiers
    //@{

    //* applicatino name
    void setApplicationName( const QString& );

    //* set icon
    void setApplicationIcon( const QIcon& );

    //* process message
    virtual void send( const QString&, const QString& );

    //@}

    private:

    //* application name
    QString applicationName_;

    //* image data
    Notifications::ImageData imageData_;

    //* image data type id
    int typeId_ = 0;

};

#endif
