#ifndef ElidedLabel_h
#define ElidedLabel_h

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
#include <QString>
#include <QLabel>
#include <QResizeEvent>

//* label with automatically elided text
class ElidedLabel: public QLabel, public Counter
{

    Q_OBJECT

    public:

    //* constructor
    ElidedLabel( QWidget* = nullptr );

    //* constructor
    ElidedLabel( const QString&, QWidget* = nullptr);

    //*@name accessors
    //@{

    //* elision mode
    Qt::TextElideMode elideMode( void ) const
    { return elideMode_; }

    //* text
    QString text( void )
    { return fullText_; }

    //@}

    //*@name modifiers
    //@{

    //* elision mode
    void setElideMode( Qt::TextElideMode mode )
    {
        if( mode == elideMode_ ) return;
        else {
            elideMode_ = mode;
            updateElidedText();
        }
    }

    //* set text
    void setText( const QString& );

    //@}

    //* minimum size hint
    virtual QSize minimumSizeHint( void ) const;

    //* size hint
    virtual QSize sizeHint( void ) const;

    protected:

    //* mouse release
    void mouseReleaseEvent(QMouseEvent *);

    //* resize event
    virtual void resizeEvent( QResizeEvent* );

    //* update text based on object size
    void updateElidedText( void );

    private:

    QRegExp hRefRegExp_;

    //* elision mode
    Qt::TextElideMode elideMode_ = Qt::ElideLeft;

    //* full text
    QString fullText_;

};

#endif
