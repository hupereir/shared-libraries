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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "SystemNotifications_p.h"

#if WITH_LIBNOTIFY
#include <libnotify/notify.h>
#endif

#include "Debug.h"
#include "IconSize.h"

#include <QApplication>
#include <QIcon>
#include <QImage>

//_________________________________________
SystemNotificationsP::SystemNotificationsP( const QString& applicationName, const QIcon& icon ):
  icon_( 0x0 )
{
    #if WITH_LIBNOTIFY
    notify_init( applicationName.toLatin1());

    // try get pixmap from options and convert to icon
    if( !icon.isNull() )
    {

        QImage image( icon.pixmap( IconSize( IconSize::Maximum ) ).toImage() );
        image = image.convertToFormat( QImage::Format_ARGB32 );
        if( !image.isNull() )
        {
            GdkPixbuf* pixbuf = gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, image.width(), image.height() );
            uchar* destBits = gdk_pixbuf_get_pixels( pixbuf );
            const int rowStride = gdk_pixbuf_get_rowstride( pixbuf );
            const int nChannels = gdk_pixbuf_get_n_channels( pixbuf );
            for( int column = 0; column < image.width(); ++column )
            {
                for( int row = 0; row < image.height(); ++row )
                {
                    QRgb pixel( image.pixel( column, row ) );
                    int index = row * rowStride + column * nChannels;
                    destBits[index+0] = qRed( pixel );
                    destBits[index+1] = qGreen( pixel );
                    destBits[index+2] = qBlue( pixel );
                    destBits[index+3] = qAlpha( pixel );
                }
            }

            icon_ = pixbuf;

        }

    } else { Debug::Throw(0) << "SystemNotificationsP::SystemNotificationsP - invalid icon" << endl; }

    #endif

}

//_________________________________________
SystemNotificationsP::~SystemNotificationsP( void )
{
    #if WITH_LIBNOTIFY
    if( icon_ ) g_object_unref( icon_ );
    #endif
}

//_________________________________________
void SystemNotificationsP::send( const QString& summary, const QString& message, const QString& icon ) const
{

    #if WITH_LIBNOTIFY
    NotifyNotification* notification( 0x0 );
    if( !icon.isEmpty() ) notification = notify_notification_new( summary.toLatin1(), message.toLatin1(), icon.toLatin1() );
    else if( icon_ )
    {

        notification = notify_notification_new( summary.toLatin1(), message.toLatin1(), 0x0 );
        if( icon_ ) notify_notification_set_image_from_pixbuf( notification, static_cast<GdkPixbuf*>( icon_ ) );

    } else notification = notify_notification_new( summary.toLatin1(), message.toLatin1(), "dialog-information" );

    notify_notification_show( notification, 0x0 );
    #endif
}
