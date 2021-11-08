#ifndef ProgressStatusBar_h
#define ProgressStatusBar_h

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

#include "BaseStatusBar.h"
#include "Debug.h"
#include "base_qt_export.h"

#include <QProgressBar>
#include <QStackedWidget>

#include <memory>

//* customized status bar for selection frame
class BASE_QT_EXPORT ProgressStatusBar: public BaseStatusBar
{

    Q_OBJECT

    public:

    //* constructor
    explicit ProgressStatusBar( QWidget* );

    //* progress bar
    QProgressBar& progressBar() const
    { return *progress_.get(); }

    //* set progress bar
    void setProgressBar( QProgressBar* );

    //* retrieves label with given index
    StatusBarLabel& label( int i = 0  ) const override
    { return (i==0) ? *label_:BaseStatusBar::label(i); }

    //* show progress bar as visible widget
    void showProgressBar();

    //* hide progress bar
    void hideProgressBar()
    { showLabel(); }

    //* show label as visible widget
    void showLabel();

    private:

    //* stack widget
    QStackedWidget* stack_ = nullptr;

    //* progress bar
    std::unique_ptr<QProgressBar> progress_;

    //* label
    StatusBarLabel* label_ = nullptr;

};

#endif
