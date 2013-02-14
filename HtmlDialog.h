#ifndef HtmlDialog_h
#define HtmlDialog_h

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
* ANY WARRANTY;  without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.   See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA   02111-1307 USA
*
*
*******************************************************************************/

#include "BrowsedLineEditor.h"
#include "CustomDialog.h"
#include "CustomComboBox.h"
#include "LineEditor.h"
#include "File.h"

#include <QString>
#include <QList>
#include <QTabWidget>

//! new attachment popup dialog
class HtmlDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //! constructor
    HtmlDialog( QWidget* parent );

    //! destructor
    virtual ~HtmlDialog( void )
    {}

    //! set command manually
    void setCommand( QString command )
    { commandEditor_->setEditText( command ); }

    //! add commands to the combo-box list
    void addCommand( QString command )
    { commandEditor_->addItem( command ); }

    //! command
    QString command( void ) const
    { return commandEditor_->currentText(); }

    //! file
    void setFile( const File& file )
    { destinationEditor_->setFile( file ); }

    //! file
    File file( void ) const
    { return destinationEditor_->editor().text(); }

    // options
    void setOptionWidgets( QList<QWidget*> );

    protected slots:

    //! browse print command
    void _browseCommand( void );

    //! save print command
    void _saveCommands( void );

    private:

    //! print command
    CustomComboBox* commandEditor_;

    //! file
    BrowsedLineEditor *destinationEditor_;

    //! tab widget
    QTabWidget* tabWidget_;

};

#endif
