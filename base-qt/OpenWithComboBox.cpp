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

#include "OpenWithComboBox.h"

#include "File.h"
#include "FileDialog.h"

//___________________________________________________________
OpenWithComboBox::OpenWithComboBox( QWidget* parent ):
    QComboBox( parent ),
    Counter( QStringLiteral("OpenWithComboBox") )
{

    setEditable( false );
    QComboBox::addItem( tr( "Other..." ) );
    connect( this, QOverload<int>::of( &OpenWithComboBox::activated ), this, &OpenWithComboBox::_indexActivated );

}

//___________________________________________________________
File OpenWithComboBox::command() const
{
    const int index = currentIndex();
    if( index == count()-1 ) return File();
    else return File( itemData( index ).toString() );
}

//___________________________________________________________
void OpenWithComboBox::addItem( const File& file )
{
    QComboBox::insertItem( count()-1, file.localName(), file.get() );
    setCurrentIndex( count()-2 );
}

//___________________________________________________________
void OpenWithComboBox::addItem( const QString& item, const File& file)
{
    QComboBox::insertItem( count()-1, item, file.get() );
    setCurrentIndex( count()-2 );
}

//___________________________________________________________
void OpenWithComboBox::_indexActivated( int index )
{
    // check if index is last
    if( index == count()-1 )
    {
        const File filename( FileDialog(this).getFile() );
        if( !filename.isEmpty() )
        {
            addItem( filename );
            newItems_.append( filename );
        }
    }
}
