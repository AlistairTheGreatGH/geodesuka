#include <geodesuka/core/util/string.h>

// Standard C Library.
//#include <limits.h>
#include <stdint.h>
//#include <float.h>
//#include <stdio.h>
#include <stdlib.h>
//#include <stdarg.h>
#include <string.h>
//#include <ctype.h>
//#include <math.h>

#include <assert.h>

namespace geodesuka::core::util {

	string::string() {
		this->Size = 0;
		this->Handle = NULL;
	}

	string::string(char aCharacter) {
		this->zero_out();
		assert(!this->resize(1));
		this->Handle[0] = aCharacter;
	}

	string::string(int aCodepoint) {
		this->zero_out();
		if (aCodepoint < 0x00080) {
			char String[2] = { 0x00, '\0' };

			String[0] = aCodepoint & 0b01111111;

			*this = String;
		} else if (aCodepoint < 0x00800) {
			char String[3] = { 0x00, 0x00, '\0' };

			String[0] = String[0] | 0b11000000; // 5 bits
			String[1] = String[1] | 0b10000000; // 6 bits

			String[0] = String[0] | (((aCodepoint >> 6) & 0b00011111) << 6);
			String[1] = String[1] | (((aCodepoint >> 0) & 0b00111111) << 0);

			*this = String;
		} else if (aCodepoint < 0x08000) {
			char String[4] = { 0x00, 0x00, 0x00, '\0' };
			String[0] = String[0] | 0b11100000; // 4 bits
			String[1] = String[1] | 0b10000000; // 6 bits
			String[2] = String[2] | 0b10000000; // 6 bits

			String[0] = String[0] | (((aCodepoint >> 12) & 0b00001111) << 12);
			String[1] = String[1] | (((aCodepoint >> 6)  & 0b00111111) << 6);
			String[2] = String[2] | (((aCodepoint >> 0)  & 0b00111111) << 0);

			*this = String;
		} else {
			char String[5] = { 0x00, 0x00, 0x00, 0x00, '\0' };
			String[0] = String[0] | 0b11110000; // 3 bits
			String[1] = String[1] | 0b10000000; // 6 bits
			String[2] = String[2] | 0b10000000; // 6 bits
			String[3] = String[3] | 0b10000000; // 6 bits

			String[0] = String[0] | (((aCodepoint >> 18) & 0b00000111) << 18);
			String[1] = String[1] | (((aCodepoint >> 12) & 0b00111111) << 12);
			String[2] = String[2] | (((aCodepoint >> 6)  & 0b00111111) << 6);
			String[3] = String[3] | (((aCodepoint >> 0)  & 0b00111111) << 0);

			*this = String;
		}
	}

	string::string(const char* aInput) {
		this->zero_out();
		assert(!this->resize(strlen(aInput)));
		memcpy(this->Handle, aInput, this->Size * sizeof(char));
	}

	string::string(const string& aInput) {
		this->zero_out();
		assert(!this->resize(aInput.Size));
		memcpy(this->Handle, aInput.Handle, this->Size * sizeof(char));
	}

	string::string(string&& aInput) noexcept {
		this->Size = aInput.Size;
		this->Handle = aInput.Handle;
		aInput.zero_out();
	}

	string::~string() {
		this->clear();
	}

	char& string::operator[](size_t aIndex) {
		return this->Handle[aIndex];
	}

	char string::operator[](size_t aIndex) const {
		return this->Handle[aIndex];
	}

	string& string::operator=(char aCharacter) {
		return (*this = string(aCharacter));
	}

	string& string::operator=(int aCodepoint) {
		return (*this = string(aCodepoint));
	}

	string& string::operator=(const char* aRhs) {
		size_t StrLen = 0;
		if (aRhs != NULL) {
			StrLen = strlen(aRhs);
		}
		assert(!this->resize(StrLen));
		if (aRhs == NULL) return *this;
		memcpy(this->Handle, aRhs, this->Size * sizeof(char));
		return *this;
	}

	string& string::operator=(const string& aRhs) {
		if (this == &aRhs) return *this;
		return (*this = aRhs.Handle);
	}

	string& string::operator=(string&& aRhs) noexcept {
		this->clear();
		this->Size = aRhs.Size;
		this->Handle = aRhs.Handle;
		aRhs.zero_out();
		return *this;
	}

	bool string::operator==(const char* aRhs) const {
		if (aRhs == NULL) return this->Size == 0;
		if (this->Size != strlen(aRhs)) return false;
		return (memcmp(this->Handle, aRhs, this->Size * sizeof(char)) == 0);
	}

	bool string::operator==(const string& aRhs) const {
		return *this == aRhs.Handle;
	}

	string string::operator+(char aCharacter) const {
		return (*this + string(aCharacter));
	}

	string string::operator+(int aCodepoint) const {
		return (*this + string(aCodepoint));
	}

	string string::operator+(const char* aRhs) const {
		if (aRhs == NULL) return *this;
		string String;
		size_t aRhsSize = strlen(aRhs);
		uintptr_t A1 = 0, A2 = 0;

		assert(!String.resize(this->Size + aRhsSize));

		A1 = (uintptr_t)String.Handle;
		A2 = (uintptr_t)String.Handle + this->Size * sizeof(char);

		memcpy((void*)A1, this->Handle, this->Size *sizeof(char));
		memcpy((void*)A2, aRhs, aRhsSize * sizeof(char));

		return String;
	}

	string string::operator+(const string& aRhs) const {
		return (*this + aRhs.Handle);
	}

	string& string::operator+=(char aCharacter) {
		*this = *this + string(aCharacter);
		return *this;
	}

	string& string::operator+=(int aCodepoint){
		*this = *this + string(aCodepoint);
		return *this;
	}

	string& string::operator+=(const char* aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	string& string::operator+=(const string& aRhs) {
		*this = *this + aRhs;
		return *this;
	}

	bool string::push(const char* aString) {
		*this += aString;
		return false;
	}

	bool string::push(string& aString) {
		*this += aString;
		return false;
	}

	bool string::pop(size_t aCount) {
		assert(!this->resize(this->Size - aCount));
		return false;
	}

	bool string::insert(size_t aIndex, const char* aString) {

		return false;
	}

	bool string::insert(size_t aIndex, string& aString) {
		return this->insert(aIndex, aString.Handle);
	}

	bool string::remove(size_t aIndex, size_t aCount) {

		return false;
	}

	bool string::exchange(size_t aI, size_t aJ, size_t aCount) {
		for (size_t i = 0; i < aCount; i++) {
			char temp 				= this->Handle[aI + i];
			this->Handle[aI + i] 	= this->Handle[aJ + i];
			this->Handle[aJ + i] 	= temp;
		}
		return false;
	}

	bool string::reverse(size_t Index, size_t Count) {
		size_t HalfCount = (Count - Count % 2) / 2;
		for (size_t i = 0; i < HalfCount; i++) {
			char temp = this->Handle[i + Index];
			this->Handle[i + Index] = this->Handle[Count + Index - i - 1];
			this->Handle[Count + Index - i - 1] = temp;
		}
		return false;
	}

	bool string::reverse() {
		return this->reverse(0, this->Size);
	}

	bool string::push(char aCharacter) {
		char temp[2];
		temp[0] = aCharacter;
		temp[1] = '\0';
		return this->push(temp);
	}

	bool string::pop() {
		return this->pop(1);
	}

	bool string::insert(size_t aIndex, char aCharacter) {
		char temp[2];
		temp[0] = aCharacter;
		temp[1] = '\0';
		return this->insert(aIndex, temp);
	}

	bool string::remove(size_t aIndex) {
		return this->remove(aIndex, 1);
	}

	bool string::exchange(size_t aI, size_t aJ) {
		return this->exchange(aI, aJ, 1);
	}

	bool string::permute(int Count) {
		if ((Handle == NULL) || (Size == 0)) { return true; }
		void* nptr = NULL;
		int a = abs(Count);
		int b = Size;
		if (a >= Size) { a = a % Size; }
		if ((Count != 0) && (a != 0)) {
			nptr = malloc(b * sizeof(char));
			if (nptr == NULL) { return true; }
			if (Count > 0) {
				// Positive Integers
				memcpy(nptr, Handle + a * sizeof(char), (b - a) * sizeof(char));
				memcpy((char*)nptr + (b - a) * sizeof(char), Handle, a * sizeof(char));
			}
			else {
				// Negative Integers
				memcpy(nptr, Handle + (b - a) * sizeof(char), a * sizeof(char));
				memcpy((char*)nptr + a * sizeof(char), Handle, (b - a) * sizeof(char));
			}
			memcpy(Handle, nptr, b * sizeof(char));
			free(nptr); nptr = NULL;
		}
		return false;
	}

	string string::split_at(char aCharacter) {
		char S[2];
		S[0] = aCharacter;
		S[1] = '\0';
		return this->split_at(S);
	}

	string string::split_at(const char* aPattern) {
		string temp;
		size_t Location = -1;
		size_t MatchCount = 0;
		size_t StrLength = strlen(aPattern);
		for (size_t i = 0; i < this->Size; i++) {
			if (this->Handle[i] == aPattern[MatchCount]) {
				if (MatchCount == 0) {
					Location = i;
				}
				MatchCount += 1;
			}
			else {
				Location = -1;
				MatchCount = 0;
			}
			if (MatchCount == StrLength) break;
		}
		if (Location > 0) {
			temp.Handle = (char*)malloc((Location + 1) * sizeof(char));
			if (temp.Handle == NULL) return temp;
			temp.Size = Location;
			memcpy(temp.Handle, this->Handle, temp.Size * sizeof(char));
			temp.Handle[temp.Size] = '\0';
			//this->remove(0, Location);
			this->remove(0, Location + StrLength);
		}
		return temp;
	}

	string string::split_at(const string& aPattern) {
		return this->split_at(aPattern.Handle);
	}

	bool string::resize(size_t aNewCount) {
		if (this->Size == aNewCount) return false;
		if (aNewCount > 0) {
			size_t TypeSize = sizeof(char);
			void* nptr = NULL;
			if (this->Handle == NULL) {
				nptr = malloc((aNewCount + 1) * TypeSize);
			}
			else {
				nptr = realloc(this->Handle, (aNewCount + 1) * TypeSize);
			}
			if (nptr == NULL) return true;
			if (aNewCount > this->Size) {
				uintptr_t Address = (uintptr_t)nptr + this->Size * TypeSize;
				memset((void*)Address, 0x00, (aNewCount - this->Size) * TypeSize);
			}
			this->Size = aNewCount;
			this->Handle = (char*)nptr;
			this->Handle[this->Size] = '\0';
			return false;
		}
		else {
			this->clear();
			return false;
		}
	}

	void string::clear() {
		free(this->Handle);
		this->zero_out();
	}

	size_t string::size() const {
		return this->Size;
	}

	const char* string::ptr() const {
		return this->Handle;
	}

	void string::zero_out() {
		this->Size = 0;
		this->Handle = NULL;
	}

}
