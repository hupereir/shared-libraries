#ifndef DockPanel_h
#define DockPanel_h

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
#include "Counter.h"
#include "Debug.h"

#include <QBasicTimer>
#include <QTimerEvent>

#include <QAction>
#include <QCloseEvent>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QPixmap>

#include <memory>

// forward declaration
namespace Private
{
    class LocalDockWidget;
    class LocalWidget;
}

//* detachable generic panel
class BASE_QT_EXPORT DockPanel: public QWidget, private Base::Counter<DockPanel>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    explicit DockPanel( QWidget* parent );

    //* destructor (needed because of unique_ptr member
    ~DockPanel() override;

    //*@name accessors
    //@{

    //* get panel (to add contents)
    QWidget& panel();

    //* true if detached
    bool isDetached() const;

    //@}

    //*@name modifiers
    //@{

    //* option name (needed to store sticky and stays-on-top state)
    void setOptionName( const QString &);

    //* set detachable group panel title
    void setTitle( const QString &);

    //@}

    Q_SIGNALS:

    //* emitted when state is changed
    void attached( bool state );

    //* emitted when panel is attached
    void attached();

    //* emitted when panel is detached
    void detached();

    private:

    //* toggle dock
    void _toggleDock();

    //* vertical layout for main widget
    QVBoxLayout* mainLayout_ = nullptr;

    //* dock title
    QLabel* dockTitleLabel_ = nullptr;

    //* dock
    std::unique_ptr<Private::LocalDockWidget> dock_;

    //* contents panel
    Private::LocalWidget* panel_ = nullptr;

};

#endif
