#include <geodesuka/core/io/dynalib.h>

#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#elif defined(__APPLE__) || defined(MACOSX)
#include <dlfcn.h>
#elif defined(__linux__) && !defined(__ANDROID__)
#include <dlfcn.h>
#elif defined(__ANDROID__)
#include <dlfcn.h>
#endif

namespace geodesuka::core::io {



	dynalib::dynalib(const char* aLibraryPath) {
		if (!this->setup(aLibraryPath)) {
			this->mloadlib(aLibraryPath);
		}
		else {
			this->Handle = NULL;
		}
	}

	dynalib::~dynalib() {
#if defined(_WIN32) || defined(_WIN64)
		FreeLibrary(reinterpret_cast<HMODULE>(this->Handle));
#elif defined(__APPLE__) || defined(MACOSX)

#elif defined(__linux__) && !defined(__ANDROID__)
		dlclose(this->Handle);
#elif defined(__ANDROID__)

#endif
	}

	// Fetch function pointer from loaded library.
	void* dynalib::get_pfn(const char* aName) {
		void* pfn = NULL;
#if defined(_WIN32) || defined(_WIN64)
		pfn = reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(this->Handle), LPCSTR(aName)));
#elif defined(__APPLE__) || defined(MACOSX)

#elif defined(__linux__) && !defined(__ANDROID__)
		pfn = dlsym(this->Handle, aName);
#elif defined(__ANDROID__)

#endif
		return pfn;
	}

	bool dynalib::mloadlib(const char* aLibraryPath) {
		// Will compile for respective platform.
#if defined(_WIN32) || defined(_WIN64)
		size_t Length = strlen(aLibraryPath);
		wchar_t* pth = (wchar_t*)malloc((Length + 1) * sizeof(wchar_t));
		if ((pth != NULL) && (aLibraryPath != NULL)) {
			pth[Length] = '\0';
			mbstowcs(pth, aLibraryPath, Length); // maybe include size, but just char count
			this->Handle = reinterpret_cast<void*>(LoadLibraryW(pth));
		}
		free(pth);
#elif defined(__APPLE__) || defined(MACOSX)

#elif defined(__linux__) && !defined(__ANDROID__)
		this->Handle = dlopen(aLibraryPath, RTLD_LAZY);
#elif defined(__ANDROID__)

#endif
		return false;
	}

}
