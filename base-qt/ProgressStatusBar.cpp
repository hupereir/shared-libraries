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

#include "ProgressStatusBar.h"

#include "Debug.h"

//______________________________________________________
ProgressStatusBar::ProgressStatusBar( QWidget* parent ):
    BaseStatusBar( parent )
{
    Debug::Throw( "ProgressStatusBar::ProgressStatusBar.\n" );

    addPermanentWidget( stack_ = new QStackedWidget( this ), 1 );
    stack_->addWidget( label_ = new StatusBarLabel );

    progress_.reset( new QProgressBar );
    stack_->addWidget( progress_.get() );

}

//______________________________________________________
void ProgressStatusBar::setProgressBar( QProgressBar* value )
{
    progress_.reset( value );
    if( progress_ )
    { stack_->addWidget( progress_.get() ); }

}

//______________________________________________________
void ProgressStatusBar::showProgressBar()
{
    if( progress_ )
    { stack_->setCurrentWidget( progress_.get() ); }
}

//______________________________________________________
void ProgressStatusBar::showLabel()
{ stack_->setCurrentWidget( label_ ); }
