#ifndef AnimatedTabWidget_h
#define AnimatedTabWidget_h

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

/*!
\file AnimatedTabWidget.h
\brief animated tabbed widget
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "Counter.h"

#include <QTabBar>
#include <QTabWidget>

class TransitionWidget;

//! tabbar
class AnimatedTabBar: public QTabBar, public Counter
{

    Q_OBJECT

    public:

    //! constructor
    AnimatedTabBar( QWidget* parent ):
        QTabBar( parent ),
        Counter( "AnimatedTabBar" )
    {}

    Q_SIGNALS:

    //! clicked
    void clicked( void );

    protected:

    //! mouse press event
    virtual void mousePressEvent( QMouseEvent* );

};

//! tabbed dialog
/*! a list of tab names appear on the left. The contents of the corresponding tag appear on the right */
class AnimatedTabWidget: public QTabWidget, public Counter
{

    Q_OBJECT

    public:

    //! creator
    AnimatedTabWidget( QWidget *parent );

    //! destructor
    virtual ~AnimatedTabWidget();

    protected Q_SLOTS:

    //! update current widget
    void _updateCurrentWidget( void );

    //! animation finished
    void _startAnimation( void );

    //! animation finished
    void _animationFinished( void );

    protected:

    //! transition widget
    TransitionWidget& _transitionWidget( void ) const
    { return *transitionWidget_; }

    private:

    //! transitionWidget
    TransitionWidget* transitionWidget_;

};


#endif
