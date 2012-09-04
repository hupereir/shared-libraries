#ifndef ToolTipManager_h
#define ToolTipManager_h

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

#include "Counter.h"
#include "BaseFileInfo.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QObject>
#include <QtCore/QTimerEvent>

#include <QtGui/QWidget>
#include <QtGui/QIcon>

class ToolTipWidget;
class ToolTipManager: public QObject, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    ToolTipManager( QWidget* );

    //! destructor
    virtual ~ToolTipManager( void )
    {}

    //! enable state
    void setEnabled( bool );

    public slots:

    //! show
    void show( const BaseFileInfo&, const QIcon&, const QRect& );

    //! hide
    void hide( void );

    protected:

    //! timer event
    virtual void timerEvent( QTimerEvent* );

    private slots:

    //! update configuration
    void _updateConfiguration( void );

    private:

    //! enable state
    bool enabled_;

    //! tooltip widget
    ToolTipWidget* widget_;

    //! timer
    QBasicTimer timer_;

};

#endif
