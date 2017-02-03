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

#include <QCheckBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QLabel>

class ImageFileDialog: public QFileDialog
{

    Q_OBJECT

    public:

    //* creator
    ImageFileDialog( QWidget* );

    //* destructor
    virtual ~ImageFileDialog( void ) = default;

    //* select file
    void selectFile( const QString& filename )
    {
        emit _currentChanged( filename );
        QFileDialog::selectFile( filename );
    }

    private Q_SLOTS:

    //* update current
    void _currentChanged( const QString& path );

    //* display current image
    void _preview( void );

    protected:

    virtual void showEvent( QShowEvent* e )
    {
        QFileDialog::showEvent(e);
        _preview();
    }

    private Q_SLOTS:

    //* stores working directory when changed
    void _saveWorkingDirectory( QString directory );

    private:

    //* automatic preview checkbox
    QCheckBox* automaticPreview_ = nullptr;

    //* current file
    QString currentPath_;

    class Label: public QLabel, public Counter
    {
        public:

        //* constructor
        Label( QWidget* parent );

        protected:

        //* drag enter event
        virtual void dragEnterEvent( QDragEnterEvent* );

        //* drop event
        virtual void dropEvent( QDropEvent* );

    };

    //* preview label
    Label* preview_ = nullptr;

};

#endif
