#ifndef LineEditor_p_h
#define LineEditor_p_h

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
#include "LineEditor.h"

#include <QAbstractButton>
#include <QProxyStyle>

//* proxy style for custom line editors
namespace Private
{

    class LineEditorStyle: public QProxyStyle, private Base::Counter<LineEditorStyle>
    {

        Q_OBJECT

        public:

        //* constructor
        explicit LineEditorStyle( QStyle* = nullptr );

        //* returns rect corresponding to one widget's subcontrol
        QRect subElementRect( SubElement, const QStyleOption*, const QWidget* ) const override;

        //* returns size matching contents
        QSize sizeFromContents( ContentsType, const QStyleOption*, const QSize&, const QWidget* ) const override;

    };

    class LineEditorButton: public QAbstractButton, private Base::Counter<LineEditorButton>
    {

        public:

        //* constructor
        explicit LineEditorButton( QWidget* parent );

        //* size hint
        QSize sizeHint() const override;

        protected:

        //* paint
        void paintEvent( QPaintEvent* ) override;

    };

}

#endif
