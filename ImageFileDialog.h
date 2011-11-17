// $Id$
#ifndef _ImageFileDialog_h_
#define _ImageFileDialog_h_

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
\file ImageFileDialog.h
\brief file dialog with preview
\author Hugo Pereira
\version $Revision$
\date $Date$
*/

#include "File.h"

#include <QtGui/QCheckBox>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QFileDialog>
#include <QLabel>

#include <cassert>
class ImageFileDialog: public QFileDialog
{

    //! Qt meta object declaration
    Q_OBJECT

        public:

        //! creator
        ImageFileDialog( QWidget* parent );

    //! select file
    void selectFile( const QString& filename )
    {
        emit _currentChanged( filename );
        QFileDialog::selectFile( filename );
    }

    private slots:

    //! update current
    void _currentChanged( const QString& path );

    //! display current image
    void _preview( void );

    protected:

    virtual void showEvent( QShowEvent* e )
    {
        QFileDialog::showEvent( e );
        _preview();
    }

    private slots:

    //! stores working directory when changed
    void _saveWorkingDirectory( const QString& directory );

    private:

    //! automatic preview checkbox
    QCheckBox& _automaticPreviewCheckbox( void ) const
    {
        assert( automaticPreview_ );
        return *automaticPreview_;
    }

    //! automatic preview checkbox
    QCheckBox* automaticPreview_;

    //! current file
    QString currentPath_;

    class Label: public QLabel, public Counter
    {
        public:

        //! constructor
        Label( QWidget* parent );

        protected:

        //! drag enter event
        virtual void dragEnterEvent( QDragEnterEvent* );

        //! drop event
        virtual void dropEvent( QDropEvent* );

    };

    //! preview label
    Label* preview_;

};

#endif
