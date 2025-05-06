#ifndef LineEditorButton_h
#define LineEditorButton_h

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
#include "base_qt_export.h"

#include <QToolButton>


//! used to insert buttons into a line editor
class BASE_QT_EXPORT LineEditorButton: public QToolButton, private Base::Counter<LineEditorButton>
{

    Q_OBJECT

    public:

    //! constructor
    explicit LineEditorButton( QWidget* = nullptr );

    //!@name accessors
    //@{

    //! size hint
    QSize sizeHint() const override;

    //@}

    //!@name modifiers
    //@{
    //! visibility
    void setVisible(bool) override;
    //@}

    Q_SIGNALS:

    //! emited when visibility is changed
    void visibilityChanged(bool);

    protected:

    //! paint
    void paintEvent( QPaintEvent* ) override;

};

#endif
