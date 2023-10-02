#include <geodesuka/core/object_list.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <vector>

//#include <geodesuka/core/stage_t.h>

namespace geodesuka::core {

	object_list::object_list() {}

	object_list::object_list(int aObjectCount) {
		this->Handle.resize(aObjectCount);
	}

	object_list::object_list(object_t* aObject) {
		this->Handle.push_back(aObject);
	}

	object_list::object_list(int aObjectCount, object_t** aObjectList) {
		this->Handle.resize(aObjectCount);
		memcpy(this->Handle.data(), aObjectList, aObjectCount * sizeof(object_t*));
	}

	// Add access to parent stage.
	object_list object_list::gather_by(stage_t* aStage) const {
		object_list List;

		size_t StageCount = 0;
		for (size_t i = 0; i < this->Handle.size(); i++){
			if (this->Handle[i]->Stage == aStage) {
				StageCount += 1;
			}
		}

		if (StageCount == 0) return List;
		List.resize(StageCount);

		size_t j = 0;
		for (size_t i = 0; i < this->Handle.size(); i++) {
			if (this->Handle[i]->Stage == aStage) {
				List[j] = this->Handle[i];
				j += 1;
			}
		}

		return List;
	}

	object_t* object_list::find_by_name(const char* aName) const {
		for (int i = 0; i < count(); i++) {
			if (Handle[i]->Name == aName) {
				return Handle[i];
			}
		}
		return nullptr;
	}

}
