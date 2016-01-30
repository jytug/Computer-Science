#ifndef _BST_IMP_H
#define _BST_IMP_H

#include "bst.h"
#include <algorithm>
#include <iostream>

/*
===========================================================
	BST METHODS
===========================================================
*/

template <typename T>
BST<T>::BST(std::initializer_list<T> l) : 
	BST(l.begin(), l.end()) {}

template <typename T>
template <typename Iter>
BST<T>::BST(Iter begin, Iter end) : 
	BST([&] () -> BST<T> {
		std::function<BST<T> (BST<T>, Iter, Iter)> build
			= [&](BST<T> t, Iter begin, Iter end) -> BST<T> {
			return begin == end ?
				t :
				build(t + *begin, std::next(begin), end);
		};
		return build(BST<T>(), begin, end);
	}()) {}

template <typename T>
BST<T>::BST(T value, BST left, BST right) :
	m_root(new Node(value, left.m_root, right.m_root)) {
}

template <typename T>
BST<T> BST<T>::left() const {	
	if (empty())
		throw std::logic_error("An empty tree has no children");
	return BST(m_root->m_left);
}

template <typename T>
BST<T> BST<T>::right() const {
	if (empty())
		throw std::logic_error("An empty tree has no chilren");
	return BST(m_root->m_right);
}

template <typename T>
T const & BST<T>::value() const {
	if (empty())
		throw std::logic_error("An empty tree has no value");
	return m_root->m_value;
}

template <typename T>
bool BST<T>::empty() const {
	return m_root == nullptr;
}

template <typename T>
T const & BST<T>::min() const {
	if (empty())
		throw std::logic_error("There's no minimum in an empty tree");
	if (m_root->m_left != nullptr)
		return left().min();
	else
		return value();
}

template <typename T>
T const & BST<T>::max() const {
	if (empty())
		throw std::logic_error("There's no maximum in an empty tree");
	if (m_root->m_right != nullptr)
		return right().max();
	else
		return value();
}

template <typename T>
template <typename Acc, typename Functor>
Acc BST<T>::fold(Acc a, Functor f) const {
	if (!empty()) {
		return (right().empty() ?
			f(left().fold(a, f) , value()) :
			right().fold(f(left().fold(a, f), value()), f));
	} else {
		return a;
	}
}

template <typename T>
BST<T> BST<T>::find(T const & t) const {
	if (empty())
		return BST<T>();
	else if (t > value())
		return right().find(t);
	else if (t < value())
		return left().find(t);
	else
		return *this;
}

template <typename T>
std::size_t BST<T>::size() const {
	return fold(0,
		[] (int x, T t) -> int {
			return x + 1;
		});
}

template <typename T>
std::size_t BST<T>::height() const {
	if (empty()) {
		return 0;
	} else {
		return std::max(left().height(), right().height()) + 1;
	}
}


/*
===========================================================
	BST-RELATED FUNCTIONS
===========================================================
*/

template <typename T>
BST<T> spine(BST<T> tree) {
	return tree.fold(BST<T>(),
		[](BST<T> left, T t) -> BST<T> {
			return BST<T>(t, left, BST<T>());
		});
}

template <typename T>
BST<T> operator+(T v, BST<T> tree) {
	return (tree.empty() ? BST<T>(v, BST<T>(), BST<T>()) :
				(v > tree.value() ?
				BST<T>(tree.value(), tree.left(), v + tree.right()) :
				BST<T>(tree.value(), v + tree.left(), tree.right())));
}

template <typename T>
BST<T> operator+(BST<T> tree, T v) {
	return v + tree;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, BST<T> tree) {
	if (!tree.empty()) {
		os << tree.left() << tree.value() << " " << tree.right();
	}
	return os;
}

template <typename T>
T max_diff(BST<T> tree) {
	if (tree.size() < 2)
		throw std::logic_error("max_diff requires the tree to have at least 2 nodes");
	T result = tree.min() - tree.min();
	tree.fold(tree.min(), 
		[&result](T t1, T t2) -> T {
			result = std::max(result, t2 - t1);
			return t2;
		}); 
	return result;
}
#endif
