#ifndef LogWidget_h
#define LogWidget_h

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
#include "TextFormat.h"
#include "base_qt_export.h"

#include <QString>
#include <QColor>

class BASE_QT_EXPORT LogWidget: public TextEditor
{

    Q_OBJECT

    public:

    //* constructor
    explicit LogWidget( QWidget* = nullptr );

    //* option name
    void setOptionName( const QString& );

    //* verbosity
    int verbosity() const
    { return verbosity_; }

    //* verbosity
    void setVerbosity( int value )
    { verbosity_ = value; }

    //* append text
    void append( const QString& value, TextFormat::Flags format = TextFormat::Default )
    { append( value, format, QColor(), 0 ); }

    void append( const QString&, TextFormat::Flags, const QColor&, int verbosity = 0 );

    //* clear
    /*
    need to bypass TextEditor::clear,
    which is disabled in read-only mode
    */
    void clear() override
    { BaseEditor::clear(); }

    protected:

    //* keypress event
    void keyPressEvent( QKeyEvent* ) override;

    //* wheel events
    void wheelEvent( QWheelEvent* ) override;

    //* wrap option name
    QString _wrapOptionName()
    { return optionName_ + "_WRAP"; }

    //* option name
    bool _hasOptionName()
    { return !optionName_.isEmpty(); }

    //* lock
    bool _locked() const
    { return locked_; }

    private:

    //* toggle wrap mode
    bool _toggleWrapMode( bool ) override;

    //* slider
    void _verticalScrollBarMoved( int );

    //* verbosity
    int verbosity_ = 0;

    //* option name
    QString optionName_;

    //* true if locked
    bool locked_ = false;

};

#endif
