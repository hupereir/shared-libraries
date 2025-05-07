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
#include "LineEditor.h"
#include "base_qt_export.h"

#include <QFileDialog>

#include <memory>

class LineEditor;

class BASE_QT_EXPORT BrowsedLineEditor: public LineEditor
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor;
    explicit BrowsedLineEditor( QWidget* = nullptr );

    //!@name modifiers
    //@{

    //! set target application
    /** it is used to browse application path and update, if not found */
    void setTargetApplication( const File &);

    //! native dialogs
    void setUseNativeFileDialog( bool value )
    { useNativeFileDialog_ = value; }

    //! open mode
    void setAcceptMode( const QFileDialog::AcceptMode mode )
    { acceptMode_ = mode; }

    //! file mode
    void setFileMode( QFileDialog::FileMode mode )
    { fileMode_ = mode; }

    //@}

    //! set lineEditor_file
    void setFile( const QString& file );

    protected:

    //! accept mode
    QFileDialog::AcceptMode _acceptMode() const
    { return acceptMode_; }

    //! file mode
    QFileDialog::FileMode _fileMode() const
    { return fileMode_; }

    private:

    //! opens a file dialog, stores result in line edit
    virtual void _browse();

    //! target
    void _findTargetApplication();

    //! use native dialogs
    bool useNativeFileDialog_ = true;

    //! accept mode
    QFileDialog::AcceptMode acceptMode_ = QFileDialog::AcceptOpen;

    //! File dialog mode
    QFileDialog::FileMode fileMode_ = QFileDialog::ExistingFile;

    //! target application
    File targetApplication_;

    //! update target button
    std::unique_ptr<QWidget> findTargetButton_;

};

#endif
