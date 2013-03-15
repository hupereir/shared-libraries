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

#include <QFile>
#include <QList>
#include <QLabel>
#include <QLayout>

class AnimatedTabWidget;
class Item;
class FilePermissionsWidget;
class GridLayout;

//! file informations
class BaseFileInformationDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //! flags
    enum ItemFlag
    {
        None = 0,
        Elide = 1<<0,
        Bold = 1<<1,
        Selectable = 1<<2,
        All = Elide|Bold|Selectable
    };

    Q_DECLARE_FLAGS( ItemFlags, ItemFlag );

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
    int addRow( const QString&, const QString& = QString(), ItemFlags = None );

    //! set custom value
    void setCustomKey( int, const QString& );

    //! set custom value
    void setCustomValue( int, const QString& );

    //@}

    //! item
    class Item: public QObject, public Counter
    {

        public:

        //! constructor
        Item( QWidget*, GridLayout*, ItemFlags = None );

        //! destructor
        virtual ~Item( void )
        {}

        //!@name modifiers
        //@{

        //! set key
        void setKey( const QString& );

        //! set text
        void setValue( const QString& );

        //@}

        //!@name visibility
        //@{

        //! set visible
        void setVisible( bool value );

        //! show
        void show( void );

        //! hide
        void hide( void );

        //@}

        private:

        ItemFlags flags_;
        QLabel* key_;
        QLabel* value_;

    };

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
    Item* fileItem_;
    Item* pathItem_;
    Item* typeItem_;
    Item* sizeItem_;
    Item* createdItem_;
    Item* accessedItem_;
    Item* modifiedItem_;
    Item* userItem_;
    Item* groupItem_;

    //! permissions
    FilePermissionsWidget* permissionsWidget_;

    //! extra rows
    QList<Item*> extraItems_;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( BaseFileInformationDialog::ItemFlags )

#endif
