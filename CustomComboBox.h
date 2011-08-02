#ifndef CustomComboBox_h
#define CustomComboBox_h

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
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

/*!
\file CustomComboBox.h
\brief Customized QCombobox to handle case sensitive auto completion
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include <cassert>
#include <QComboBox>
#include <QKeyEvent>

#include "AnimatedLineEditor.h"
#include "Counter.h"
#include "Debug.h"

//! local line editor to handle autocompletion
class ComboLineEdit: public AnimatedLineEditor
{
    
    //! Qt meta object declaration
    Q_OBJECT
        
    public:
        
    //! constructor
    ComboLineEdit( QWidget* parent ):
    AnimatedLineEditor( parent )
    { Debug::Throw( "ComboLineEdit::ComboLineEdit.\n" ); }
    
    public slots:
    
    //! start animation
    void startAnimation( void );
        
};
    
//! Customized QCombobox to handle case sensitive auto completion
class CustomComboBox: public QComboBox, public Counter
{
    
    //! Qt meta object declaration
    Q_OBJECT
        
        public:
        
        //! constructor
        CustomComboBox( QWidget* parent );
    
    //! editability
    void setEditable( bool value );
    
    //! auto completion
    void setAutoCompletion( bool value, Qt::CaseSensitivity  = Qt::CaseInsensitive);
    
    private:
    
    //! local line editor
    ComboLineEdit* editor_;
    
};

#endif
