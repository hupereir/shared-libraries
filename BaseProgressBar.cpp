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

#include "BaseProgressBar.h"
#include "BaseProgressBar.moc"
#include "Debug.h"

#include <QStyleOptionProgressBarV2>
#include <QStylePainter>

//___________________________________________________________
void BaseProgressBar::setText( const QString& text )
{
    if( text_ == text ) return;
    text_ = text;
    update();
}

//___________________________________________________________
void BaseProgressBar::paintEvent( QPaintEvent* event )
{

    // default painting if no text is set
    if( text_.isEmpty() ) return QProgressBar::paintEvent( event );
    else {

        QStylePainter painter(this);
        painter.setClipRegion( event->region() );
        QStyleOptionProgressBarV2 opt;
        initStyleOption(&opt);
        opt.text = text_;
        painter.drawControl(QStyle::CE_ProgressBar, opt);

    }
}
