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

#include "base_qt_export.h"
#include "ToolButton.h"
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
class ApplicationMenu;

/**
\class BaseMainWindow
\brief customized QDialog
*/
class BASE_QT_EXPORT BaseMainWindow: public QMainWindow
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseMainWindow( QWidget*, Qt::WindowFlags = 0);

    //*@name accessors
    //@{

    //* restore window size
    QSize minimumSizeHint() const override;

    //* restore window size
    QSize sizeHint() const override;

    //* lock toolbars
    QAction& lockToolBarsAction() const
    { return *lockToolBarsAction_; }

    //* lock panels
    QAction& lockPanelsAction() const
    { return *lockPanelsAction_; }

    //* show menubar
    QAction& showMenuBarAction() const
    { return *showMenuBarAction_; }

    //* show status bar
    QAction& showStatusBarAction() const
    { return *showStatusBarAction_; }

    //* menu option name
    bool hasOptionName() const
    { return !lockToolBarsOptionName().isEmpty(); }

    //* lock toolbar option name
    const QString& lockToolBarsOptionName() const
    { return lockToolBarsOptionName_; }

    //* lock panels option name
    const QString& lockPanelsOptionName() const
    { return lockPanelsOptionName_; }

    //* menu option name
    const QString& showMenuBarOptionName() const
    { return showMenuBarOptionName_; }

    //* status bar option name
    const QString& showStatusBarOptionName() const
    { return showStatusBarOptionName_; }

    //@}

    //*@name modifiers
    //@{

    //* center widget on argument widget
    void centerOnDesktop();

    //* center on parent widget
    void centerOnParent()
    { centerOnWidget( parentWidget() ); }

    //* center widget on argument widget
    void centerOnWidget( QWidget* );

    //* window title
    virtual void setWindowTitle( const QString& );

    //* set menu bar (overloaded)
    void setMenuBar( QMenuBar* );

    //* set status bar (overloaded)
    void setStatusBar( QStatusBar* );

    //* create context menu (overloaded)
    QMenu* createPopupMenu() override;

    //* setup application menu button
    void setupApplicationMenu( QToolButton* );

    //* setup application menu button
    void addApplicationMenu( QToolBar* );

    //* update application menu
    void updateApplicationMenu();

    //* toolbar menu
    ToolBarMenu* toolBarMenu( QWidget* = nullptr );

    //* option name
    void setOptionName( const QString& name );

    //* uniconify
    void uniconify();

    //@}

    Q_SIGNALS:

    //* toolbar configuration changed
    void toolbarConfigurationChanged();

    protected:

    //* generic event
    bool event( QEvent* ) override;

    //* true if main window has menu
    bool _hasMenuBar() const;

    //* true if main window has menu
    bool _hasStatusBar() const;

    //* true if main window has toolbars
    bool _hasToolBars() const;

    //* returns true if hideable toolbars are found
    bool _hasHideableToolBars() const;

    //* returns true if lockable toolbars are found
    bool _hasLockableToolBars() const;

    //* returns true if toolbars with customizable icon size are found
    bool _hasIconSizeToolBars() const;

    //* returns true if toolbars with customizable toolbutton style are found
    bool _hasToolButtonStyleToolBars() const;

    //* true if main window has dock widgets
    bool _hasPanels() const;

    //* action list
    using ActionList = QVector<QAction*>;

    //* toolbar actions list
    ActionList _toolBarsActions( QMenu* );

    //* maximize state prior to minimization
    bool _wasMaximized() const
    { return wasMaximized_; }

    //* maximize state prior to minimization
    void _setWasMaximized( bool value )
    { wasMaximized_ = value; }

    //* update statusbar visibility
    void _updateStatusBarVisibility();

    private:

    //* update configuration
    void _updateConfiguration();

    //* toolbar text position
    void _updateToolButtonStyle( int );

    //* toolbar text position
    void _updateToolButtonIconSize( IconSize::Size );

    //* update layout actions
    void _updateLayoutActions();

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

    //* menubar action
    ApplicationMenu* applicationMenu_ = nullptr;

    //* window state prior to minimization
    bool wasMaximized_ = false;

};

#endif
