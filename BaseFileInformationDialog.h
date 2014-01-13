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
* Any WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include "CustomDialog.h"
#include "ToolTipWidgetItem.h"
#include "TimeStamp.h"

#include <QFile>
#include <QList>
#include <QLabel>
#include <QLayout>

class AnimatedTabWidget;
class FilePermissionsWidget;
class GridLayout;

//! file informations
class BaseFileInformationDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //! constructor
    BaseFileInformationDialog( QWidget* );

    //! destructor
    virtual ~BaseFileInformationDialog( void )
    {}

    //!@name accessors
    //@{

    //! tab widget
    AnimatedTabWidget& tabWidget( void ) const
    { return *tabWidget_; }

    //! main widget
    QWidget& mainPage( void ) const
    { return *mainPage_; }

    //! main layout
    QVBoxLayout& pageLayout( void ) const
    { return *pageLayout_; }

    //@}

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
    void setPermissions( QFile::Permissions );

    //! user
    void setUser( const QString& );

    //! group
    void setGroup( const QString& );

    //! add a row
    int addRow( const QString&, const QString& = QString(), ToolTipWidgetItem::ItemFlags = ToolTipWidgetItem::None );

    //! set custom value
    void setCustomKey( int, const QString& );

    //! set custom value
    void setCustomValue( int, const QString& );

    //@}

    private:

    //! tab widget
    AnimatedTabWidget* tabWidget_;

    //! main tab widget
    QWidget* mainPage_;

    //! main tab layout
    QVBoxLayout* pageLayout_;

    //! grid layout
    GridLayout* gridLayout_;

    //! icon
    QLabel* iconLabel_;

    //! items
    ToolTipWidgetItem* fileItem_;
    ToolTipWidgetItem* pathItem_;
    ToolTipWidgetItem* typeItem_;
    ToolTipWidgetItem* sizeItem_;
    ToolTipWidgetItem* createdItem_;
    ToolTipWidgetItem* accessedItem_;
    ToolTipWidgetItem* modifiedItem_;
    ToolTipWidgetItem* userItem_;
    ToolTipWidgetItem* groupItem_;

    //! permissions
    FilePermissionsWidget* permissionsWidget_;

    //! extra rows
    QList<ToolTipWidgetItem*> extraItems_;

};

#endif
