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

#include "TextEncodingWidget.h"
#include "TextEncodingWidget.moc"

#include "CustomComboBox.h"
#include "Debug.h"
#include "GridLayout.h"
#include "XmlOptions.h"

#include <QLabel>
#include <QLayout>
#include <QTextCodec>

//_____________________________________________________________________
TextEncodingWidget::TextEncodingWidget( QWidget* parent ):
    QWidget( parent ),
    Counter( "TextEncodingWidget" )
{

    Debug::Throw( "TextEncodingWidget::TextEncodingWidget.\n" );

    GridLayout* layout = new GridLayout();
    layout->setMargin(0);
    layout->setSpacing(5);
    layout->setMaxCount(2);
    layout->setColumnAlignment( 0, Qt::AlignRight|Qt::AlignVCenter );

    setLayout( layout );

    QLabel* label;
    layout->addWidget( label = new QLabel( tr( "Default text encoding:" ), this ) );
    CustomComboBox* defaultComboBox = new CustomComboBox( this );
    layout->addWidget( defaultComboBox );
    label->setBuddy( defaultComboBox );

    layout->addWidget( label = new QLabel( tr( "Current text encoding:" ) ) );
    CustomComboBox* currentComboBox = new CustomComboBox( this );
    layout->addWidget( currentComboBox );
    label->setBuddy( currentComboBox );

    // get list of available codecs and assign to comboboxes
    QList<QByteArray> codecs( QTextCodec::availableCodecs() );
    foreach( const QByteArray& value, codecs )
    {
        defaultComboBox->addItem( value );
        currentComboBox->addItem( value );
    }

    QString defaultCodec( "ISO-8859-1" );
    if( XmlOptions::get().contains( "TEXT_ENCODING" ) )
    {

        // get value
        QString value( XmlOptions::get().raw( "TEXT_ENCODING" ) );

        // check if in list
        if( codecs.indexOf( qPrintable( value ) ) >= 0 ) defaultCodec = value;
        else foreach( const QByteArray& arrayValue, codecs )
        {

            if( QTextCodec::codecForName( arrayValue )->aliases().indexOf( qPrintable( value ) ) )
            { defaultCodec = arrayValue; }

        }

    }

    // assign to comboboxes
    defaultComboBox->setCurrentIndex( defaultComboBox->findText( defaultCodec ) );
    currentComboBox->setCurrentIndex( currentComboBox->findText( defaultCodec ) );

    // connections
    connect( defaultComboBox, SIGNAL(currentTextChanged(const QString&)), this, SIGNAL(defaultEncodingChanged(const QString&)));
    connect( defaultComboBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(_updateDefaultEncoding(const QString&)));
    connect( currentComboBox, SIGNAL(currentTextChanged(const QString&)), this, SIGNAL(currentEncodingChanged(const QString&)));

}

//_____________________________________________________________________
void TextEncodingWidget::_updateDefaultEncoding( const QString& value )
{ XmlOptions::get().setRaw( "TEXT_ENCODING", value ); }
