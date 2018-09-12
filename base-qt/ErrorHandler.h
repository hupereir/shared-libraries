#ifndef ErrorHandler_h
#define ErrorHandler_h

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

#include "NonCopyable.h"

#include <QApplication>
#include <QStringList>

class ErrorHandler: private Base::NonCopyable<ErrorHandler>
{

    public:

    //* singleton
    static ErrorHandler& get();

    //* install context
    static void initialize();

    //* adds a disabled message in the list
    void disableMessage( QString message )
    { disabledMessages_ << message; }

    //* adds a disabled message in the list
    void clearDisabledMessages()
    { disabledMessages_.clear(); }

    //* exit
    void exit();

    protected:

    //* constructor
    explicit ErrorHandler();

    using MessageList = QStringList;

    //* check/display Qt error messages
    #if QT_VERSION >= 0x050000
    static void _throw( QtMsgType, const QMessageLogContext&, const QString& );
    #endif

    //* check/display Qt error messages
    /** qt4 version */
    static void _throw( QtMsgType, const char* );

    private:

    //* all messages containing one of the string in the list are not displayed
    MessageList disabledMessages_;

};

#endif
