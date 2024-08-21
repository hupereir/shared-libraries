#ifndef InterruptionHandler_h
#define InterruptionHandler_h


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
#include "base_qt_export.h"

class BASE_QT_EXPORT InterruptionHandler: private Base::NonCopyable<InterruptionHandler>
{
    public:

    //* singleton
    static InterruptionHandler& get();

    //* initialize
    static void initialize();

    //* change interruption handler function
    using InterruptionHandlerFuntion = void (*)( int );
    void setInterruptionHandlerFunction( InterruptionHandlerFuntion );

    private:

    //* constructor
    explicit InterruptionHandler() = default;

    //* interruption handler
    static void _handleInterruption( int );

    //* default interuption handler function
    InterruptionHandlerFuntion function_ = _handleInterruption;

    //* initialize
    bool initialized_ = false;

};

#endif
