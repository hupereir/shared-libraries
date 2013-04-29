// $Id$
#ifndef _ColorDisplay_h_
#define _ColorDisplay_h_

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
#include "AnimatedLineEditor.h"

//! used to display colors and a choose button
class ColorDisplay: public QWidget, public Counter
{

    //! Qt metaobject macro
    Q_OBJECT

    public:

    //! constructor
    ColorDisplay( QWidget* parent );

    //! color
    QColor color( void ) const
    { return editor_->color(); }

    public slots:

    //! set color
    void setColor( QColor color )
    { editor_->setColor( color ); }

    private slots:

    //! change color (from button)
    void _selectColorFromDialog( void );

    //! change color (from line editor)
    void _selectColorFromText( void );

    private:

    //! internal customized label to have correct background color
    class Editor : public AnimatedLineEditor
    {

        public:

        //! constructor
        Editor( QWidget *parent ):
            AnimatedLineEditor( parent )
        {}

        //! retrieve color
        QColor color( void ) const;

        //! set color
        void setColor( QColor );

    };

    //! label used to display the color
    Editor* editor_;

};
#endif
