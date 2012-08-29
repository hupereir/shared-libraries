#ifndef _FontEditor_h_
#define _FontEditor_h_

// $Id$

/*******************************************************************************
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

#include "Counter.h"
#include "Debug.h"

#include <QtGui/QFont>
#include <QtGui/QFontDialog>
#include <QtGui/QLabel>

//! readonly font name display and font dialog
class FontEditor: public QWidget, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    FontEditor( QWidget *parent );

    //! display font
    virtual void setFont( const QFont& font )
    {
        font_ = font;
        _updateLabel();
    }

    //! display font
    virtual void setFont( const QString& font )
    {
        font_.fromString( font );
        _updateLabel();
    }

    //! retrieve font
    virtual const QFont& font( void ) const
    { return font_; }

    protected slots:

    //! select font from dialog
    virtual void _selectFont( void )
    {
        bool ok( false );
        QFont font = QFontDialog::getFont( &ok, this->font(), this );
        if( ok ) setFont( font );
        return;
    }

    protected:

    //! update label
    virtual void _updateLabel( void )
    {
        QString buffer;
        QTextStream( &buffer ) << font().family() << ", " << font().pointSize() << "pt";
        label_->setText( buffer );
    }

    private:

    //! font name
    QLabel* label_;

    //! selected font
    QFont font_;

};
#endif
