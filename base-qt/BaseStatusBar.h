#ifndef BaseStatusBar_h
#define BaseStatusBar_h

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

#include <QApplication>
#include <QContextMenuEvent>
#include <QLabel>
#include <QList>
#include <QStatusBar>

//* local label for additional slots
class BASE_QT_EXPORT StatusBarLabel: public QLabel
{

    Q_OBJECT

    public:

    //* constructor
    explicit StatusBarLabel( QWidget* parent = nullptr ):
        QLabel( parent )
    {}

    //* set text
    void setText( const QString& message, bool alsoUpdate = true )
    {
        if( alsoUpdate ) setTextAndUpdate( message );
        else QLabel::setText( message );
    }

    //* set label text and process events
    void setTextAndUpdate( const QString& );

};

//* custom status bar
class BASE_QT_EXPORT BaseStatusBar: public QStatusBar, private Base::Counter<BaseStatusBar>
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseStatusBar( QWidget* );

    //* add clock
    void addClock();

    //* add label
    void addLabel( int stretch = 0 );

    //* add labels
    void addLabels( int n, int stretch = 0 )
    { for( int i=0; i<n; ++i ) addLabel( stretch ); }

    //* retrieves label with given index
    virtual StatusBarLabel& label( int i = 0  ) const
    { return *labels_[i]; }

    protected:

    //* context menu
    void contextMenuEvent( QContextMenuEvent* ) override;

    private:

    //* vector of output labels.
    QList<StatusBarLabel*> labels_;

};

#endif
