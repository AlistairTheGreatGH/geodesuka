#include <vector>

#include <geodesuka/engine.h>

// Used for internal engine debugging.
#include <geodesuka/builtin/app/unit_test.h>

using namespace geodesuka::builtin::app;
using namespace geodesuka::core;

// Using entry point for app.
int main(int aCmdArgCount, char* aCmdArgList[]) {
	
	// Instance Layers & Extensions.
	std::vector<const char*> Layer = {
		"VK_LAYER_KHRONOS_validation"
	};

	std::vector<const char*> Ext = {
		VK_KHR_DISPLAY_EXTENSION_NAME
	};

	geodesuka::engine Engine(aCmdArgCount, (const char**)aCmdArgList, Layer.size(), Layer.data(), 0, NULL);
	if (Engine.is_ready()) {
		try {
			unit_test UnitTest(&Engine);
			Engine.run(&UnitTest);
		} catch (geodesuka::core::util::log::message& aLastMessage) {
			// Critical Error (Typically OOM condition)
			Engine << "CRITICAL ERROR:\n";
			Engine << aLastMessage;

			// Log Dump.

			// Print to Terminal.

			// Return Error Code.
			return Engine.return_value();
		} catch (...) {
			// Any runtime error will 
			return Engine.return_value();
		}
	}
	return Engine.return_value();
}
