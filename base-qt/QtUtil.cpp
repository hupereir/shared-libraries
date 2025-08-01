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
#include "Color.h"
#include "Debug.h"
#include "Util.h"
#include "XcbUtil.h"
#include "WaylandUtil.h"
#include "WinUtil.h"

#include <QApplication>
#include <QCursor>
#include <QFontMetrics>
#include <QLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QStyleOption>
#include <QWindow>

namespace
{
    namespace PropertyNames
    {
        static constexpr char bordersSides[] = "_breeze_borders_sides";
    }
}

//____________________________________________________________
void bindToGeometry( QPoint& position, QSize size, QRect geometry )
{
    if( position.x() + size.width() > geometry.right()+1 ) position.setX( geometry.right() + 1 - size.width() );
    if( position.y() + size.height() > geometry.bottom()+1 ) position.setY( geometry.bottom() + 1 - size.height() );

    if( position.x() < 0 ) position.setX(0);
    if( position.y() < 0 ) position.setY(0);
}

//____________________________________________________________
void QtUtil::setWidgetSides( QWidget* w, Qt::Edges edges )
{
    if(w)
    { w->setProperty( PropertyNames::bordersSides, Qt::Edges::Int(edges) ); }
}

//____________________________________________________________
bool QtUtil::hasWidgetSides( QWidget* w )
{
    if( !w ) { return false; }
    const auto borders = w->property(PropertyNames::bordersSides);
    return borders.isValid() && borders.canConvert<Qt::Edges>();
}

//____________________________________________________________
Qt::Edges QtUtil::widgetSides( QWidget* w )
{
    if( !w ) { return {}; }

    const auto borders = w->property(PropertyNames::bordersSides);
    if (borders.isValid() && borders.canConvert<Qt::Edges>())
    {
        return borders.value<Qt::Edges>();
    } else {
        return {};
    }
}


//____________________________________________________________
void QtUtil::initializeResources()
{
    Q_INIT_RESOURCE( basePixmaps );
}

//____________________________________________________________
QRect QtUtil::desktopGeometry( QWidget* widget )
{

    // get screen for widget
    if( widget && widget->window()->windowHandle() && widget->window()->windowHandle()->screen() )
    { return widget->window()->windowHandle()->screen()->availableGeometry(); }

    // get primary screen from qApp
    if( qApp && qApp->primaryScreen() ) return qApp->primaryScreen()->availableGeometry();

    // return invalid geometry
    return QRect();
}

//____________________________________________________________
void QtUtil::setWidgetGeometry( QWidget* widget, const QRect& geometry )
{

    Debug::Throw( QStringLiteral("QtUtil::setWidgetGeometry.\n") );
    if( !widget ) return;

    widget->resize( geometry.size() );
    moveWidget( widget, geometry.topLeft() );
}

//____________________________________________________________
void QtUtil::moveWidget( QWidget* widget, QPoint position )
{
    Debug::Throw( QStringLiteral("QtUtil::moveWidget.\n") );
    if( !widget ) return;

    const auto geometry( desktopGeometry( widget ) );
    bindToGeometry( position, widget->size(), geometry );

    #if WITH_KWAYLAND
    if( WaylandUtil::isWayland() )
    {
        WaylandUtil::moveWidget(widget, position);
        return;
    }
    #endif

    // default implementation
    widget->move( position );
}

//____________________________________________________________
void QtUtil::toggleHideWidgetFromTaskbar( QWidget* w, bool value )
{
    // check that widget is top level
    if( w->parentWidget() ) return;

    #if WITH_XCB
    if( XcbUtil::isX11() )
    {
        XcbUtil::toggleHideWidgetFromTaskbar( w, value );
        return;
    }
    #endif

    #if WITH_KWAYLAND
    if( WaylandUtil::isWayland() )
    {
        WaylandUtil::toggleHideWidgetFromTaskbar( w, value );
        return;
    }
    #endif

    #if defined(Q_OS_WIN)
    WinUtil( w ).toggleHideFromTaskBar( value );
    #endif

}

//____________________________________________________________
void QtUtil::toggleShowWidgetOnAllDesktops( QWidget* w, bool value )
{
    // check that widget is top level
    if( w->parentWidget() ) return;

    #if WITH_XCB
    if( XcbUtil::isX11() )
    {
        XcbUtil::toggleShowWidgetOnAllDesktops( w, value );
        return;
    }
    #endif

    #if WITH_KWAYLAND
    if( WaylandUtil::isWayland() )
    {
        WaylandUtil::toggleShowWidgetOnAllDesktops( w, value );
        return;
    }
    #endif

}

//____________________________________________________________
void QtUtil::toggleWidgetStaysOnTop( QWidget* w, bool value )
{
    // check that widget is top level
    if( w->parentWidget() ) return;

    #if WITH_XCB
    if( XcbUtil::isX11() )
    {
        XcbUtil::toggleWidgetStaysOnTop( w, value );
        return;
    }
    #endif

    #if WITH_KWAYLAND
    if( WaylandUtil::isWayland() )
    {
        WaylandUtil::toggleWidgetStaysOnTop( w, value );
        return;
    }
    #endif

    // try Qt API
    const bool visible( !w->isHidden() );
    if( visible )
    {
        w->hide();
        qApp->processEvents();
    }

    // Qt implementation
    if( value ) w->setWindowFlags( w->windowFlags() | Qt::WindowStaysOnTopHint );
    else w->setWindowFlags( w->windowFlags() & ~Qt::WindowStaysOnTopHint );

    if( visible ) w->show();
}

//____________________________________________________________
QPoint QtUtil::centerOnPointer( QSize size )
{
    Debug::Throw( QStringLiteral("QtUtil::centerOnPointer.\n") );

    // get cursor position
    QPoint position( QCursor::pos() - QPoint( size.width()/2, size.height()/2) );

    // retrieve desktop
    const auto geometry( desktopGeometry() );
    bindToGeometry( position, size, geometry );
    return position;
}


//____________________________________________________________
QPoint QtUtil::centerOnWidget( QSize size, QWidget* widget )
{
    Debug::Throw( QStringLiteral("QtUtil::centerOnWidget.\n") );
    if( !widget ) return centerOnDesktop( size );

    Debug::Throw() << "QtUtil::centerOnWidget - size: (" << size.width() << "," << size.height() << ")" << Qt::endl;

    // get parent position and size
    QPoint position( widget->pos() );
    if( widget != widget->window() ) position = widget->mapToGlobal( position );

    const auto parentSize( widget->frameSize() );
    position.setX( position.x() + ( parentSize.width() - size.width() )/2 );
    position.setY( position.y() + ( parentSize.height() - size.height() )/2 );

    // retrieve desktop
    const auto geometry( desktopGeometry( widget ) );
    bindToGeometry( position, size, geometry );
    return position;
}

//____________________________________________________________
QPoint QtUtil::centerOnDesktop( QSize size )
{
    Debug::Throw( QStringLiteral("QtUtil::centerOnDesktop.\n") );

    // retrieve desktop
    const auto geometry( desktopGeometry() );

    QPoint position( geometry.center() - QPoint( size.width()/2, size.height()/2 ) );
    bindToGeometry( position, size, geometry );
    return position;
}

//____________________________________________________________
QWidget* QtUtil::centerOnPointer( QWidget* widget )
{
    Debug::Throw( QStringLiteral("QtUtil::centerOnPointer.\n") );

    // move widget
    widget->move( centerOnPointer( widget->frameSize() ) );
    return widget;
}

//____________________________________________________________
QWidget* QtUtil::centerOnParent( QWidget* widget )
{ return centerOnWidget( widget, widget->parentWidget() ); }


//____________________________________________________________
QWidget* QtUtil::centerOnWidget( QWidget* widget, QWidget* parent )
{
    Debug::Throw( QStringLiteral("QtUtil::centerOnParent.\n") );

    // get parent widget
    if( !( parent && parent->window() ) ) centerOnDesktop( widget );
    else widget->move( centerOnWidget( widget->frameSize(), parent->window() ) );
    return widget;
}


//____________________________________________________________
QWidget* QtUtil::centerOnDesktop( QWidget* widget )
{
    Debug::Throw( QStringLiteral("QtUtil::centerOnDesktop.\n") );
    widget->move( centerOnDesktop( widget->frameSize() ) );
    return widget;
}

//____________________________________________________________
QWidget* QtUtil::uniconify( QWidget *widget )
{

    Debug::Throw( QStringLiteral("QtUtil::uniconify.\n") );

    if( !widget->isTopLevel() ) return widget;
    widget = widget->window();

    if( widget->isMinimized() ) widget->showNormal();
    else if( widget->isHidden() ) widget->show();

    widget->activateWindow();
    widget->raise();

    return widget;

}

//____________________________________________________________
int QtUtil::titleFontPointSize( const QFont& font )
{ return qRound(font.pointSize() * 1.4); }

//____________________________________________________________
QFont QtUtil::titleFont( QFont font )
{
    font.setPointSize( titleFontPointSize( font ) );
    font.setWeight( QFont::Medium );
    return font;
}

//____________________________________________________________
QColor QtUtil::lightTextColor( const QColor& color )
{  return Base::Color( color ).addAlpha( 0.7 ); }

//____________________________________________________________
QAction* QtUtil::addMenuSection( QMenu* menu, const QIcon& icon, const QString& text )
{
    menu->setSeparatorsCollapsible( false );
    auto action = menu->addSection( icon, text );

    // calculate minimum size, needed to properly account for header
    int width = 0;
    if( !icon.isNull() )
    {
        QStyleOption opt;
        opt.initFrom( menu );
        const int iconSize = menu->style()->pixelMetric(QStyle::PM_SmallIconSize, &opt, menu);
        width += iconSize;
    }

    if( !text.isNull() )
    {
        QFont font( menu->font() );
        font.setWeight( QFont::Bold );
        width += QFontMetrics( font ).horizontalAdvance( text ) + 24;
    }

    if( width > 0 )
    {
        QSize minSizeHint( menu->minimumSizeHint() );
        menu->setMinimumSize( QSize( qMax( width, minSizeHint.width() ), qMax( 0, minSizeHint.height() ) ) );
    }

    return action;
}
