#ifndef AnimatedTextEditor_h
#define AnimatedTextEditor_h

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

#include "TextEditor.h"

class TransitionWidget;

//! Animated text editor
/*! uses fading when calling setPlainText, setHTML or clear */
class AnimatedTextEditor: public TextEditor
{

    public:

    //! constructor
    AnimatedTextEditor( QWidget* parent = 0 );

    //! destrutor
    virtual ~AnimatedTextEditor( void );

    //! set text
    virtual void setPlainText( const QString& );

    //! set text
    virtual void setHtml( const QString& );

    //! clear
    virtual void clear( void );

    protected:

    //! transition widget
    TransitionWidget& _transitionWidget( void ) const
    { return *transitionWidget_; }

    private:

    //! transition widget
    TransitionWidget* transitionWidget_;


};

#endif
