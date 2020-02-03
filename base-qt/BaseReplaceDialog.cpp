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

#include "BaseReplaceDialog.h"
#include "Debug.h"

//________________________________________________________________________
BaseReplaceDialog::BaseReplaceDialog( QWidget* parent, Qt::WindowFlags flags ):
    BaseFindDialog( parent, flags )
{
    Debug::Throw( "BaseReplaceDialog::BaseReplaceDialog.\n" );

    // set dialog title
    setWindowTitle( tr( "Replace" ) );
    setOptionName( "REPLACE_DIALOG" );

    // set base find widget
    auto replaceWidget = new BaseReplaceWidget( this, false );
    setBaseFindWidget( replaceWidget );

    // connections
    connect( replaceWidget, &BaseReplaceWidget::replace, this, &BaseReplaceDialog::replace );
    connect( replaceWidget, &BaseReplaceWidget::replaceInWindow, this, &BaseReplaceDialog::replaceInWindow );
    connect( replaceWidget, &BaseReplaceWidget::replaceInSelection, this, &BaseReplaceDialog::replaceInSelection );
    connect( replaceWidget, &BaseReplaceWidget::replaceTextChanged, this, &BaseReplaceDialog::replaceTextChanged );

}
