#ifndef SelectLineWidget_h
#define SelectLineWidget_h

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
#include "EmbeddedWidget.h"
#include "base_qt_export.h"

#include <QAbstractButton>
#include <QPalette>
#include <QWidget>

class LineEditor;

//! find_text dialog for text editor widgets
class BASE_QT_EXPORT SelectLineWidget: public EmbeddedWidget, private Base::Counter<SelectLineWidget>
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    explicit SelectLineWidget( QWidget* = nullptr, bool compact = true );

    //!@name accessors
    //@{

    //! retrieve editor
    LineEditor& editor() const
    { return *editor_; }

    //! ok button
    QAbstractButton& okButton() const
    { return *okButton_; }

    //! close button
    QAbstractButton& closeButton() const
    { return *closeButton_; }

    //@}

    //!@name modifiers
    //@{

    //! take action when at least one match is found
    void matchFound();

    //! take action when no match is found
    void noMatchFound();

    //! visibility
    void setVisible(bool) override;

    //@}

    Q_SIGNALS:

    //! emitted when widget visibility is changed
    void visibilityChanged(bool);

    //! emitted when pressing the Ok button
    void lineSelected( int );

    protected:

    //! change event
    void changeEvent( QEvent* ) override;

    private:

    //! retrieve line number and emit signal
    void _selectLine();

    //! create not found palette
    void _updateNotFoundPalette();

    //! line editor for text to find
    LineEditor* editor_ = nullptr;

    //! ok button
    QAbstractButton* okButton_ = nullptr;

    //! cancel button
    QAbstractButton* closeButton_ = nullptr;

    //! not found palette
    QPalette notFoundPalette_;

};
#endif
