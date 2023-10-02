#include <geodesuka/core/io/file_manager.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace geodesuka::core::io {

	file_manager::file_manager() {
		this->zero_out();
	}

	file_manager::~file_manager() {

	}

	const file* file_manager::operator[](int aFileIndex) const {
		if ((aFileIndex >= 0) && (aFileIndex < this->Count)) {
			return this->Handle[aFileIndex];
		}
		else {
			return nullptr;
		}
	}

	const file* file_manager::operator[](const char* aFilePath) const {
		for (int i = 0; i < this->Count; i++) {
			if (this->Handle[i]->Path == aFilePath) {
				return this->Handle[i];
			}
		}
		return nullptr;
	}

	file* file_manager::open(const char* aFilePath) {
		file* File = nullptr;
		// Insure that aFilePath is absolute path.

		for (int i = 0; i < this->Count; i++) {
			if (this->Handle[i]->Path == aFilePath) {
				File = this->Handle[i];
				break;
			}
		}
		if (File != nullptr) return File;

		// Open File and add to list.
		File = file::open(aFilePath);

		return File;
	}

	void file_manager::close(const char* aFilePath) {

	}

	void file_manager::zero_out() {
		this->Count		= 0;
		this->Handle	= NULL;
	}

}

