#ifndef GEODESUKA_CORE_IO_IMAGE_H
#define GEODESUKA_CORE_IO_IMAGE_H

#include "file.h"

#include "../math.h"

namespace geodesuka::core::io {

    class image : public io::file {
    public:

    private:

    	// static bool initialize();
		// static void terminate();

        math::vec4<uint> Resolution;

    };

}

#endif // !GEODESUKA_CORE_IO_IMAGE_H