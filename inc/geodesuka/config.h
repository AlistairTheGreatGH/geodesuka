#pragma once
#ifndef GEODESUKA_CONFIG_H
#define GEODESUKA_CONFIG_H

#define GEODESUKA_ENGINE_VERSION_MAJOR 0
#define GEODESUKA_ENGINE_VERSION_MINOR 1
#define GEODESUKA_ENGINE_VERSION_PATCH 5

#define GEODESUKA_ENGINE_VERSION_BIRTH_DATE 20221102

// geodesuka/config.h is responsible for build time specific
// options of the geodesuka engine. Depending on the platform
// target, some features may not exist on a particular platform.
//

#define GEODESUKA_UNIT_TEST_ENABLED

// Disables Terminal Window, for windows only
#if defined(_WIN32) || defined(_WIN64)
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif


namespace geodesuka {
	namespace core {
		class app;
	}
	class engine;
}

#endif // !GEODESUKA_CONFIG_H
