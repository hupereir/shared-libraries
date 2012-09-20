#ifndef ProgressStatusBar_h
#define ProgressStatusBar_h

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

#include "BaseStatusBar.h"
#include "Debug.h"

#include <QtGui/QProgressBar>
#include <QtGui/QStackedWidget>

//! customized status bar for selection frame
class ProgressStatusBar: public BaseStatusBar
{

    Q_OBJECT

    public:

    //! constructor
    ProgressStatusBar( QWidget* parent );

    //! destructor
    virtual ~ProgressStatusBar( void )
    {}

    //! progress bar
    QProgressBar& progressBar( void ) const
    { return *progress_; }

    //! set progress bar
    void setProgressBar( QProgressBar* );

    //! retrieves label with given index
    virtual StatusBarLabel& label( const unsigned int& i = 0  ) const
    { return (i==0) ? *label_:BaseStatusBar::label(i); }

    public slots:

    //! show progress bar as visible widget
    void showProgressBar( void );

    //! show label as visible widget
    void showLabel( void );

    private:

    //! stack widget
    QStackedWidget* stack_;

    //! progress bar
    QProgressBar* progress_;

    //! label
    StatusBarLabel* label_;

};

#endif
