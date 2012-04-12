#ifndef DictionarySelectionButton_h
#define DictionarySelectionButton_h

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

#include "OptionWidget.h"
#include "XmlOptions.h"

#include <QtGui/QToolButton>

namespace SPELLCHECK
{

    class DictionarySelectionButton: public QToolButton, public OptionWidget
    {

        Q_OBJECT

        public:

        //! constructor
        DictionarySelectionButton( QWidget* );

        //! destructor
        virtual ~DictionarySelectionButton( void )
        {}

        //! retrieve value
        QString value( void ) const
        { return value_; }

        //! read value from option
        virtual void read( void )
        { value_ = XmlOptions::get().raw( optionName() ); }

        //! write value to option
        virtual void write( void ) const
        { XmlOptions::get().set( optionName(), value_ ); }

        signals:

        //! emmitted when selection is modified
        void modified( void );

        protected slots:

        //! edit dictionaries
        void _edit( void );

        private:

        //! disabled dictionaries
        QString value_;

    };

}

#endif
