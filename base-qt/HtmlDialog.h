#ifndef HtmlDialog_h
#define HtmlDialog_h

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

#include "BrowsedLineEditor.h"
#include "CustomDialog.h"
#include "CustomComboBox.h"
#include "LineEditor.h"
#include "File.h"

#include <QString>
#include <QList>
#include <QTabWidget>

//* new attachment popup dialog
class HtmlDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //* constructor
    explicit HtmlDialog( QWidget* parent );

    //* set command manually
    void setCommand( QString command )
    { commandEditor_->setEditText( command ); }

    //* add commands to the combo-box list
    void addCommand( QString command )
    { commandEditor_->addItem( command ); }

    //* command
    QString command() const
    { return commandEditor_->currentText(); }

    //* file
    void setFile( const File& file )
    { destinationEditor_->setFile( file ); }

    //* file
    File file() const
    { return File( destinationEditor_->editor().text() ); }

    // options
    void setOptionWidgets( QList<QWidget*> );

    protected Q_SLOTS:

    //* browse print command
    void _browseCommand();

    //* save print command
    void _saveCommands();

    private:

    //* print command
    CustomComboBox* commandEditor_ = nullptr;

    //* file
    BrowsedLineEditor *destinationEditor_ = nullptr;

    //* tab widget
    QTabWidget* tabWidget_ = nullptr;

};

#endif
