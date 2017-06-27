#ifndef FileDialog_h
#define FileDialog_h

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
#include <QObject>
#include <QString>
#include <QStringList>

//* customized file selection dialog, to store last selected directory
class FileDialog: public QObject, private Base::Counter<FileDialog>
{

    public:

    //* creator
    explicit FileDialog( QWidget* parent );

    //* open mode
    FileDialog& setAcceptMode( const QFileDialog::AcceptMode mode )
    {
        acceptMode_ = mode;
        return *this;
    }

    //* file mode
    FileDialog& setFileMode( const QFileDialog::FileMode mode )
    {
        fileMode_ = mode;
        return *this;
    }

    //* caption
    FileDialog& setCaption( const QString& caption )
    {
        caption_ = caption;
        return *this;
    }

    //* default file/directory
    /** it is used as a starting selection when file dialog is opened */
    FileDialog& selectFile( const File& file );

    //* filter
    FileDialog& setFilter( const QString& filter )
    {
        filter_ = filter;
        return *this;
    }

    //* options
    FileDialog& setOptions( const QFileDialog::Options& options )
    {
        options_ = options;
        return *this;
    }

    //* get file
    File getFile();

    //* get list of files
    File::List getFiles();

    //* reference to statically scoped working directory
    const File& workingDirectory() const
    { return _workingDirectory(); }

    protected:

    //* reference to statically scoped working directory
    static File& _workingDirectory();

    //* accept mode
    QFileDialog::AcceptMode _acceptMode() const
    { return acceptMode_; }

    //* file mode
    QFileDialog::FileMode _fileMode() const
    { return fileMode_; }

    //* caption
    const QString& _caption() const
    { return caption_; }

    //* selected file
    /** it is used as a starting selection when file dialog is opened */
    const File& _selectedFile() const
    { return selectedFile_; }

    //* filter
    const QString& _filter() const
    { return filter_; }

    //* options
    QFileDialog::Options _options() const
    { return options_; }

    private:

    //* accept mode
    QFileDialog::AcceptMode acceptMode_ = QFileDialog::AcceptOpen;

    //* file mode
    QFileDialog::FileMode fileMode_ = QFileDialog::ExistingFile;

    //* caption
    QString caption_;

    //* default file
    File selectedFile_;

    //* filter
    QString filter_;

    //* options
    QFileDialog::Options options_ = 0;

    //* needed to allow ImageFileDialog to acces working directory
    friend class ImageFileDialog;

};

#endif
