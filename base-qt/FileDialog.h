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
    virtual FileDialog& setAcceptMode( const QFileDialog::AcceptMode mode )
    {
        acceptMode_ = mode;
        return *this;
    }

    //* file mode
    virtual FileDialog& setFileMode( const QFileDialog::FileMode mode )
    {
        fileMode_ = mode;
        return *this;
    }

    //* caption
    virtual FileDialog& setCaption( const QString& caption )
    {
        caption_ = caption;
        return *this;
    }

    //* default file/directory
    /** it is used as a starting selection when file dialog is opened */
    virtual FileDialog& selectFile( const File& file );

    //* filter
    virtual FileDialog& setFilter( const QString& filter )
    {
        filter_ = filter;
        return *this;
    }

    //* options
    virtual FileDialog& setOptions( const QFileDialog::Options& options )
    {
        options_ = options;
        return *this;
    }

    //* get file
    virtual File getFile( void );

    //* get list of files
    virtual File::List getFiles( void );

    //* reference to statically scoped working directory
    virtual const File& workingDirectory() const
    { return _workingDirectory(); }

    protected:

    //* reference to statically scoped working directory
    static File& _workingDirectory();

    //* accept mode
    virtual QFileDialog::AcceptMode _acceptMode( void ) const
    { return acceptMode_; }

    //* file mode
    virtual QFileDialog::FileMode _fileMode( void ) const
    { return fileMode_; }

    //* caption
    virtual const QString& _caption( void ) const
    { return caption_; }

    //* selected file
    /** it is used as a starting selection when file dialog is opened */
    virtual const File& _selectedFile( void ) const
    { return selectedFile_; }

    //* filter
    virtual const QString& _filter( void ) const
    { return filter_; }

    //* options
    virtual QFileDialog::Options _options( void ) const
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
