#ifndef ErrorHandler_h
#define ErrorHandler_h

// $Id$

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

#include <QtGui/QApplication>
#include <QtCore/QStringList>

/*!
\class ErrorHandler
\brief Customized Qt error handler. Messages in the disabledMessages_ list
are skipped.
*/

class ErrorHandler
{

    public:

    //! check/display Qt error messages
    static void Throw( QtMsgType type, const char* message );

    //! singleton
    static ErrorHandler& get( void );

    //! adds a disabled message in the list
    void disableMessage( const QString& message )
    { disabledMessages_ << message; }

    //! adds a disabled message in the list
    void clearDisabledMessages( void )
    { disabledMessages_.clear(); }

    //! exit
    void exit( void ) const
    {}

    protected:

    //! constructor
    ErrorHandler( void );

    typedef QStringList MessageList;

    //! disabled message
    const MessageList& _disabledMessages( void ) const
    { return disabledMessages_; }

    private:

    //! all messages containing one of the string in the list are not displayed
    MessageList disabledMessages_;

};

#endif
