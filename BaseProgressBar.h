#ifndef BaseProgressBar_h
#define BaseProgressBar_h

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

#include <QtCore/QString>
#include <QtGui/QPaintEvent>
#include <QtGui/QProgressBar>

//! display command progress and remaining time
class BaseProgressBar:public QProgressBar
{

    Q_OBJECT

    public:

    //! constructor
    BaseProgressBar( QWidget* parent = 0 ):
        QProgressBar( parent )
    {}

    //! destructor
    virtual ~BaseProgressBar( void )
    {}

    //! reset
    virtual void reset( void )
    {
        text_ = QString();
        QProgressBar::reset();
    }

    public slots:

    //! set maximum
    virtual void setMaximum( int value )
    { QProgressBar::setMaximum( value ); }

    //! change displayed text
    virtual void setText( const QString& );

    protected:

    //! paint
    void paintEvent( QPaintEvent* );

    private:

    //! text
    QString text_;

};

#endif
