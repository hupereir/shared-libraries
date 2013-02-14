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
#include "AnimatedLineEditor.h"

#include <QColorDialog>

class ColorGrabButton;

//! used to display colors and a choose button
class ColorDisplay: public QWidget, public Counter
{

    //! Qt metaobject macro
    Q_OBJECT

    public:

    //! default name for no color
    static const QString NONE;

    //! constructor
    ColorDisplay( QWidget* parent );

    //! retrieve color name
    QString colorName( void ) const
    {
        QString out( editor_.text() );
        return out.isEmpty() ? NONE:out;
    }

    public slots:

    //! set color
    void setColor( QString );

    private slots:

    //! change color (from button)
    void _selectColorFromDialog( void );

    //! change color (from line editor)
    void _selectColorFromText( void );

    private:

    //! internal customized label to have correct background color
    class LocalLineEdit : public AnimatedLineEditor
    {

        public:

        //! constructor
        LocalLineEdit( QWidget *parent ):
            AnimatedLineEditor( parent )
        {}

        //! retrieve color
        QColor color( void ) const;

        //! set color
        void setColor( QColor color );

    };

    //! label used to display the color
    LocalLineEdit editor_;

    //! grab button
    ColorGrabButton* grabButton_;

};
#endif
