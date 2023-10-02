#pragma once
#ifndef GEODESUKA_CORE_UTIL_LIST_H
#define GEODESUKA_CORE_UTIL_LIST_H

#include <cstddef>

#include <vector>

namespace geodesuka::core::util {

	template <typename T>
	class list {
	public:

		std::vector<T> Handle;

		list() {}

		list(size_t aCount) {
			this->Handle.resize(aCount);
		}

		list(T aElement) {
			this->Handle.push_back(aElement);
		}

        list(size_t aCount, T* aElementList) {
            this->Handle.resize(aCount);
            for (size_t i = 0; i < aCount; i++){
                this->Handle[i] = aElementList[i];
            }
        }

        T& operator[](size_t aIndex);
		T operator[](size_t aIndex) const;

		list<T>& operator=(std::vector<T> aRhs);
		list<T> operator&(const list<T>& aRhs) const;
		list<T> operator|(const list<T>& aRhs) const;
		list<T> operator-(const list<T>& aRhs) const;

		list<T> operator&(T aRhs) const;
		list<T> operator|(T aRhs) const;
		list<T> operator-(T aRhs) const;

		list<T>& operator&=(const list<T>& aRhs);
		list<T>& operator|=(const list<T>& aRhs);
		list<T>& operator-=(const list<T>& aRhs);

		list<T>& operator&=(T aRhs);
		list<T>& operator|=(T aRhs);
		list<T>& operator-=(T aRhs);

		list<T> purify() const;
		bool exists(T aObject) const;
		void resize(size_t aNewSize);
		void clear();
	    size_t count() const;

	};

	template<typename T>
	inline T& list<T>::operator[](size_t aIndex) {
		return Handle[aIndex];
	}

	template<typename T>
	inline T list<T>::operator[](size_t aIndex) const {
		return Handle[aIndex];
	}

	template<typename T>
	inline list<T>& list<T>::operator=(std::vector<T> aRhs) {
		this->Handle = aRhs;
		return *this;
	}

	template<typename T>
	inline list<T> list<T>::operator&(const list<T>& aRhs) const {
		list<T> Intersection;
		list<T> Left = this->purify();
		list<T> Right = aRhs.purify();

		size_t k = 0;
		for (size_t i = 0; i < Left.count(); i++) {
			if (Right.exists(Left[i])) {
				k += 1;
			}
		}

		if (k == 0) return Intersection;
		Intersection.resize(k);

		k = 0;
		for (size_t i = 0; i < Left.count(); i++) {
			if (Right.exists(Left[i])) {
				Intersection[k] = Left[i];
				k += 1;
			}
		}

		return Intersection;
	}

	template<typename T>
	inline list<T> list<T>::operator|(const list<T>& aRhs) const {
		list<T> Union;
		list<T> Left = this->purify();
		list<T> Right = aRhs.purify();
		list<T> Intersection = Left & Right;

		size_t LeftUniqueCount = Left.count() - Intersection.count();
		size_t RightUniqueCount = Right.count() - Intersection.count();
		Union.resize(LeftUniqueCount + Intersection.count() + RightUniqueCount);

		size_t k = 0;
		for (size_t i = 0; i < Left.count(); i++) {
			if (!Intersection.exists(Left[i])) {
				Union[k] = Left[i];
				k += 1;
			}
		}

		for (size_t i = 0; i < Intersection.count(); i++) {
			Union[k] = Intersection[i];
			k += 1;
		}

		for (size_t i = 0; i < Right.count(); i++) {
			if (!Intersection.exists(Right[i])) {
				Union[k] = Right[i];
				k += 1;
			}
		}

		return Union;
	}

	template<typename T>
	inline list<T> list<T>::operator-(const list<T>& aRhs) const {
		list<T> Sublist;
		list<T> Left = this->purify();
		list<T> Right = aRhs.purify();
		list<T> Intersection = *this & aRhs;

		if (Intersection.count() == 0) return *this;

		if (Left.count() == Intersection.count()) return Sublist;

		Sublist.resize(Left.count() - Intersection.count());

		size_t k = 0;
		for (size_t i = 0; i < Left.count(); i++) {
			if (!Intersection.exists(Left[i])) {
				Sublist[k] = Left[i];
				k += 1;
			}
		}

		return Sublist;
	}

	template<typename T>
	inline list<T> list<T>::operator&(T aRhs) const {
		return *this & list<T>(aRhs);
	}

	template<typename T>
	inline list<T> list<T>::operator|(T aRhs) const {
		return *this | list<T>(aRhs);
	}

	template<typename T>
	inline list<T> list<T>::operator-(T aRhs) const {
		return *this - list<T>(aRhs);
	}

	template<typename T>
	inline list<T>& list<T>::operator&=(const list<T>& aRhs) {
		*this = *this & aRhs;
		return *this;
	}

	template<typename T>
	inline list<T>& list<T>::operator|=(const list<T>& aRhs) {
		*this = *this | aRhs;
		return *this;
	}

	template<typename T>
	inline list<T>& list<T>::operator-=(const list<T>& aRhs) {
		*this = *this - aRhs;
		return *this;
	}

	template<typename T>
	inline list<T>& list<T>::operator&=(T aRhs) {
		*this = *this & list<T>(aRhs);
		return *this;
	}

	template<typename T>
	inline list<T>& list<T>::operator|=(T aRhs) {
		*this = *this | list<T>(aRhs);
		return *this;
	}

	template<typename T>
	inline list<T>& list<T>::operator-=(T aRhs) {
		*this = *this - list<T>(aRhs);
		return *this;
	}

	template<typename T>
	inline list<T> list<T>::purify() const {

		// Count The Number Of Duplicates In Set.
		size_t RepeatCount = 0;
		size_t Count = this->count();
		for (size_t i = 0; i < Count; i++) {
			for (size_t j = i + 1; j < Count; j++) {
				if (this->Handle[i] == this->Handle[j]) {
					RepeatCount += 1;
				}
			}
		}

		size_t UniqueCount = Count - RepeatCount;
		if (UniqueCount == this->count()) return *this;
		list<T> Purified(UniqueCount);
		//Purified.resize(UniqueCount);

		size_t Offset = 0;
		for (size_t i = 0; i < Count; i++) {
			if (!Purified.exists(this->Handle[i])) {
				Purified[Offset] = this->Handle[i];
				Offset += 1;
			}
		}

		return Purified;
	}

	template<typename T>
	inline bool list<T>::exists(T aObject) const {
		for (int i = 0; i < this->count(); i++) {
			if (this->Handle[i] == aObject) {
				return true;
                break;
			}
		}
		return false;
	}

	template<typename T>
	inline void list<T>::resize(size_t aNewSize) {
        this->Handle.resize(aNewSize);
	}

	template<typename T>
	inline void list<T>::clear() {
        this->Handle.clear();
	}

	template<typename T>
	inline size_t list<T>::count() const {
		return this->Handle.size();
	}

}

#endif // !GEODESUKA_CORE_UTIL_LIST_H
