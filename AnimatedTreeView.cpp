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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "AnimatedTreeView.h"
#include "AnimatedTreeView.moc"
#include "Debug.h"
#include "ScrollObject.h"
#include "TransitionWidget.h"

//________________________________________________________________________
AnimatedTreeView::AnimatedTreeView( QWidget* parent ):
TreeView( parent ),
transitionWidget_( new TransitionWidget( this ) )
{
    Debug::Throw( "AnimatedTreeView::AnimatedTreeView.\n" );

    // transition widget
    transitionWidget().setFlag( TransitionWidget::FROM_PARENT, false );
    transitionWidget().hide();
    connect( &transitionWidget().timeLine(), SIGNAL(finished()), &transitionWidget(), SLOT(hide()) );

    setVerticalScrollMode( QAbstractItemView::ScrollPerPixel );
    setHorizontalScrollMode( QAbstractItemView::ScrollPerPixel );

    new ScrollObject( this );

}

//________________________________________________________________________
bool AnimatedTreeView::initializeAnimation( void )
{

    // transition
    if( transitionWidget().isEnabled() && QTreeView::isVisible() )
    {
        transitionWidget().initialize();
        return true;
    } else return false;

}

//________________________________________________________________________
bool AnimatedTreeView::startAnimation( void )
{
    // transition
    if( transitionWidget().isEnabled() && QTreeView::isVisible() )
    {
        transitionWidget().start();
        return true;
    } else return false;

}
