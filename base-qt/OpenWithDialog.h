#ifndef OpenWithDialog_h
#define OpenWithDialog_h

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
#include "File.h"
#include "OpenWithComboBox.h"

#include <QIcon>

//* QDialog used to commit selected files
class OpenWithDialog: public CustomDialog
{

    Q_OBJECT

    public:

    //* constructor
    OpenWithDialog( QWidget* );

    //*@name accessors
    //@{

    //* combo box
    OpenWithComboBox& comboBox( void ) const
    { return *comboBox_; }

    //@}

    //*@name modifiers
    //@{

    //* file
    void setFile( const File& value )
    {
        isLink_ = false;
        files_.clear();
        files_.append( value );
    }

    //* link
    void setLink( const File& value )
    {
        isLink_ = true;
        files_.clear();
        files_.append( value );
    }

    //* file
    void setFiles( const File::List value )
    {
        isLink_ = false;
        files_ = value;
    }

    //* icon
    void setIcon( const QIcon& value )
    { icon_ = value; }

    //* option name
    void setOptionName( const QString& value )
    {
        optionName_ = value;
        CustomDialog::setOptionName( QString( "%1_DIALOG" ).arg( value ) );
    }

    //* realize
    void realizeWidget( void );

    //@}

    protected Q_SLOTS:

    //* open file
    void _open( void );

    private:

    //* file
    File::List files_;

    //* icon
    QIcon icon_;

    //* true if link
    bool isLink_ = false;

    //* option name
    QString optionName_;

    //* combobox
    OpenWithComboBox* comboBox_ = nullptr;

};

#endif
