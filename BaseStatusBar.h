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

#include "AnimatedLabel.h"
#include "Counter.h"

#include <QApplication>
#include <QContextMenuEvent>
#include <QStatusBar>

#include <QList>

//* local label for additional slots
class StatusBarLabel: public AnimatedLabel
{

    //* Qt meta object macro
    Q_OBJECT

    public:
    StatusBarLabel( QWidget* parent = 0 ):
    AnimatedLabel( parent )
    {}

    public Q_SLOTS:

    //* set text
    virtual void setText( const QString& message, bool alsoUpdate = true )
    {
        if( alsoUpdate ) setTextAndUpdate( message );
        else AnimatedLabel::setText( message );
    }

    //* set label text and process events
    void setTextAndUpdate( const QString& );

};

//* custom status bar
class BaseStatusBar: public QStatusBar, public Counter
{

    public:

    //* constructor
    BaseStatusBar( QWidget* );

    //* destructor
    ~BaseStatusBar( void )
    {}

    //* add clock
    void addClock( void );

    //* add label
    void addLabel( const int& stretch = 0, bool animated = false );

    //* add labels
    void addLabels( const unsigned int& n, const int& stretch = 0, bool animated = false )
    { for( unsigned int i=0; i<n; i++ ) addLabel( stretch, animated ); }

    //* retrieves label with given index
    virtual StatusBarLabel& label( int i = 0  ) const
    { return *labels_[i]; }

    protected:

    //* context menu
    virtual void contextMenuEvent( QContextMenuEvent* );

    private:

    //* vector of output labels.
    QList< StatusBarLabel* > labels_;

};

#endif
