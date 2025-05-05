#ifndef BaseReplaceWidget_h
#define BaseReplaceWidget_h

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

#include "BaseFindWidget.h"
#include "base_qt_export.h"

#include <QMenu>

//! find dialog for text editor widgets
class BASE_QT_EXPORT BaseReplaceWidget: public BaseFindWidget
{

    //! Qt meta object declaration
    Q_OBJECT

    public:

    //! constructor
    explicit BaseReplaceWidget( QWidget* = nullptr, bool compact = true );

    //!@name accessors
    //@{

    //! string to replace
    QString replaceText() const
    { return replaceEditor_->currentText(); }

    //! retrieve text selection from dialog
    TextSelection selection( bool ) const override;

    //@}

    //!@name modifiers
    //@{

    //! enable replace in selection
    void enableReplaceInSelection( bool value )
    { replaceInSelectionAction_->setEnabled( value ); }

    //! string to replace
    void setReplaceText( const QString& text )
    { replaceEditor_->setEditText( text ); }

    //! synchronize searched strings and ComboBox
    void synchronize() override;

    //@}

    Q_SIGNALS:

    //! emitted when replace all menu is about to show
    void menuAboutToShow();

    //! emitted when Replace is pressed
    void replace( TextSelection );

    //! emitted when Replace_in_window is pressed
    void replaceInWindow( TextSelection );

    //! emitted when Replace_in_selection is pressed
    void replaceInSelection( TextSelection );

    //! emitted when text replacement is changed
    void replaceTextChanged( QString text_replace );

    protected:

    //! update combo box with current text
    void _updateReplaceComboBox()
    { _addReplacedString( replaceEditor_->currentText() ); }

    //! create Selection object when replace button is pressed
    void _replace()
    { emit replace( selection( false ) ); }

    //! create Selection object when replace button is pressed
    void _replaceInWindow();

    //! create Selection object when replace button is pressed
    void _replaceInSelection();

    //! replace editor
    ComboBox& _replaceEditor() const
    { return *replaceEditor_; }

    //! replace all menu
    QMenu& _replaceAllMenu() const
    { return *replaceAllMenu_; }

    //! add string to both combo box and static set
    void _addReplacedString( const QString& );

    private:

    //! location layout
    QBoxLayout* locationLayout_ = nullptr;

    //! line editor for text to replace
    ComboBox* replaceEditor_ = nullptr;

    //! replace all menu
    QMenu* replaceAllMenu_ = nullptr;

    //! replace in selection action
    QAction* replaceInSelectionAction_ = nullptr;

    //! replaced strings
    static QOrderedSet<QString>& _replacedStrings();

};
#endif
