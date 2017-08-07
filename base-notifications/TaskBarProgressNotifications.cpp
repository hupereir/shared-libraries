/*
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

#include "TaskBarProgressNotifications.h"
#include "CppUtil.h"
#include "Debug.h"

#ifndef QT_NO_DBUS
#include <QDBusConnection>
#include <QDBusMessage>
#endif

#include <QVariantMap>

//____________________________________________________________
void TaskBarProgressNotifications::setProgressVisible(bool progressVisible)
{
    if( progressVisible_ != progressVisible )
    {
        progressVisible_ = progressVisible;
        if( progressVisible_ && first_ )
        {
            _update({ {QLatin1String("progress"), 0} });
            first_ = false;
        }
        _update({ {QLatin1String("progress-visible"), progressVisible} });
    }

}

//____________________________________________________________
void TaskBarProgressNotifications::setMaximum(int maximum)
{

    if( maximum_ != maximum )
    {
        maximum_ = maximum;
        if( maximum_ > 0 )
        {

            if( progressVisible_ ) setValue( 0 );

        } else {

            setProgressVisible( false );
            setValue(0);

        }
    }

}

//____________________________________________________________
void TaskBarProgressNotifications::setValue(int value)
{
    if( (value_ != value || first_ ) && maximum_ > 0 )
    {
        first_ = false;
        value_ = value;
        _update({ {QLatin1String("progress"), value/qreal(maximum_)} });
    }
}

//____________________________________________________________
void TaskBarProgressNotifications::_update(std::initializer_list<typename std::pair<QString, QVariant>>&& properties)
{
    if( launcherId_.isEmpty() || path_.isEmpty() ) return;

    #ifndef QT_NO_DBUS
    auto message = QDBusMessage::createSignal(
        path_,
        QLatin1String("com.canonical.Unity.LauncherEntry"),
        QLatin1String("Update"));

    message.setArguments({launcherId_, Base::makeT<QVariantMap>(std::move(properties))});
    QDBusConnection::sessionBus().send(message);
    #endif

}
