#include <geodesuka/core/hid/keyboard.h>

namespace geodesuka {
	namespace core {
		namespace hid {

			keyboard::keyboard() {

			}

			keyboard::~keyboard() {

			}

			bool keyboard::operator[](int keyID) {
				if ((Action[keyID] == KEY_PRESS) || (Action[keyID] == KEY_REPEAT)) {
					return true;
				}
				else {
					return false;
				}
			}

		}
	}
}
