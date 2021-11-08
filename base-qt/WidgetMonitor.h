#ifndef WidgetMonitor_h
#define WidgetMonitor_h

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

#include <QBasicTimer>
#include <QSize>
#include <QTimerEvent>
#include <QWidget>

//* used to automatically save size of top level widgets
class BASE_QT_EXPORT WidgetMonitor: public QObject, private Base::Counter<WidgetMonitor>
{

    Q_OBJECT

    public:

    //* constructor
    explicit WidgetMonitor( QWidget* );

    //* mode
    enum Mode
    {
        None = 0,
        Size = 1<<0,
        Position = 1<<1
    };

    Q_DECLARE_FLAGS( Modes, Mode );

    //* mode
    void setMode( Modes value )
    { mode_ = value; }

    //* size option name
    void setOptionName( const QString& );

    //* size hint
    /** an invalid size is returned when option name is not set */
    QSize sizeHint() const;

    //* position
    QPoint position() const;

    //* event filter
    bool eventFilter( QObject*, QEvent* ) override;

    protected:

    //* timer event
    /** need to save updated window size */
    void timerEvent( QTimerEvent* ) override;

    private:

    //* true when option name was set
    bool _hasOptionName() const
    { return !widthOptionName_.isEmpty(); }

    //* save window size
    void _saveWindowSize() const;

    //* save position
    void _saveWindowPosition() const;

    //* resize timer
    QBasicTimer timer_;

    //* mode
    Modes mode_ = Mode::Size;

    //* window size option name
    QString widthOptionName_;

    //* window size option name
    QString heightOptionName_;

    //* position option name
    QString xOptionName_;

    //* position option name
    QString yOptionName_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( WidgetMonitor::Modes )

#endif
