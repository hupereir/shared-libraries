#ifndef BaseProgressBar_h
#define BaseProgressBar_h

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

#include "base_qt_export.h"
#include <QString>
#include <QPaintEvent>
#include <QProgressBar>

//* display command progress and remaining time
class BASE_QT_EXPORT BaseProgressBar:public QProgressBar
{

    Q_OBJECT

    public:

    //* constructor
    explicit BaseProgressBar( QWidget* parent = nullptr ):
        QProgressBar( parent )
    { setTextVisible( true ); }

    //* reset
    virtual void reset()
    {
        text_ = QString();
        QProgressBar::reset();
    }

    //* set maximum
    virtual void setMaximum( int value )
    { QProgressBar::setMaximum( value ); }

    //* change displayed text
    virtual void setText( const QString& );

    protected:

    //* paint
    void paintEvent( QPaintEvent* ) override;

    private:

    //* text
    QString text_;

};

#endif
