#ifndef ImageFileDialog_h
#define ImageFileDialog_h

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
#include "base_qt_export.h"

#include <QCheckBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>

class BASE_QT_EXPORT ImageFileDialog: public QFileDialog
{

    Q_OBJECT

    public:

    //* creator
    explicit ImageFileDialog( QWidget* );

    //* select file
    void selectFile( const File& filename )
    {
        _currentChanged( filename );
        QFileDialog::selectFile( filename );
    }

    //* stores working directory when changed
    void saveWorkingDirectory( const QString &directory );

    protected:

    void showEvent( QShowEvent* e ) override
    {
        QFileDialog::showEvent(e);
        _preview();
    }

    private:

    //* update current
    void _currentChanged( const QString& path );

    //* display current image
    void _preview();

    //* automatic preview checkbox
    QCheckBox* automaticPreview_ = nullptr;

    //* current file
    File currentPath_;

    class BASE_QT_EXPORT Label: public QLabel, private Base::Counter<Label>
    {
        public:

        //* constructor
        explicit Label( QWidget* parent );

        protected:

        //* drag enter event
        void dragEnterEvent( QDragEnterEvent* ) override;

        //* drop event
        void dropEvent( QDropEvent* ) override;

    };

    //* preview label
    Label* preview_ = nullptr;

};

#endif
