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

// forward declaration
namespace Private
{
    class LocalDockWidget;
    class LocalWidget;
}

//* detachable generic panel
class DockPanel: public QWidget, public Counter
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    DockPanel( QWidget* parent );

    //* destructor
    virtual ~DockPanel();

    //*@name accessors
    //@{

    //* get panel (to add contents)
    virtual QWidget& panel( void );

    //* true if detached
    bool isDetached( void ) const;

    //@}

    //*@name modifiers
    //@{

    //* option name (needed to store sticky and stays-on-top state)
    void setOptionName( QString );

    //* set detachable group panel title
    void setTitle( QString );

    //@}

    Q_SIGNALS:

    //* emitted when state is changed
    void attached( bool state );

    //* emitted when panel is attached
    void attached( void );

    //* emitted when panel is detached
    void detached( void );

    //* visibility changed
    void visibilityChanged( bool );

    protected Q_SLOTS:

    //* toggle dock
    virtual void _toggleDock( void );

    protected:

    //* hide event
    virtual void hideEvent( QHideEvent* );

    private:

    //* vertical layout for main widget
    QVBoxLayout* mainLayout_ = nullptr;

    //* dock title
    QLabel* dockTitleLabel_ = nullptr;

    //* dock
    Private::LocalDockWidget* dock_ = nullptr;

    //* contents panel
    Private::LocalWidget* panel_ = nullptr;

};

#endif
