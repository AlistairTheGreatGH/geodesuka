#pragma once
#ifndef GEODESUKA_CORE_UTIL_STRING_H
#define GEODESUKA_CORE_UTIL_STRING_H

#include <stddef.h>

namespace geodesuka::core::util {

	class string {
	public:

		string();
		string(char aCharacter);
		string(int aCodepoint);
		string(const char* aInput);
		string(const string& aInput);
		string(string&& aInput) noexcept;
		~string();

		char& operator[](size_t aIndex);
		char operator[](size_t aIndex) const;

		string& operator=(char aCharacter);
		string& operator=(int aCodepoint);
		string& operator=(const char* aRhs);
		string& operator=(const string& aRhs);
		string& operator=(string&& aRhs) noexcept;

		bool operator==(const char* aRhs) const;
		bool operator==(const string& aRhs) const;

		string operator+(char aCharacter) const;
		string operator+(int aCodepoint) const;
		string operator+(const char* aRhs) const;
		string operator+(const string& aRhs) const;

		string& operator+=(char aCharacter);
		string& operator+=(int aCodepoint);
		string& operator+=(const char* aRhs);
		string& operator+=(const string& aRhs);

		// Multi Character Modifiers
		bool push(const char* aString);
		bool push(string& aString);
		bool pop(size_t aCount);
		bool insert(size_t aIndex, const char* aString);
		bool insert(size_t aIndex, string& aString);
		bool remove(size_t aIndex, size_t aCount);
		bool exchange(size_t aI, size_t aJ, size_t aCount);
		bool reverse(size_t aIndex, size_t aCount);
		bool reverse();

		// Single Character Modifiers
		bool push(char aCharacter);
		bool pop();
		bool insert(size_t aIndex, char aCharacter);
		bool remove(size_t aIndex);
		bool exchange(size_t aI, size_t aJ);

		bool permute(int aCount);

		string split_at(char aCharacter);
		string split_at(const char* aPattern);
		string split_at(const string& aPattern);

		// Will resize string buffer to size aNewCount + NullChar.
		bool resize(size_t aNewCount);

		// Clears string.
		void clear();

		size_t size() const;

		const char* ptr() const;

	//private:

		char* Handle;
		size_t Size;

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_UTIL_STRING_H
