//
// Copyright (C) 2021 OpenSim Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
//


#ifndef __INET_STLUTILS_H
#define __INET_STLUTILS_H

// various utility functions to make STL containers more usable
// (Copy of omnetpp stlutil.h)

#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "inet/common/INETDefs.h" // for ASSERT

namespace inet {

template<typename T>
typename std::vector<T>& addAll(std::vector<T>& v, const std::vector<T>& w) {
    v.insert(v.end(), w.begin(), w.end());
    return v;
}

template<typename T, typename _C, typename _C2>
typename std::set<T,_C>& addAll(std::set<T,_C>& s, const std::set<T,_C2>& t) {
    s.insert(t.begin(), t.end());
    return s;
}

template<typename K, typename V, typename _C, typename _C2>
inline std::map<K,V,_C>& addAll(std::map<K,V,_C>& m, const std::map<K,V,_C2>& n) {
    m.insert(n.begin(), n.end());
    return m;
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
typename std::vector<T>::iterator find(std::vector<T>& v, const Tk& a) {
    return std::find(v.begin(), v.end(), a);
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
typename std::vector<T>::const_iterator find(const std::vector<T>& v, const Tk& a) {
    return std::find(v.begin(), v.end(), a);
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
inline int count(const std::vector<T>& v, const Tk& a) {
    return std::count(v.begin(), v.end(), a);
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
int indexOf(const std::vector<T>& v, const Tk& a) {
    auto it = find(v, a);
    return it == v.end() ? -1 : it - v.begin();
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
inline bool contains(const std::vector<T>& v, const Tk& a) {
    return find(v, a) != v.end();
}

template<typename T, typename _C, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
inline bool contains(const std::set<T,_C>& s, const Tk& a) {
    return s.find(a) != s.end();
}

template<typename T, typename _H, typename _P, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
inline bool contains(const std::unordered_set<T,_H,_P>& s, const Tk& a) {
    return s.find(a) != s.end();
}

template<typename K, typename V, typename _C, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, K>::value>::type>
inline bool containsKey(const std::map<K,V,_C>& m, const Tk& a) {
    return m.find(a) != m.end();
}

template<typename K, typename V, typename _C, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, K>::value>::type>
inline bool containsKey(const std::multimap<K,V,_C>& m, const Tk& a) {
    return m.find(a) != m.end();
}

template<typename K, typename V, typename _H, typename _P, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, K>::value>::type>
inline bool containsKey(const std::unordered_map<K,V,_H,_P>& m, const Tk& a) {
    return m.find(a) != m.end();
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
void insert(std::vector<T>& v, int pos, const Tk& a) {
    ASSERT(pos >= 0 && (size_t)pos <= v.size());
    v.insert(v.begin() + pos, a);
}

template<typename T>
void erase(std::vector<T>& v, int pos) {
    ASSERT(pos >= 0 && (size_t)pos < v.size());
    v.erase(v.begin() + pos);
}

template<typename T, typename Tk, typename = typename std::enable_if<std::is_convertible<Tk, T>::value>::type>
inline void remove(std::vector<T>& v, const Tk& a) {
    if (!v.empty()) // optimization
        v.erase(std::remove(v.begin(), v.end(), a), v.end());
}

template<typename K, typename V>
inline std::vector<K> keys(const std::map<K,V>& m) {
    std::vector<K> result;
    for (auto it = m.begin(); it != m.end(); ++it)
        result.push_back(it->first);
    return result;
}

template<typename K, typename V>
inline std::vector<V> values(const std::map<K,V>& m) {
    std::vector<V> result;
    for (auto it = m.begin(); it != m.end(); ++it)
        result.push_back(it->second);
    return result;
}

template<typename T>
void sort(std::vector<T>& v) {
    std::sort(v.begin(), v.end());
}

template<typename T>
std::vector<T> sorted(const std::vector<T>& v) {
    std::vector<T> result = v;
    std::sort(result.begin(), result.end());
    return result;
}

template <typename T>
std::string to_str(const std::vector<T>& v) {
    std::stringstream out;
    out << '[';
    for (auto it = v.begin(); it != v.end(); ++it) {
        if (it != v.begin())
            out << ", ";
        out << *it;
    }
    out << "]";
    return out.str();
}

template <typename K, typename V, typename _C>
std::string to_str(const std::map<K,V,_C>& m) {
    std::stringstream out;
    out << '{';
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it != m.begin())
            out << ", ";
        out << it->first << " -> " << it->second;
    }
    out << "}";
    return out.str();
}

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) // from boost
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second); // combining could be more sophisticated
    }
};

} // namespace inet

#endif

