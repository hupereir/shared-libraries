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

#include "QtUtil.h"

#include "Debug.h"
#include "Util.h"

#include <QApplication>
#include <QCursor>
#include <QDesktopWidget>
#include <QFontMetrics>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QStyleOption>

//____________________________________________________________
void QtUtil::moveWidget( QWidget* widget, QPoint position )
{

    Debug::Throw( "QtUtil::moveWidget.\n" );
    if( !widget ) return;
    QDesktopWidget* desktop( qApp->desktop() );
    QRect geometry( desktop->screenGeometry( desktop->screenNumber( widget ) ) );
    if( position.y() + widget->height() > geometry.bottom()+1 ) position.setY( geometry.bottom() + 1 - widget->height() );

    widget->move( position );
}

//____________________________________________________________
QPoint QtUtil::centerOnPointer( const QSize& size )
{
    Debug::Throw( "QtUtil::centerOnPointer.\n" );

    // get cursor position
    QPoint point( QCursor::pos() );

    point.setX( point.x() - size.width()/2 );
    point.setY( point.y() - size.height()/2 );

    // retrieve desktop
    QDesktopWidget *desktop( qApp->desktop() );

    // check point against desktop size
    if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
    if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );

    // check point against (0,0)
    if( point.x() < 0 ) point.setX( 0 );
    if( point.y() < 0 ) point.setY( 0 );
    return point;
}


//____________________________________________________________
QPoint QtUtil::centerOnWidget( const QSize& size, QWidget* widget )
{
    Debug::Throw( "QtUtil::centerOnWidget.\n" );
    if( !widget ) return centerOnDesktop( size );

    Debug::Throw() << "QtUtil::centerOnWidget - size: (" << size.width() << "," << size.height() << ")" << endl;

    // get parent position and size
    QPoint point( widget->pos() );
    if( widget != widget->window() ) point = widget->mapToGlobal( point );

    QSize parent_size( widget->frameSize() );

    Debug::Throw() << "QtUtil::centerOnWidget - parent size: (" << parent_size.width() << "," << parent_size.height() << ")" << endl;
    Debug::Throw() << "QtUtil::centerOnWidget - parent position: (" << point.x() << "," << point.y() << ")" << endl;

    point.setX( point.x() + ( parent_size.width() - size.width() )/2 );
    point.setY( point.y() + ( parent_size.height() - size.height() )/2 );

    // retrieve desktop
    QDesktopWidget *desktop( qApp->desktop() );

    // check point against desktop size
    if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
    if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );

    // check point against (0,0)
    if( point.x() < 0 ) point.setX( 0 );
    if( point.y() < 0 ) point.setY( 0 );
    return point;

}

//____________________________________________________________
QPoint QtUtil::centerOnDesktop( const QSize& size )
{
    Debug::Throw( "QtUtil::centerOnDesktop.\n" );

    // retrieve desktop
    QDesktopWidget *desktop( qApp->desktop() );
    QSize desktop_size( desktop->frameSize() );

    // get parent position and size
    QPoint point( desktop->pos() );

    point.setX( point.x() + ( desktop_size.width() - size.width() )/2 );
    point.setY( point.y() + ( desktop_size.height() - size.height() )/2 );

    // check point against desktop size
    if( point.x() + size.width()> desktop->width() ) point.setX( desktop->width() - size.width() );
    if( point.y() + size.height()> desktop->height() ) point.setY( desktop->height() - size.height() );

    // check point against (0,0)
    if( point.x() < 0 ) point.setX( 0 );
    if( point.y() < 0 ) point.setY( 0 );
    return point;
}

//____________________________________________________________
QWidget* QtUtil::centerOnPointer( QWidget* widget )
{
    Debug::Throw( "QtUtil::centerOnPointer.\n" );
    Q_CHECK_PTR( widget );

    // move widget
    widget->move( centerOnPointer( widget->frameSize() ) );
    qApp->processEvents();
    return widget;
}

//____________________________________________________________
QWidget* QtUtil::centerOnParent( QWidget* widget )
{
    Q_CHECK_PTR( widget );
    return centerOnWidget( widget, widget->parentWidget() );
}


//____________________________________________________________
QWidget* QtUtil::centerOnWidget( QWidget* widget, QWidget* parent )
{
    Debug::Throw( "QtUtil::centerOnParent.\n" );
    Q_CHECK_PTR( widget );

    // get parent widget
    if( !( parent && parent->window() ) ) centerOnDesktop( widget );
    else widget->move( centerOnWidget( widget->frameSize(), parent->window() ) );
    return widget;
}


//____________________________________________________________
QWidget* QtUtil::centerOnDesktop( QWidget* widget )
{
    Debug::Throw( "QtUtil::centerOnDesktop.\n" );
    Q_CHECK_PTR( widget );
    widget->move( centerOnDesktop( widget->frameSize() ) );
    return widget;
}

//____________________________________________________________
QWidget* QtUtil::uniconify( QWidget *widget )
{

    Debug::Throw( "QtUtil::uniconify.\n" );

    if( !widget->isTopLevel() ) return widget;
    widget = widget->window();

    if( widget->isMinimized() ) widget->showNormal();
    else if( widget->isHidden() ) widget->show();

    widget->activateWindow();
    widget->raise();

    return widget;

}

//____________________________________________________________
QFont QtUtil::titleFont( QFont font )
{
    font.setPointSize( qRound(font.pointSize() * 1.4) );
    return font;
}

//____________________________________________________________
QAction* QtUtil::addMenuSection( QMenu* menu, const QIcon& icon, const QString& text )
{
    menu->setSeparatorsCollapsible( false );

    #if QT_VERSION >= 0x050000
    QAction* action = menu->addSection( icon, text );
    #else
    QAction* action = menu->addSeparator();
    if( !text.isEmpty() ) action->setText( text );
    if( !icon.isNull() ) action->setIcon( icon );
    #endif

    // calculate minimum size, needed to properly account for header
    int width = 0;
    if( !icon.isNull() )
    {
        QStyleOption opt;
        opt.init( menu );
        const int iconSize = menu->style()->pixelMetric(QStyle::PM_SmallIconSize, &opt, menu);
        width += iconSize;
    }

    if( !text.isNull() )
    {
        QFont font( menu->font() );
        font.setWeight( QFont::Bold );
        width += QFontMetrics( font ).width( text ) + 24;
    }

    if( width > 0 )
    {
        QSize minSizeHint( menu->minimumSizeHint() );
        menu->setMinimumSize( QSize( qMax( width, minSizeHint.width() ), qMax( 0, minSizeHint.height() ) ) );
    }

    return action;
}
