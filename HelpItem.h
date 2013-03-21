#ifndef HelpItem_h
#define HelpItem_h

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

#include "Counter.h"
#include "Debug.h"

#include <QDomElement>
#include <QDomDocument>
#include <QList>

//! Help system namespace
namespace BASE
{


    //!@name xml tags
    //@{
    const QString XML_HELP = "help";
    const QString XML_ITEM = "item";
    const QString XML_LABEL = "label";
    const QString XML_TEXT = "text";
    //@}

    //! reference manual single entry
    class HelpItem : public Counter
    {

        public:

        //! constructor
        HelpItem( const QString& label = QString(), const QString& text = QString() ):
            Counter( "HelpItem" ),
            label_( label ),
            text_( text )
        { Debug::Throw( "HelpItem::HelpItem.\n" ); }

        //! constructor from Dom
        HelpItem( const QDomElement& dom );

        //! write to dom elelement
        QDomElement domElement( QDomDocument& parent ) const;

        //! equal-to operator
        bool operator == (const HelpItem& item ) const
        { return label() == item.label() && text() == item.text(); }

        //! equal-to operator
        bool operator < (const HelpItem& item ) const
        { return label() < item.label() || (label() == item.label() && text() < item.text() ); }

        //! item label
        void setLabel( const QString& label )
        { label_ = label; }

        //! label
        const QString& label( void ) const
        { return label_; }

        //! item text
        void setText( const QString& text )
        { text_ = text; }

        //! item text
        const QString& text( void ) const
        { return text_; }

        //! shortcut to help item list
        typedef QList< HelpItem > List;

        private:

        //! help label
        QString label_;

        //! help text
        QString text_;

    };

};

#endif
