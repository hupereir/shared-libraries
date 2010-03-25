#ifndef ElidedLabel_h
#define ElidedLabel_h

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

/*!
   \file ElidedLabel.h
   \brief handles text elision in QLabel
   \author Hugo Pereira
   \version $Revision$
   \date $Date$
*/

#include "Counter.h"
#include <QtCore/QString>
#include <QtGui/QLabel>
#include <QtGui/QResizeEvent>

class ElidedLabel: public QLabel, public Counter
{

    public:

    //! constructor
    ElidedLabel( QWidget* = 0 );

    //! constructor
    ElidedLabel( const QString&, QWidget* = 0);

    //! destructor
    virtual ~ElidedLabel( void )
    {}

    //! elision mode
    Qt::TextElideMode elideMode( void ) const
    { return elideMode_; }

    //! elision mode
    void setElideMode( Qt::TextElideMode mode )
    {
        if( mode == elideMode_ ) return;
        else {
            elideMode_ = mode;
            updateElidedText();
        }
    }

    //! text
    QString text( void )
    { return fullText_; }

    //! set text
    void setText( const QString& );

    //! size hing
    virtual QSize minimumSizeHint( void ) const;
    virtual QSize sizeHint( void ) const;

    protected:

    //! resize event
    virtual void resizeEvent( QResizeEvent* );

    //! update text based on object size
    void updateElidedText( void );

    private:

    //! elision mode
    Qt::TextElideMode elideMode_;

    //! full text
    QString fullText_;

};

#endif
