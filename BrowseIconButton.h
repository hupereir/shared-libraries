#ifndef BrowseIconButton_h
#define BrowseIconButton_h

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

#include "CustomToolButton.h"
#include "Counter.h"
#include "Debug.h"


#include <QString>

//! customized tool button to display tooltip in a dedicated label
class BrowseIconButton: public CustomToolButton
{

    //! Qt meta object macros
    Q_OBJECT

    public:

    //! no icon filename
    static const QString NO_ICON;

    //! creator
    BrowseIconButton(
        QWidget* parent,
        const QString& file = NO_ICON
        );

    //! destructor
    ~BrowseIconButton( void )
    {}

    //! iconFile
    virtual const QString& file( void ) const
    { return file_; }

    //! iconFile
    virtual bool setFile( const QString& file, bool check );

    protected:

    //! drag enter event
    virtual void dragEnterEvent( QDragEnterEvent* );

    //! drop event
    virtual void dropEvent( QDropEvent* );

    protected Q_SLOTS:

    //! opens file dialog to browse icon file
    virtual void _browse( void );

    private:

    //! empty pixmap
    QPixmap noIconPixmap_;

    //! icon file
    QString file_;

};

#endif
