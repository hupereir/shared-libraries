#ifndef Key_h
#define Key_h

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
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include <QTextStream>
#include <QSet>
#include <algorithm>

//! base namespace
namespace BASE
{

    // temporary forward declaration
    class Key;

    //! \brief is used to sort pointer to keys according to their unique ID
    //! \brief generic key object to handle object associations
    /*!
    Generic Key object to handle objects associations. Each Key
    has a unique integer for identification and sorting, and a set
    of associated Keys. Keys are reflexively associated two by two.
    When a key is deleted, it get removed from the set of all its associated
    keys first, to avoid keeping pointers to deleted objects.
    Either the full set or a typename driven set of associated keys can
    be retrieved, provided the template typename inherits from Key.
    Keys provide easy handling of object reflexives associations
    without explicit care of object deletions. Following diagram shows the current object
    association scheme.
    <p><img src="associations.gif" border="0" >

    */
    class Key
    {

        public:

        //! constructor
        Key( void ):
            key_( _counter()++ )
        {}

        //! \brief copy constructor
        /*!
        copy constructor.
        The set of associated keys is copied as is.
        The new key gets a new unique id. It gets associated to all objects
        in the list
        */
        Key( const Key& key ):
            key_( _counter()++ ),
            associatedKeys_( key.associatedKeys_ )
        {
            foreach( Key* associate, associatedKeys_ )
            { associate->_associate( this ); }
        }

        //! destructor
        virtual ~Key( void )
        { clearAssociations(); }

        //! equal to operator
        bool operator == (const Key& key ) const
        { return this->key() == key.key(); }

        //! lower than operator
        bool operator < (const Key& key ) const
        { return this->key() < key.key(); }

        //! shortcut for key unique id
        typedef unsigned long int Type;

        //! retrieve key
        virtual const Type& key( void ) const
        { return key_; }

        //! shortcut for set of Key
        typedef QSet< Key* > Set;

        //! retrieve all associated keys
        const Set& getAssociated( void ) const
        { return associatedKeys_; }

        //! clear associations for this key
        void clearAssociations( void )
        {
            foreach( Key* key, associatedKeys_ ) key->_disassociate( this );
            associatedKeys_.clear();
        }

        //! clear associations of a given type for this key
        template<typename T> inline void clearAssociations( void );

        //! associated two Keys
        static void associate( Key* first, Key* second )
        {
            Q_ASSERT( first->key() != second->key() );
            first->_associate( second );
            second->_associate( first );
        }

        //! associated two Keys
        static void associate( Key& first, Key& second )
        {
            Q_ASSERT( first.key() != second.key() );
            first._associate( &second );
            second._associate( &first );
        }

        //! used to find keys of matching id
        class SameKeyFTor
        {

            public:

            //! constructor
            SameKeyFTor( const Type& key ):
                key_( key )
            {}

            //! predicate
            bool operator() (const Key* key ) const
            { return key->key() == key_; }

            private:

            //! predicted key
            Type key_;

        };

        //! used to find keys of matching id
        class IsAssociatedFTor
        {

            public:

            //! constructor
            IsAssociatedFTor( const Key* key ):
                key_( key )
            {}

            //! predicate
            bool operator() (const Key* key ) const
            { return key_->isAssociated( key ); }

            private:

            //! predicted key
            const Key* key_;

        };


        //! return true if keys are associated
        bool isAssociated( const Key* key ) const
        {
            return std::find_if(
                associatedKeys_.begin(),
                associatedKeys_.end(),
                SameKeyFTor( key->key() ) ) != associatedKeys_.end();
        }

        //! disassociate two Keys
        static void disassociate( Key* first, Key* second )
        {
            first->_disassociate( second );
            second->_disassociate( first );
        }

        //! disassociated two Keys
        static void disassociate( Key& first, Key& second )
        {
            first._disassociate( &second );
            second._disassociate( &first );
        }

        private:

        //! add a key to associates
        void _associate( Key* key )
        { associatedKeys_.insert( key ); }

        //! remove a key from associates
        void _disassociate( Key* key )
        { associatedKeys_.remove( key ); }

        //! unique id
        Type key_;

        //! associated keys
        Set associatedKeys_;

        //! unique id counter
        static Type& _counter( void );

        //! to dump Key and associations
        friend QTextStream &operator << (QTextStream &out,const Key &key)
        {

            //! dump this key uid
            out << "key=" << key.key();

            // dump associated key uid
            if( key.associatedKeys_.size() )
            {
                out << " associations:";

                foreach( Key* associate, key.associatedKeys_ )
                { out << " " << associate->key(); }

            }

            out << endl;
            return out;
        }

    };

    /*! \brief
    templatized sorted set of casted keys.
    Is constructed from a pointer to a Key;
    Contains all objects of type T associated to the Key.
    */
    template<typename T>
    class KeySet: public QSet< T* >
    {

        public:

        //! default constructor
        KeySet( void )
        {}

        /*!
        \brief constructor
        fill the set with all objects of type T associated with the key
        */
        KeySet( const Key* key )
        {

            foreach( Key* associate, key->getAssociated() )
            {
                T* t( dynamic_cast<T*>( associate ) );
                if( t ) this->insert( t );
            }

        }

        /*!
        \brief constructor
        fill the set with all objects of type T associated with the key
        */
        KeySet( const Key& key )
        {

            foreach( Key* associate, key.getAssociated() )
            {
                T* t( dynamic_cast<T*>( associate ) );
                if( t ) this->insert( t );
            }

        }

        //! Merge argument KeySet with this one
        void merge( const KeySet<T>& keySet )
        {
            foreach( T* key, keySet )
            { this->insert( key ); }
        }

    };

    /*! \brief
    templatized sorted set of casted keys.
    Is constructed from a pointer to a Key;
    Contains all objects of type T associated to the Key.
    */
    template<typename T>
    class KeySetIterator: public QSetIterator< T* >
    {
        public:

        //! constructor
        KeySetIterator( KeySet<T> keySet ):
            QSetIterator<T*>( keySet )
        {}
    };

};

//______________________________________________________________
template<typename T> void BASE::Key::clearAssociations( void )
{
    KeySet<T> tmp( this );
    foreach( T* key, tmp )
    {
        key->_disassociate( this );
        _disassociate( key );
    }
}

//____________________________________________________
inline uint qHash( const BASE::Key*& key )
{ return key->key(); }

#endif
