#ifndef BrowsedLineEditor_h
#define BrowsedLineEditor_h

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
#include "File.h"

#include <QFileDialog>
#include <QWidget>

class LineEditor;

class BrowsedLineEditor: public QWidget, private Base::Counter<BrowsedLineEditor>
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    using Editor = LineEditor;

    //* constructor;
    BrowsedLineEditor( QWidget* parent );

    //*@name accessors
    //@{

    //* retrieve line editor
    virtual Editor& editor( void ) const
    { return *lineEditor_; }

    //*}

    //*@name modifiers
    //@{


    //* set target application
    /** it is used to browse application path and update, if not found */
    void setTargetApplication( File );

    //* open mode
    virtual void setAcceptMode( const QFileDialog::AcceptMode mode )
    { acceptMode_ = mode; }

    //* file mode
    virtual void setFileMode( const QFileDialog::FileMode& mode )
    { fileMode_ = mode; }

    //@}

    public Q_SLOTS:

    //* set lineEditor_file
    virtual void setFile( const QString& file );

    protected:

    //* accept mode
    virtual QFileDialog::AcceptMode _acceptMode( void ) const
    { return acceptMode_; }

    //* file mode
    virtual QFileDialog::FileMode _fileMode( void ) const
    { return fileMode_; }

    protected Q_SLOTS:

    //* opens a file dialog, stores result in line edit
    virtual void _browse( void );

    //* target
    void _findTargetApplication( void );

    private:

    //* line editor widget
    Editor *lineEditor_ = nullptr;

    //* accept mode
    QFileDialog::AcceptMode acceptMode_ = QFileDialog::AcceptOpen;

    //* File dialog mode
    QFileDialog::FileMode fileMode_ = QFileDialog::ExistingFile;

    //* target application
    File targetApplication_;

    //* update target button
    QWidget* findTargetButton_ = nullptr;

};

#endif
