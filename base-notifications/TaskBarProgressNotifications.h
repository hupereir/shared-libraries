#ifndef TaskBarProgressNotifications_h
#define TaskBarProgressNotifications_h

/*
 * based on UnityLauncher class
 * Copyright 2016 Kai Uwe Broulik <kde@privat.broulik.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Counter.h"

#include <QObject>

class TaskBarProgressNotifications : public QObject, private Base::Counter<TaskBarProgressNotifications>
{
    Q_OBJECT

    public:

    //* contructor
    explicit TaskBarProgressNotifications(QObject* parent = nullptr):
        QObject( parent ),
        Counter( "TaskBarProgressNotifications" )
        {}

    //*@name accessors
    //@{

    //* dbus path
    QString path( void ) const
    { return path_; }

    //* launcher id
    QString launcherId( void ) const
    { return launcherId_; }

    //* true if progress is visible
    bool progressVisible( void ) const
    { return progressVisible_; }

    //* max progress
    int maximum( void ) const
    { return maximum_; }

    //* current progress
    int value( void ) const
    { return value_; }

    //@}

    //*@name modifiers
    //@{

    void setPath( const QString& value )
    { path_ = value; }

    void setLauncherId( const QString& value )
    { launcherId_ = value; }

    void setProgressVisible( bool );

    //@}

    public Q_SLOTS:

    //* maximum
    void setMaximum( int );

    //* value
    void setValue( int );

    private:

    //* update progress, by passing dbus signal
    void _update(const QVariantMap &properties);

    //* dbus signal path
    QString path_;

    //* launcher id
    QString launcherId_;

    //* true if progress is visible
    bool progressVisible_ = false;

    //* maximum progress
    int maximum_ = 0;

    //* current progress
    int value_ = 0;

};


#endif
