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

#include "DockWidget.h"
#include "BaseMainWindow.h"
#include "Singleton.h"
#include "XmlOptions.h"

#include <QtGui/QStyle>

//! titlebar
class TitleBar: public QWidget
{

    public:

    //! constructor
    TitleBar(QWidget* parent = 0 ):
        QWidget(parent)
    {}

    //! destructor
    virtual ~TitleBar()
    {}

    //! size hint
    virtual QSize minimumSizeHint() const
    {
        const int border = style()->pixelMetric( QStyle::PM_DockWidgetTitleBarButtonMargin );
        return QSize(border, border);
    }

    //! size hint
    virtual QSize sizeHint() const
    { return minimumSizeHint(); }

};

//_________________________________________________________
DockWidget::DockWidget(const QString& title, QWidget* parent, Qt::WindowFlags flags) :
    QDockWidget(title, parent, flags),
    locked_(false),
    titleBar_(0)
{
    // configuration
    connect( Singleton::get().application(), SIGNAL( configurationChanged() ), SLOT( _updateConfiguration() ) );
    _updateConfiguration();
}

//_________________________________________________________
void DockWidget::setLocked(bool locked)
{
    if( locked == locked_ ) return;

    locked_ = locked;
    if( locked )
    {
        if( !titleBar_ ) titleBar_ = new TitleBar( this );
        setTitleBarWidget( titleBar_ );
        setFeatures(QDockWidget::NoDockWidgetFeatures);

    } else {

        setTitleBarWidget(0);
        setFeatures(
            QDockWidget::DockWidgetMovable |
            QDockWidget::DockWidgetFloatable |
            QDockWidget::DockWidgetClosable);
    }

}


//_______________________________________________________________
void DockWidget::_updateConfiguration( void )
{
    Debug::Throw( "DockWidget::_updateConfiguration.\n" );

    // lock
    BaseMainWindow* mainwindow( qobject_cast<BaseMainWindow*>( window() ) );
    if( mainwindow && mainwindow->hasOptionName() && XmlOptions::get().contains( mainwindow->lockPanelsOptionName() ) )
    { setLocked( XmlOptions::get().get<bool>( mainwindow->lockPanelsOptionName() ) ); }

}
