#ifndef QuestionDialog_h
#define QuestionDialog_h


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

#include "DetailsDialog.h"
#include "Counter.h"

#include <QString>
#include <QLabel>

//* simplified question dialog
class QuestionDialog: public DetailsDialog
{

    public:

    //* constructor
    explicit QuestionDialog( QWidget*, const QString& = QString() );

    //*@name modifiers
    //@{

    //* set text
    void setText( const QString& );

    //@}

    protected:

    //* label
    QLabel& _label() const
    { return *label_; }

    private:

    //* main label
    QLabel *label_ = nullptr;

};

#endif
