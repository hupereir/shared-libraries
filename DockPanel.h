#ifndef DockPanel_h
#define DockPanel_h

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

//! detachable generic panel
class DockPanel: public QWidget, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    DockPanel( QWidget* parent );

    //! destructor
    virtual ~DockPanel();

    //! option name (needed to store sticky and stays-on-top state)
    void setOptionName( QString value )
    {
        stickyOptionName_ = value + "_STICKY";
        staysOnTopOptionName_ = value + "_STAYS_ON_TOP";
        _updateConfiguration();
    }

    //! get panel (to add contents)
    virtual QWidget& panel( void )
    { return *panel_; }

    //! set detachable group panel title
    void setTitle( const QString& title )
    { title_ = title; }

    //! main widget
    QWidget& main( void ) const
    { return *main_; }

    signals:

    //! emmited when state is changed
    void attached( bool state );

    //! emmited when panel is attached
    void attached( void );

    //! emmited when panel is detached
    void detached( void );

    //! visibility changed
    void visibilityChanged( bool );


    protected slots:

    //! toggle dock
    virtual void _toggleDock( void );

    //! stays on top
    virtual void _toggleStaysOnTop( bool );

    //! toggle window stickyness
    virtual void _toggleSticky( bool );

    protected:

    //! hide event
    virtual void hideEvent( QHideEvent* );

    private slots:

    //! configuration
    void _updateConfiguration( void );

    private:

    //! true if option name is set
    bool _hasOptionName( void ) const
    { return !(_stickyOptionName().isEmpty() || _staysOnTopOptionName().isEmpty() ); }

    //! option name
    const QString& _stickyOptionName( void ) const
    { return stickyOptionName_; }

    //! option name
    const QString& _staysOnTopOptionName( void ) const
    { return staysOnTopOptionName_; }

    //! dock title
    QString title_;

    //! option name
    /*! needed to store sticky and stays on top state */
    QString stickyOptionName_;

    //! option name
    QString staysOnTopOptionName_;

    //! vertical layout for main_ widget
    QVBoxLayout* mainLayout_;

    //! detachable main widget
    QWidget* main_;

    //! contents panel
    QWidget* panel_;

    //! icon pixmap
    QPixmap pixmap_;

};

#endif
