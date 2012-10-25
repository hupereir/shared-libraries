#ifndef BaseFileInformationDialog_h
#define BaseFileInformationDialog_h
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

#include "CustomDialog.h"
#include "TimeStamp.h"

#include <QtGui/QLabel>

class AnimatedTabWidget;
class BaseInformationDialogItem;
class FilePermissionsWidget;

//! file informations
class BaseFileInformationDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //! constructor
    BaseFileInformationDialog( QWidget* );

    //! destructor
    virtual ~FileInformationDialog( void )
    {}

    //!@name modifiers
    //@{

    //! icon
    void setIcon( const QIcon& );

    //! file
    void setFile( const QString& );

    //! path
    void setPath( const QString& );

    //! type
    void setType( const QString& );

    //! size
    void setSize( qint64 );

    //! created
    void setCreated( TimeStamp );

    //! accessed
    void setAccessed( TimeStamp );

    //! modified
    void setModified( TimeStamp );

    //! permissions
    void setPermissions( const QString& );

    //! user
    void setUser( const QString& );

    //! group
    void setGroup( const QString& );

    //@}

    private:

    //! icon
    QLabel* iconLabel_;

    //! items
    BaseInformationDialogItem* fileItem_;
    BaseInformationDialogItem* pathItem_;
    BaseInformationDialogItem* typeItem_;
    BaseInformationDialogItem* sizeItem_;
    BaseInformationDialogItem* createdItem_;
    BaseInformationDialogItem* accessedItem_;
    BaseInformationDialogItem* modifiedItem_;
    BaseInformationDialogItem* userItem_;
    BaseInformationDialogItem* groupItem_;

    //! permissions
    FilePermissionsWidget* permissionsWidget_;

}

