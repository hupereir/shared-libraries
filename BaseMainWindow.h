#ifndef BaseMainWindow_h
#define BaseMainWindow_h

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

#include "CustomToolButton.h"
#include "Debug.h"
#include "IconSize.h"
#include "WindowMonitor.h"
#include "XmlOptions.h"

#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QStatusBar>
#include <QtGui/QStyle>

class ToolBarMenu;

/*!
\class BaseMainWindow
\brief customized QDialog
*/
class BaseMainWindow: public QMainWindow
{

    Q_OBJECT

    public:

    //! constructor
    BaseMainWindow( QWidget *parent, Qt::WFlags wflags = 0);

    //! destructor
    virtual ~BaseMainWindow( void )
    { Debug::Throw( "BaseMainWindow::~BaseMainWindow.\n" ); }

    //! restore window size
    virtual QSize minimumSizeHint( void ) const;

    //! restore window size
    virtual QSize sizeHint( void ) const;

    //! center widget on argument widget
    void centerOnDesktop( void );

    //! center on parent widget
    void centerOnParent( void )
    { centerOnWidget( parentWidget() ); }

    //! center widget on argument widget
    void centerOnWidget( QWidget* );

    //! window title
    virtual void setWindowTitle( const QString& );

    //! set menu bar (overloaded)
    void setMenuBar( QMenuBar* );

    //! set menu bar (overloaded)
    void setStatusBar( QStatusBar* );

    //! lock toolbars
    QAction& lockToolBarsAction( void ) const
    { return *lockToolBarsAction_; }

    //! lock panels
    QAction& lockPanelsAction( void ) const
    { return *lockPanelsAction_; }

    //! show menubar
    QAction& showMenuBarAction( void ) const
    { return *showMenuBarAction_; }

    //! show status bar
    QAction& showStatusBarAction( void ) const
    { return *showStatusBarAction_; }

    //! create context menu (overloaded)
    virtual QMenu* createPopupMenu( void );

    //! toolbar menu
    virtual ToolBarMenu& toolBarMenu( QWidget* parent = 0 );

    //! install toolbar visibility actions
    /*! returns true if lockable toolbars are found */
    virtual bool installToolBarsActions( QMenu& );

    //! option name
    virtual void setOptionName( const QString& name );

    //! menu option name
    bool hasOptionName( void ) const
    { return !lockToolBarsOptionName().isEmpty(); }

    //! lock toolbar option name
    const QString& lockToolBarsOptionName( void ) const
    { return lockToolBarsOptionName_; }

    //! lock panels option name
    const QString& lockPanelsOptionName( void ) const
    { return lockPanelsOptionName_; }

    //! menu option name
    const QString& showMenuBarOptionName( void ) const
    { return showMenuBarOptionName_; }

    //! status bar option name
    const QString& showStatusBarOptionName( void ) const
    { return showStatusBarOptionName_; }

    signals:

    //! toolbar configuration changed
    void toolbarConfigurationChanged( void );

    public slots:

    //! uniconify
    void uniconify( void );

    protected:

    //! generic event
    virtual bool event( QEvent* );

    //! true if main window has menu
    virtual bool _hasMenuBar( void ) const;

    //! true if main window has menu
    virtual bool _hasStatusBar( void ) const;

    //! true if main window has toolbars
    virtual bool _hasToolBars( void ) const;

    //! true if main window has dock widgets
    virtual bool _hasPanels( void ) const;

    //! maximize state prior to minimization
    bool _wasMaximized( void ) const
    { return wasMaximized_; }

    //! maximize state prior to minimization
    void _setWasMaximized( bool value )
    { wasMaximized_ = value; }

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    //! toolbar text position
    void _updateToolButtonStyle( int );

    //! toolbar text position
    void _updateToolButtonIconSize( IconSize::Size );

    //! lock toolbars
    void _lockToolBars( bool );

    //! lock panels
    void _lockPanels( bool );

    //! toggle menu
    void _toggleMenuBar( bool );

    //! toggle status bar
    void _toggleStatusBar( bool );

    private:

    //! option name
    QString lockToolBarsOptionName_;

    //! option name
    QString lockPanelsOptionName_;

    //! option name
    QString showMenuBarOptionName_;

    //! option name
    QString showStatusBarOptionName_;

    //! size watcher
    WindowMonitor monitor_;

    //! lock toolbars
    QAction* lockToolBarsAction_;

    //! lock docks
    QAction* lockPanelsAction_;

    //! toggle menu
    QAction* showMenuBarAction_;

    //! toggle statusBar
    QAction* showStatusBarAction_;

    //! window state prior to minimization
    bool wasMaximized_;

};

#endif

