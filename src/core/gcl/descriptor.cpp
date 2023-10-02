#include <geodesuka/core/gcl/descriptor.h>

namespace geodesuka::core::gcl {

	descriptor::pool::pool() {

	}

	descriptor::pool::pool(uint32_t aDescriptorSetCount, const std::vector<VkDescriptorPoolSize>& aDescriptorPoolSizes) {

	}

	//descriptor::pool::pool(const std::vector<pipeline>& aPipelineList) {
	//	
	//}

	descriptor::pool::~pool() {

	}

	descriptor::set::set() {

	}

	descriptor::set::set(const set& aInput) {

	}

	descriptor::set::set(set&& aInput) noexcept {

	}
	
	descriptor::set::~set() {

	}

	void descriptor::set::zero_out() {

	}

	descriptor::table::table() {
		this->zero_out();
	}

	bool descriptor::table::resize(uint32_t aNewCount) {

		return false;
	}

	void descriptor::table::clear() {
		delete[] this->Set;
		this->zero_out();
	}

	void descriptor::table::zero_out() {
		this->Count = 0;
		this->Set = nullptr;
	}


	descriptor::descriptor(uint32_t aBindingIndex, type aType, shader::stage aStage) {
		this->Binding	= aBindingIndex;
		this->Type		= aType;
		this->Count		= 1u;
		this->Stage		= aStage;
		this->Sampler	= NULL;
	}

	descriptor::descriptor(uint32_t aBindingIndex, type aType, uint32_t aCount, shader::stage aStage) {
		this->Binding	= aBindingIndex;
		this->Type		= aType;
		this->Count		= aCount;
		this->Stage		= aStage;
		this->Sampler	= NULL;
	}

	descriptor::descriptor(uint32_t aBindingIndex, type aType, shader::stage aStage, VkSampler* aSampler) {
		this->Binding	= aBindingIndex;
		this->Type		= aType;
		this->Count		= 1u;
		this->Stage		= aStage;
		this->Sampler	= aSampler;
	}

	descriptor::descriptor(uint32_t aBindingIndex, type aType, uint32_t aCount, shader::stage aStage, VkSampler* aSampler) {
		this->Binding	= aBindingIndex;
		this->Type		= aType;
		this->Count		= aCount;
		this->Stage		= aStage;
		this->Sampler	= aSampler;
	}

	void descriptor::zero_out() {
		this->Binding	= 0;
		this->Type		= SAMPLER;
		this->Count		= 0;
		this->Stage		= shader::stage::UNKNOWN;
		this->Sampler	= NULL;
	}

}
