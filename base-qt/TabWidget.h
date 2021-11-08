#ifndef TabWidget_h
#define TabWidget_h

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
#include "Debug.h"
#include "WidgetDragMonitor.h"
#include "base_qt_export.h"

#include <QAction>
#include <QBasicTimer>
#include <QCloseEvent>
#include <QLabel>
#include <QLayout>
#include <QSizeGrip>
#include <QTabWidget>

#include <memory>

// forward declaration
namespace Private
{
    class LocalTabWidget;
}

//* Tab Child container
class BASE_QT_EXPORT TabWidget: public QWidget, private Base::Counter<TabWidget>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit TabWidget( QTabWidget* parent );

    //* destructor
    ~TabWidget() override;

    //*@name accessors
    //@{

    //* title
    const QString& title() const
    { return title_; }

    //* get parent TabWidget
    QTabWidget& parentTabWidget()
    { return *parent_; }

    //* true if attached
    bool isDetached() const
    { return widgetDragMonitor_.isEnabled(); }

    //* detach action
    QAction& detachAction() const
    { return *detachAction_; }

    //* stay on top
    QAction& staysOnTopAction() const
    { return *staysOnTopAction_; }

    //* widget is hidden from taskbar
    QAction& stickyAction() const
    { return *stickyAction_; }

    //@}

    //*@name modifiers
    //@{

    //* change title
    void setTitle( const QString &);

    //* update actions
    void updateActions( bool );

    //@}

    Q_SIGNALS:

    //* emitted when state is changed
    void attached( bool state );

    //* emitted when box is detached
    void detached();

    //* emitted when box is attached
    void attached();

    private:

    //* toggle dock
    void _toggleDock();

    //* stays on top
    void _toggleStaysOnTop( bool );

    //* toggle window stickyness
    void _toggleSticky( bool );

    //* update context menu
    void _updateContextMenu( QPoint  );

    //* actions
    void _installActions();

    //* parent TabWidget
    QTabWidget* parent_ = nullptr;

    //* title
    QString title_;

    //* dock title
    QLabel* dockTitleLabel_ = nullptr;

    //* dock
    std::unique_ptr<Private::LocalTabWidget> dock_;

    //* index in parent tab
    int index_ = 0;

    //*@name actions
    //@{

    //* attach/detach action
    QAction* detachAction_ = nullptr;

    //* stay on top
    QAction* staysOnTopAction_ = nullptr;

    //* make window sticky
    QAction* stickyAction_ = nullptr;

    //@}

    //* widget drag monitor
    WidgetDragMonitor widgetDragMonitor_;

};
#endif
