#ifndef AnimatedTreeView_h
#define AnimatedTreeView_h

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

#include "TreeView.h"

class TransitionWidget;

//! customized tree view
class AnimatedTreeView: public TreeView
{

    Q_OBJECT

    public:

    //! constructor
    AnimatedTreeView( QWidget* = 0 );

    //! destructor
    virtual ~AnimatedTreeView( void )
    {}

    //! initialize animation
    bool initializeAnimation( void );

    //! start animation
    bool startAnimation( void );

    //! transition widget
    TransitionWidget& transitionWidget( void ) const
    { return *transitionWidget_; }

    private:

    //! transition widget
    TransitionWidget* transitionWidget_;

};

#endif
