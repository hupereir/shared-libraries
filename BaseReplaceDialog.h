#ifndef BaseReplaceDialog_h
#define BaseReplaceDialog_h

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

#include "BaseFindDialog.h"
#include "BaseReplaceWidget.h"

//* replace_text dialogs
class BaseReplaceDialog: public BaseFindDialog
{

    //* Qt meta object declaration
    Q_OBJECT

    public:

    //* constructor
    BaseReplaceDialog( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    //* destructor
    virtual ~BaseReplaceDialog( void ) = default;

    //*@name accessors
    //@{

    //* string to replace
    QString replaceText( void ) const
    { return static_cast<BaseReplaceWidget*>(&baseFindWidget())->replaceText(); }

    //* get selection
    virtual TextSelection selection( bool value ) const
    { return static_cast<BaseReplaceWidget*>(&baseFindWidget())->selection( value ); }

    //@}

    //*@name modifiers
    //@{

    //* string to replace
    void setReplaceText( const QString& text )
    { static_cast<BaseReplaceWidget*>(&baseFindWidget())->setReplaceText( text ); }

    //@}

    Q_SIGNALS:

    //* emmited when Replace is pressed
    void replace( TextSelection );

    //* emmited when Replace_in_window is pressed
    void replaceInWindow( TextSelection );

    //* emmited when Replace_in_selection is pressed
    void replaceInSelection( TextSelection );

    //* emmited when text replacement is changed
    void replaceTextChanged( QString );

};
#endif
