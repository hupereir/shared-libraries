#ifndef OptionListBox_h
#define OptionListBox_h

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
#include "OptionWidget.h"

#include <QAction>
#include <QCheckBox>
#include <QFileDialog>
#include <QPushButton>

#include <cmath>

class OptionModel;
class TreeView;

//* ListBox for Special options, and buttons to add/remove values
class OptionListBox: public QWidget, public OptionWidget
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    OptionListBox( QWidget*, const QString& );

    //* list
    TreeView& list( void ) const
    { return *list_; }

    //* read value from option
    void read( const Options& options );

    //* write value to option
    void write( Options& options ) const;

    //* browsability
    void setBrowsable( bool value )
    { browsable_ = value; }

    //* set file dialog mode
    void setFileMode( const QFileDialog::FileMode& mode )
    { fileMode_ = mode; }

    Q_SIGNALS:

    //* modified
    void modified( void );

    protected Q_SLOTS:

    //* update buttons
    virtual void _updateButtons( void );

    //* add a value
    virtual void _add( void );

    //* add a value
    virtual void _edit( void );

    //* remove a value
    virtual void _remove();

    //* set value as default
    virtual void _setDefault();

    protected:

    //* model
    OptionModel& _model( void ) const
    { return *model_; }

    //* set model
    void _setModel( OptionModel* );

    private:

    //* if true, use browsable line editor for Add
    bool browsable_ = false;

    //* browsable dialog mode
    QFileDialog::FileMode fileMode_ = QFileDialog::AnyFile;

    //* model
    OptionModel* model_ = nullptr;

    //* value list
    TreeView* list_ = nullptr;

    //*@name buttons
    //@{

    //* edit button
    QPushButton* edit_ = nullptr;

    //* remove button
    QPushButton* remove_ = nullptr;

    //* default button
    QPushButton* default_ = nullptr;

    //@}

    //*@name actions
    //@{

    //* remove action
    QAction* editAction_ = nullptr;

    //* remove action
    QAction* removeAction_ = nullptr;

    //* remove action
    QAction* defaultAction_ = nullptr;

    //@}

};
#endif
