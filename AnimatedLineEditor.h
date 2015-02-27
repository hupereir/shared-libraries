#ifndef AnimatedLineEditor_h
#define AnimatedLineEditor_h

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

#include <QTimeLine>

#include "LineEditor.h"

class TransitionWidget;

//! Animated text editor
/*! uses fading when calling setPlainText, setHTML or clear */
class AnimatedLineEditor: public LineEditor
{

    Q_OBJECT

        public:

        //! constructor
        AnimatedLineEditor( QWidget* parent = 0 );

    //! destrutor
    virtual ~AnimatedLineEditor( void );

    //! timeline
    QTimeLine& timeLine( void )
    { return timeLine_; }

    public Q_SLOTS:

    //! clear
    virtual void clear( void );

    //! set text
    virtual void setText( const QString& );

    private Q_SLOTS:

    //! configuration
    virtual void _updateConfiguration( void );

    protected:

    //! transition widget
    TransitionWidget& _transitionWidget( void ) const
    { return *transitionWidget_; }

    private:

    //! timeline
    QTimeLine timeLine_;

    //! transition widget
    TransitionWidget* transitionWidget_;


};

#endif
