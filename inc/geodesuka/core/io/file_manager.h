#pragma once
#ifndef GEODESUKA_CORE_IO_FILE_MANAGER_H
#define GEODESUKA_CORE_IO_FILE_MANAGER_H

#include "file.h"

namespace geodesuka::core::io {

	class file_manager {
	public:

		file_manager();
		~file_manager();

		const file* operator[](int aFileIndex) const;
		const file* operator[](const char* aFilePath) const;

		file* open(const char* aFilePath);
		void close(const char* aFilePath);

		void clear();

	private:

		int Count;
		file** Handle;

		bool resize(int aNewCount);

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_IO_FILE_MANAGER_H
