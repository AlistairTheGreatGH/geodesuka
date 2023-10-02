#pragma once
#ifndef GEODESUKA_CORE_IO_FONT_H
#define GEODESUKA_CORE_IO_FONT_H

#include "../../config.h"

#include "../io/file.h"
#include "../gcl/image.h"

namespace geodesuka::core::graphics {

	// Rename class to typeface?
	class font : public io::file {
	public:

		friend class geodesuka::engine;

		/*

		texture SymbolSet;

		int m, n, l;
		void *hptr;
		float *sx;
		float *sy;
		float *bx;
		float *by;
		float *ax;
		float *ay;

		font();
		~font();

		bool read(const char* Path);
		bool load();
		bool clear();
		*/

		font();
		font(util::string aFilePath);
		font(const char* aFilePath);

		~font();

	private:

		static bool initialize();
		static bool terminate();

		int m, n, l;
		void* hptr;
		float* sx;
		float* sy;
		float* bx;
		float* by;
		float* ax;
		float* ay;

		gcl::image Symbol;

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_IO_FONT_H
