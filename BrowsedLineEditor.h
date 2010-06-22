#ifndef _BrowsedLineEditor_h_
#define _BrowsedLineEditor_h_

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
\file BrowsedLineEditor.h
\brief combined QLineEdit + associated browse button
line edit is also customized to have faster popup menu
result of the file dialog associated to the button is stored into the
LineEditor object
\author Hugo Pereira
\version $Revision$
\date $Date$
*/


#include <cassert>
#include <QFileDialog>
#include <QWidget>


#include "Counter.h"

class AnimatedLineEditor;

/*!
\class BrowsedLineEditor
\brief combined QLineEdit + associated browse button
line edit is also customized to have faster popup menu
result of the file dialog associated to the button is stored into the
LineEditor object
*/
class BrowsedLineEditor: public QWidget, public Counter
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    typedef AnimatedLineEditor Editor;

    //! constructor;
    BrowsedLineEditor( QWidget* parent );

    //! retrieve line editor
    virtual Editor& editor( void ) const
    {
        assert( lineEditor_ );
        return *lineEditor_;
    }

    //! open mode
    virtual void setAcceptMode( const QFileDialog::AcceptMode mode )
    { acceptMode_ = mode; }

    //! file mode
    virtual void setFileMode( const QFileDialog::FileMode& mode )
    { fileMode_ = mode; }

    public slots:

    //! set lineEditor_file
    virtual void setFile( const QString& file );

    protected:

    //! accept mode
    virtual const QFileDialog::AcceptMode& _acceptMode( void ) const
    { return acceptMode_; }

    //! file mode
    virtual const QFileDialog::FileMode& _fileMode( void ) const
    { return fileMode_; }

    protected slots:

    //! opens a file dialog, stores result in line edit
    virtual void _browse( void );

    private:

    //! line editor widget
    Editor *lineEditor_;

    //! accept mode
    QFileDialog::AcceptMode acceptMode_;

    //! File dialog mode
    QFileDialog::FileMode fileMode_;

};

#endif
