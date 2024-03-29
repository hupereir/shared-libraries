#ifndef TextEncodingWidget_h
#define TextEncodingWidget_h

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
#include "base_qt_export.h"

#include <QWidget>

class TreeView;
class TextEncodingModel;
class BASE_QT_EXPORT TextEncodingWidget: public QWidget, private Base::Counter<TextEncodingWidget>
{


    Q_OBJECT

    public:

    //* constructor
    explicit TextEncodingWidget( QWidget* = nullptr );

    //*@name accessors
    //@{

    QByteArray encoding() const
    { return qPrintable( editor_->text() ); }

    //@}

    //*@name modifiers
    //@{

    //* set text encoding
    void setEncoding( const QByteArray& );

    //@}

    private:

    //* update selection
    void _updateSelection();

    //* find
    void _find(const QString&);

    //* load text codecs
    void _loadTextCodecs();

    //* list
    TreeView* list_ = nullptr;

    //* editor
    LineEditor* editor_ = nullptr;

    //* model
    TextEncodingModel* model_ = nullptr;

    //* lock
    bool locked_ = false;

};

#endif
