#ifndef BaseMainWindow_h
#define BaseMainWindow_h

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

#include "CustomToolButton.h"
#include "Debug.h"
#include "IconSize.h"
#include "WidgetMonitor.h"
#include "XmlOptions.h"

#include <QAction>
#include <QActionGroup>
#include <QMainWindow>
#include <QMenu>
#include <QStatusBar>
#include <QStyle>

class ToolBarMenu;

/**
\class BaseMainWindow
\brief customized QDialog
*/
class BaseMainWindow: public QMainWindow
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseMainWindow( QWidget*, Qt::WindowFlags = 0);

    //* restore window size
    QSize minimumSizeHint( void ) const override;

    //* restore window size
    QSize sizeHint( void ) const override;

    //* center widget on argument widget
    void centerOnDesktop( void );

    //* center on parent widget
    void centerOnParent( void )
    { centerOnWidget( parentWidget() ); }

    //* center widget on argument widget
    void centerOnWidget( QWidget* );

    //* window title
    virtual void setWindowTitle( const QString& );

    //* set menu bar (overloaded)
    void setMenuBar( QMenuBar* );

    //* set menu bar (overloaded)
    void setStatusBar( QStatusBar* );

    //* lock toolbars
    QAction& lockToolBarsAction( void ) const
    { return *lockToolBarsAction_; }

    //* lock panels
    QAction& lockPanelsAction( void ) const
    { return *lockPanelsAction_; }

    //* show menubar
    QAction& showMenuBarAction( void ) const
    { return *showMenuBarAction_; }

    //* show status bar
    QAction& showStatusBarAction( void ) const
    { return *showStatusBarAction_; }

    //* create context menu (overloaded)
    QMenu* createPopupMenu( void ) override;

    //* toolbar menu
    ToolBarMenu& toolBarMenu( QWidget* = nullptr );

    //* option name
    void setOptionName( const QString& name );

    //* menu option name
    bool hasOptionName( void ) const
    { return !lockToolBarsOptionName().isEmpty(); }

    //* lock toolbar option name
    const QString& lockToolBarsOptionName( void ) const
    { return lockToolBarsOptionName_; }

    //* lock panels option name
    const QString& lockPanelsOptionName( void ) const
    { return lockPanelsOptionName_; }

    //* menu option name
    const QString& showMenuBarOptionName( void ) const
    { return showMenuBarOptionName_; }

    //* status bar option name
    const QString& showStatusBarOptionName( void ) const
    { return showStatusBarOptionName_; }

    Q_SIGNALS:

    //* toolbar configuration changed
    void toolbarConfigurationChanged( void );

    public Q_SLOTS:

    //* uniconify
    void uniconify( void );

    protected:

    //* generic event
    bool event( QEvent* ) override;

    //* true if main window has menu
    bool _hasMenuBar( void ) const;

    //* true if main window has menu
    bool _hasStatusBar( void ) const;

    //* true if main window has toolbars
    bool _hasToolBars( void ) const;

    //* returns true if lockable toolbars are found
    bool _hasLockableToolBars( void ) const;

    //* true if main window has dock widgets
    bool _hasPanels( void ) const;

    //* action list
    using ActionList = QVector<QAction*>;

    //* toolbar actions list
    ActionList _toolBarsActions( QMenu* );

    //* maximize state prior to minimization
    bool _wasMaximized( void ) const
    { return wasMaximized_; }

    //* maximize state prior to minimization
    void _setWasMaximized( bool value )
    { wasMaximized_ = value; }

    private Q_SLOTS:

    //* update configuration
    void _updateConfiguration( void );

    //* toolbar text position
    void _updateToolButtonStyle( int );

    //* toolbar text position
    void _updateToolButtonIconSize( IconSize::Size );

    //* lock toolbars
    void _lockToolBars( bool );

    //* lock panels
    void _lockPanels( bool );

    //* toggle menu
    void _toggleMenuBar( bool );

    //* toggle status bar
    void _toggleStatusBar( bool );

    private:

    //* option name
    QString lockToolBarsOptionName_;

    //* option name
    QString lockPanelsOptionName_;

    //* option name
    QString showMenuBarOptionName_;

    //* option name
    QString showStatusBarOptionName_;

    //* size watcher
    WidgetMonitor monitor_;

    //* lock toolbars
    QAction* lockToolBarsAction_ = nullptr;

    //* lock docks
    QAction* lockPanelsAction_ = nullptr;

    //* toggle menu
    QAction* showMenuBarAction_ = nullptr;

    //* toggle statusBar
    QAction* showStatusBarAction_ = nullptr;

    //* window state prior to minimization
    bool wasMaximized_ = false;

};

#endif
