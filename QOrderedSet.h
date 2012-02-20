/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QORDEREDSET_H
#define QORDEREDSET_H

#include <QtCore/QMap>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

struct QOrderedSetDummyValue
{};

template <class T>
class QOrderedSet
{
    typedef QMap<T, QOrderedSetDummyValue> Map;

public:
    inline QOrderedSet()
    {}

    inline QOrderedSet(const QOrderedSet<T> &other) :
        q_map(other.q_map)
        {}

    inline QOrderedSet<T> &operator=(const QOrderedSet<T> &other)
    { q_map = other.q_map; return *this; }

    #ifdef Q_COMPILER_RVALUE_REFS
    inline QOrderedSet<T> &operator=(QOrderedSet<T> &&other)
    { qSwap(q_map, other.q_map); return *this; }
    #endif

    inline void swap(QOrderedSet<T> &other)
    { q_map.swap(other.q_map); }

    inline bool operator==(const QOrderedSet<T> &other) const
    { return q_map == other.q_map; }

    inline bool operator!=(const QOrderedSet<T> &other) const
    { return q_map != other.q_map; }

    inline int size() const
    { return q_map.size(); }

    inline bool isEmpty() const
    { return q_map.isEmpty(); }

    inline int capacity() const
    { return q_map.capacity(); }

    inline void reserve(int size);

    inline void squeeze()
    { q_map.squeeze(); }

    inline void detach()
    { q_map.detach(); }

    inline bool isDetached() const
    { return q_map.isDetached(); }

    inline void setSharable(bool sharable)
    { q_map.setSharable(sharable); }

    inline void clear()
    { q_map.clear(); }

    inline bool remove(const T &value)
    { return q_map.remove(value) != 0; }

    inline bool contains(const T &value) const
    { return q_map.contains(value); }

    bool contains(const QOrderedSet<T> &set) const;

    class const_iterator;

    class iterator
    {
        typedef QMap<T, QOrderedSetDummyValue> Map;
        typename Map::iterator i;
        friend class const_iterator;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline iterator()
        {}

        inline iterator(typename Map::iterator o) : i(o)
        {}

        inline iterator(const iterator &o) : i(o.i)
        {}

        inline iterator &operator=(const iterator &o)
        { i = o.i; return *this; }

        inline const T &operator*() const
        { return i.key(); }

        inline const T *operator->() const
        { return &i.key(); }

        inline bool operator==(const iterator &o) const
        { return i == o.i; }

        inline bool operator!=(const iterator &o) const
        { return i != o.i; }

        inline bool operator==(const const_iterator &o) const
        { return i == o.i; }

        inline bool operator!=(const const_iterator &o) const
        { return i != o.i; }

        inline iterator &operator++()
        { ++i; return *this; }

        inline iterator operator++(int)
        { iterator r = *this; ++i; return r; }

        inline iterator &operator--()
        { --i; return *this; }

        inline iterator operator--(int)
        { iterator r = *this; --i; return r; }

        inline iterator operator+(int j) const
        { return i + j; }

        inline iterator operator-(int j) const
        { return i - j; }

        inline iterator &operator+=(int j)
        { i += j; return *this; }

        inline iterator &operator-=(int j)
        { i -= j; return *this; }

    };

    class const_iterator
    {
        typedef QMap<T, QOrderedSetDummyValue> Map;
        typename Map::const_iterator i;
        friend class iterator;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef qptrdiff difference_type;
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline const_iterator() {}
        inline const_iterator(typename Map::const_iterator o):
            i(o)
        {}
        inline const_iterator(const const_iterator &o):
            i(o.i)
        {}

        inline const_iterator(const iterator &o):
            i(o.i)
            {}

        inline const_iterator &operator=(const const_iterator &o)
        { i = o.i; return *this; }

        inline const T &operator*() const
        { return i.key(); }

        inline const T *operator->() const
        { return &i.key(); }

        inline bool operator==(const const_iterator &o) const
        { return i == o.i; }

        inline bool operator!=(const const_iterator &o) const
        { return i != o.i; }

        inline const_iterator &operator++()
        { ++i; return *this; }

        inline const_iterator operator++(int)
        { const_iterator r = *this; ++i; return r; }

        inline const_iterator &operator--()
        { --i; return *this; }

        inline const_iterator operator--(int)
        { const_iterator r = *this; --i; return r; }

        inline const_iterator operator+(int j) const
        { return i + j; }

        inline const_iterator operator-(int j) const
        { return i - j; }

        inline const_iterator &operator+=(int j)
        { i += j; return *this; }

        inline const_iterator &operator-=(int j)
        { i -= j; return *this; }
    };

    // STL style
    inline iterator begin()
    { return q_map.begin(); }

    inline const_iterator begin() const
    { return q_map.begin(); }

    inline const_iterator constBegin() const
    { return q_map.constBegin(); }

    inline iterator end()
    { return q_map.end(); }

    inline const_iterator end() const
    { return q_map.end(); }

    inline const_iterator constEnd() const
    { return q_map.constEnd(); }

    iterator erase(iterator i)
    { return q_map.erase(reinterpret_cast<typename Map::iterator &>(i)); }

    // more Qt
    typedef iterator Iterator;
    typedef const_iterator ConstIterator;

    inline int count() const
    { return q_map.count(); }

    inline const_iterator insert(const T &value) // ### Qt 5: should return an 'iterator'
    { return static_cast<typename Map::const_iterator>(q_map.insert(value, QOrderedSetDummyValue())); }

    iterator find(const T &value)
    { return q_map.find(value); }

    const_iterator find(const T &value) const
    { return q_map.find(value); }

    inline const_iterator constFind(const T &value) const
    { return find(value); }

    QOrderedSet<T> &unite(const QOrderedSet<T> &other);

    QOrderedSet<T> &intersect(const QOrderedSet<T> &other);

    QOrderedSet<T> &subtract(const QOrderedSet<T> &other);

    // STL compatibility
    typedef T key_type;
    typedef T value_type;
    typedef value_type *pointer;
    typedef const value_type *const_pointer;
    typedef value_type &reference;
    typedef const value_type &const_reference;
    typedef qptrdiff difference_type;
    typedef int size_type;

    inline bool empty() const
    { return isEmpty(); }

    // comfort
    inline QOrderedSet<T> &operator<<(const T &value)
    { insert(value); return *this; }

    inline QOrderedSet<T> &operator|=(const QOrderedSet<T> &other)
    { unite(other); return *this; }

    inline QOrderedSet<T> &operator|=(const T &value)
    { insert(value); return *this; }

    inline QOrderedSet<T> &operator&=(const QOrderedSet<T> &other)
    { intersect(other); return *this; }

    inline QOrderedSet<T> &operator&=(const T &value)
    { QOrderedSet<T> result; if (contains(value)) result.insert(value); return (*this = result); }

    inline QOrderedSet<T> &operator+=(const QOrderedSet<T> &other)
    { unite(other); return *this; }

    inline QOrderedSet<T> &operator+=(const T &value)
    { insert(value); return *this; }

    inline QOrderedSet<T> &operator-=(const QOrderedSet<T> &other)
    { subtract(other); return *this; }

    inline QOrderedSet<T> &operator-=(const T &value)
    { remove(value); return *this; }

    inline QOrderedSet<T> operator|(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result |= other; return result; }

    inline QOrderedSet<T> operator&(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result &= other; return result; }

    inline QOrderedSet<T> operator+(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result += other; return result; }

    inline QOrderedSet<T> operator-(const QOrderedSet<T> &other) const
    { QOrderedSet<T> result = *this; result -= other; return result; }

    #if QT_VERSION < 0x050000
    // ### Qt 5: remove
    inline QOrderedSet<T> operator|(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result |= other; return result; }

    inline QOrderedSet<T> operator&(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result &= other; return result; }

    inline QOrderedSet<T> operator+(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result += other; return result; }

    inline QOrderedSet<T> operator-(const QOrderedSet<T> &other)
    { QOrderedSet<T> result = *this; result -= other; return result; }
#endif

    QList<T> toList() const;
    inline QList<T> values() const { return toList(); }

    static QOrderedSet<T> fromList(const QList<T> &list);

private:
    Map q_map;
};

template <class T>
Q_INLINE_TEMPLATE void QOrderedSet<T>::reserve(int asize) { q_map.reserve(asize); }

template <class T>
Q_INLINE_TEMPLATE QOrderedSet<T> &QOrderedSet<T>::unite(const QOrderedSet<T> &other)
{
    QOrderedSet<T> copy(other);
    typename QOrderedSet<T>::const_iterator i = copy.constEnd();
    while (i != copy.constBegin()) {
        --i;
        insert(*i);
    }
    return *this;
}

template <class T>
Q_INLINE_TEMPLATE QOrderedSet<T> &QOrderedSet<T>::intersect(const QOrderedSet<T> &other)
{
    QOrderedSet<T> copy1(*this);
    QOrderedSet<T> copy2(other);
    typename QOrderedSet<T>::const_iterator i = copy1.constEnd();
    while (i != copy1.constBegin()) {
        --i;
        if (!copy2.contains(*i))
            remove(*i);
    }
    return *this;
}

template <class T>
Q_INLINE_TEMPLATE QOrderedSet<T> &QOrderedSet<T>::subtract(const QOrderedSet<T> &other)
{
    QOrderedSet<T> copy1(*this);
    QOrderedSet<T> copy2(other);
    typename QOrderedSet<T>::const_iterator i = copy1.constEnd();
    while (i != copy1.constBegin()) {
        --i;
        if (copy2.contains(*i))
            remove(*i);
    }
    return *this;
}

template <class T>
Q_INLINE_TEMPLATE bool QOrderedSet<T>::contains(const QOrderedSet<T> &other) const
{
    typename QOrderedSet<T>::const_iterator i = other.constBegin();
    while (i != other.constEnd()) {
        if (!contains(*i))
            return false;
        ++i;
    }
    return true;
}

template <typename T>
Q_OUTOFLINE_TEMPLATE QList<T> QOrderedSet<T>::toList() const
{
    QList<T> result;
    result.reserve(size());
    typename QOrderedSet<T>::const_iterator i = constBegin();
    while (i != constEnd()) {
        result.append(*i);
        ++i;
    }
    return result;
}

template <typename T>
QOrderedSet<T> QOrderedSet<T>::fromList(const QList<T> &list)
{
    QOrderedSet<T> out;
    foreach( const T& t, list )
    { out.insert( t ); }

    return out;
}

Q_DECLARE_SEQUENTIAL_ITERATOR(OrderedSet)

template <typename T>
class QMutableOrderedSetIterator
{
    typedef typename QOrderedSet<T>::iterator iterator;
    QOrderedSet<T> *c;
    iterator i, n;
    inline bool item_exists() const { return c->constEnd() != n; }

public:
    inline QMutableOrderedSetIterator(QOrderedSet<T> &container)
        : c(&container)
    { c->setSharable(false); i = c->begin(); n = c->end(); }
    inline ~QMutableOrderedSetIterator()
    { c->setSharable(true); }
    inline QMutableOrderedSetIterator &operator=(QOrderedSet<T> &container)
    { c->setSharable(true); c = &container; c->setSharable(false);
      i = c->begin(); n = c->end(); return *this; }
    inline void toFront() { i = c->begin(); n = c->end(); }
    inline void toBack() { i = c->end(); n = i; }
    inline bool hasNext() const { return c->constEnd() != i; }
    inline const T &next() { n = i++; return *n; }
    inline const T &peekNext() const { return *i; }
    inline bool hasPrevious() const { return c->constBegin() != i; }
    inline const T &previous() { n = --i; return *n; }
    inline const T &peekPrevious() const { iterator p = i; return *--p; }
    inline void remove()
    { if (c->constEnd() != n) { i = c->erase(n); n = c->end(); } }
    inline const T &value() const { Q_ASSERT(item_exists()); return *n; }
    inline bool findNext(const T &t)
    { while (c->constEnd() != (n = i)) if (*i++ == t) return true; return false; }
    inline bool findPrevious(const T &t)
    { while (c->constBegin() != i) if (*(n = --i) == t) return true;
      n = c->end(); return false;  }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSET_H
