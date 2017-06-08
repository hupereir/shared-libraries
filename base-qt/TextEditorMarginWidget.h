#ifndef TextEditorMarginWidget_h
#define TextEditorMarginWidget_h

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

#include <QEvent>
#include <QPaintEvent>
#include <QWidget>

class TextEditor;

// margin widget
class TextEditorMarginWidget: public QWidget, private Base::Counter<TextEditorMarginWidget>
{

    Q_OBJECT

    public:

    //* constructor
    explicit TextEditorMarginWidget( TextEditor* );

    //*@name accessors
    //@{

    //* vertical line
    bool drawVerticalLine( void ) const
    { return true; }

    //* background color
    QColor backgroundColor( void ) const;

    //* foreground color
    QColor foregroundColor( void ) const;

    //@}

    public Q_SLOTS:

    //* dirty
    void setDirty( void );

    protected:

    //* paint event
    virtual void paintEvent( QPaintEvent* );

    //* parent editor
    TextEditor& _editor( void ) const
    { return *editor_; }

    private:

    //* parent editor
    TextEditor* editor_ = nullptr;

    //* dirty flag
    bool dirty_ = false;

};

#endif
