#include <geodesuka/core/gcl/variable.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <geodesuka/core/math.h>

namespace geodesuka::core::gcl {

		// TODO: Maybe move this to a gconfig.h file so it can be shared with other objects?
	static struct {
		type::id TypeID;
		util::string TypeName;
		type::id DataTypeID;
		int ElementCount;
		int Rows;
		int Columns;
		size_t Size;
	} BuiltInType[] = {
		{	type::id::STRUCT		,		"struct"		,	type::id::STRUCT		,		0		  ,		0		,		0		,	0					}//,
		// {	type::id::UCHAR			,		"uchar"			,	type::id::UCHAR			,		1		  ,		1		,		1		,	sizeof(uchar)		},
		// {	type::id::USHORT		,		"ushort"		,	type::id::USHORT		,		1		  ,		1		,		1		,	sizeof(ushort)		},
		// {	type::id::UINT			,		"uint"			,	type::id::UINT			,		1		  ,		1		,		1		,	sizeof(uint)		},
		// {	type::id::CHAR			,		"byte"			,	type::id::CHAR			,		1		  ,		1		,		1		,	sizeof(char)		},
		// {	type::id::SHORT			,		"short"			,	type::id::SHORT			,		1		  ,		1		,		1		,	sizeof(short)		},
		// {	type::id::INT			,		"int"			,	type::id::INT			,		1		  ,		1		,		1		,	sizeof(int)			},
		// //{	type::id::HALF			,		"half"			,	type::id::HALF			,		1		  ,		1		,		1		,	sizeof(half)		},
		// {	type::id::FLOAT			,		"float"			,	type::id::FLOAT			,		1		  ,		1		,		1		,	sizeof(float)		},
		// {	type::id::DOUBLE		,		"double"		,	type::id::DOUBLE		,		1		  ,		1		,		1		,	sizeof(double)		},
		// {	type::id::UCHAR2		,		"ubyte2"		,	type::id::UCHAR			,		2		  ,		1		,		1		,	sizeof(uchar2)		},
		// {	type::id::UCHAR3		,		"ubyte3"		,	type::id::UCHAR			,		3		  ,		1		,		1		,	sizeof(uchar3)		},
		// {	type::id::UCHAR4		,		"ubyte4"		,	type::id::UCHAR			,		4		  ,		1		,		1		,	sizeof(uchar4)		},
		// {	type::id::USHORT2		,		"ushort2"		,	type::id::USHORT		,		2		  ,		1		,		1		,	sizeof(ushort2)		},
		// {	type::id::USHORT3		,		"ushort3"		,	type::id::USHORT		,		3		  ,		1		,		1		,	sizeof(ushort3)		},
		// {	type::id::USHORT4		,		"ushort4"		,	type::id::USHORT		,		4		  ,		1		,		1		,	sizeof(ushort4)		},
		// {	type::id::UINT2			,		"uint2"			,	type::id::UINT			,		2		  ,		1		,		1		,	sizeof(uint2)		},
		// {	type::id::UINT3			,		"uint3"			,	type::id::UINT			,		3		  ,		1		,		1		,	sizeof(uint3)		},
		// {	type::id::UINT4			,		"uint4"			,	type::id::UINT			,		4		  ,		1		,		1		,	sizeof(uint4)		},
		// {	type::id::CHAR2			,		"byte2"			,	type::id::CHAR			,		2		  ,		1		,		1		,	sizeof(char2)		},
		// {	type::id::CHAR3			,		"byte3"			,	type::id::CHAR			,		3		  ,		1		,		1		,	sizeof(char3)		},
		// {	type::id::CHAR4			,		"byte4"			,	type::id::CHAR			,		4		  ,		1		,		1		,	sizeof(char4)		},
		// {	type::id::SHORT2		,		"short2"		,	type::id::SHORT			,		2		  ,		1		,		1		,	sizeof(short2)		},
		// {	type::id::SHORT3		,		"short3"		,	type::id::SHORT			,		3		  ,		1		,		1		,	sizeof(short3)		},
		// {	type::id::SHORT4		,		"short4"		,	type::id::SHORT			,		4		  ,		1		,		1		,	sizeof(short4)		},
		// {	type::id::INT2			,		"int2"			,	type::id::INT			,		2		  ,		1		,		1		,	sizeof(int2)		},
		// {	type::id::INT3			,		"int3"			,	type::id::INT			,		3		  ,		1		,		1		,	sizeof(int3)		},
		// {	type::id::INT4			,		"int4"			,	type::id::INT			,		4		  ,		1		,		1		,	sizeof(int4)		},
		// //{	type::id::HALF2			,		"half2"			,	type::id::HALF			,		2		  ,		1		,		1		,	sizeof(half2)		},
		// //{	type::id::HALF3			,		"half3"			,	type::id::HALF			,		3		  ,		1		,		1		,	sizeof(half3)		},
		// //{	type::id::HALF4			,		"half4"			,	type::id::HALF			,		4		  ,		1		,		1		,	sizeof(half4)		},
		// {	type::id::FLOAT2		,		"float2"		,	type::id::FLOAT			,		2		  ,		1		,		1		,	sizeof(float2)		},
		// {	type::id::FLOAT3		,		"float3"		,	type::id::FLOAT			,		3		  ,		1		,		1		,	sizeof(float3)		},
		// {	type::id::FLOAT4		,		"float4"		,	type::id::FLOAT			,		4		  ,		1		,		1		,	sizeof(float4)		},
		// {	type::id::FLOAT2X2		,		"float2x2"		,	type::id::FLOAT			,		2*2		  ,		2		,		2		,	sizeof(float2x2)	},
		// {	type::id::FLOAT2X3		,		"float2x3"		,	type::id::FLOAT			,		2*3		  ,		3		,		2		,	sizeof(float2x3)	},
		// {	type::id::FLOAT2X4		,		"float2x4"		,	type::id::FLOAT			,		2*4		  ,		4		,		2		,	sizeof(float2x4)	},
		// {	type::id::FLOAT3X2		,		"float3x2"		,	type::id::FLOAT			,		3*2		  ,		2		,		3		,	sizeof(float3x2)	},
		// {	type::id::FLOAT3X3		,		"float3x3"		,	type::id::FLOAT			,		3*3		  ,		3		,		3		,	sizeof(float3x3)	},
		// {	type::id::FLOAT3X4		,		"float3x4"		,	type::id::FLOAT			,		3*4		  ,		4		,		3		,	sizeof(float3x4)	},
		// {	type::id::FLOAT4X2		,		"float4x2"		,	type::id::FLOAT			,		4*2		  ,		2		,		4		,	sizeof(float4x2)	},
		// {	type::id::FLOAT4X3		,		"float4x3"		,	type::id::FLOAT			,		4*3		  ,		3		,		4		,	sizeof(float4x3)	},
		// {	type::id::FLOAT4X4		,		"float4x4"		,	type::id::FLOAT			,		4*4		  ,		4		,		4		,	sizeof(float4x4)	}
	};

	type::id type::id_of_string(const char* aTypeName) {
		size_t n = (sizeof(BuiltInType) / sizeof(BuiltInType[0]));
		for (size_t i = 0; i < n; i++) {
			if (BuiltInType[i].TypeName == aTypeName) {
				return BuiltInType[i].TypeID;
			}
		}
		return id::UNKNOWN;
	}

	const char* type::name_of(id aID) {
		size_t n = (sizeof(BuiltInType) / sizeof(BuiltInType[0]));
		for (size_t i = 0; i < n; i++) {
			if (BuiltInType[i].TypeID == aID) {
				return BuiltInType[i].TypeName.ptr();
			}
		}
		return NULL;
	}

	size_t type::size_of(id aID) {
		size_t n = (sizeof(BuiltInType) / sizeof(BuiltInType[0]));
		for (size_t i = 0; i < n; i++) {
			if (aID == BuiltInType[i].TypeID) {
				return BuiltInType[i].Size;
			}
		}
		return 0;
	}

	type::type() {
		this->Master = nullptr;
		this->zero_out();
	}

	type::type(type::id aID) {
		this->Master = nullptr;
		this->zero_out();
		assert(!this->create(aID, NULL, 0, NULL));
	}

	type::type(type::id aID, const char* aName) {
		this->Master = nullptr;
		this->zero_out();
		assert(!this->create(aID, aName, 0, NULL));
	}

	type::type(type::id aID, const char* aName, int aMemberCount, variable* aMemberList) {
		this->Master = nullptr;
		this->zero_out();
		assert(!this->create(aID, aName, aMemberCount, aMemberList));
	}

	type::type(const type& aInput) {
		this->Master = nullptr;
		this->zero_out();
		*this = aInput;
	}

	type::type(type&& aInput) noexcept {
		this->Master		= nullptr;
		this->ID			= aInput.ID;
		this->MemberCount	= aInput.MemberCount;
		this->Member		= aInput.Member;
		this->Name			= aInput.Name;
		aInput.zero_out();
	}

	type::~type() {
		this->clear();
	}

	type& type::operator=(const type& aRhs) {
		if (this == &aRhs) return *this;
		this->ID			= aRhs.ID;
		assert(!this->resize(aRhs.MemberCount));
		for (int i = 0; i < this->MemberCount; i++) {
			if (this->Master != nullptr) {
				this->Member[i].Root = this->Master->Root;
			}
			this->Member[i].Parent	= this->Master;
			this->Member[i]			= aRhs.Member[i];
		}
		this->Name			= aRhs.Name;
		return *this;
	}

	type& type::operator=(type&& aRhs) noexcept {
		this->clear();
		this->ID			= aRhs.ID;
		this->MemberCount	= aRhs.MemberCount;
		this->Member		= aRhs.Member;
		this->Name			= aRhs.Name;
		aRhs.zero_out();
		return *this;
	}

	size_t type::size() const {
		size_t TotalSize = 0;
		if (this->ID == id::STRUCT) {
			for (int i = 0; i < this->MemberCount; i++) {
				TotalSize += this->Member[i].size();
			}
		}
		else {
			TotalSize = type::size_of(this->ID);
		}
		return TotalSize;
	}

	bool type::resize(int aNewMemberCount) {
		if (this->MemberCount == aNewMemberCount) return false;
		if (aNewMemberCount > 0) {
			variable* nptr = new variable[aNewMemberCount];
			if (nptr == nullptr) return true;
			for (int i = 0; i < aNewMemberCount; i++) {
				nptr[i].Parent = this->Master;
				if (i < this->MemberCount) {
					nptr[i] = this->Member[i];
				}
			}
			this->MemberCount = aNewMemberCount;
			this->Member = nptr;
		}
		else {
			delete[] this->Member;
			this->Member = nullptr;
			this->MemberCount = 0;
		}
		return false;
	}

	type& type::operator=(type::id aID) {
		*this = type(aID);
		return *this;
	}

	void type::clear() {
		delete[] this->Member;
		this->zero_out();
	}

	bool type::create(type::id aID, const char* aName, int aMemberCount, variable* aMemberList) {
		if (this->resize(aMemberCount)) return true;
		this->ID		= aID;
		for (int i = 0; i < aMemberCount; i++) {
			this->Member[i].Parent = this->Master;
			this->Member[i] = aMemberList[i];
		}
		if (aID == id::STRUCT) {
			this->Name = aName;
		}
		else {
			this->Name = type::name_of(aID);
		}
		return false;
	}

	void type::zero_out() {
		this->ID				= type::id::UNKNOWN;
		this->MemberCount		= 0;
		this->Member			= nullptr;
		this->Name				= "";
	}

	variable::variable() {
		this->Root			= this;
		this->Parent		= nullptr;
		this->Type.Master	= this;
		this->zero_out();
	}

	variable::variable(type aType, const char* aName) {
		this->Root			= this;
		this->Parent		= nullptr;
		this->Type.Master	= this;
		this->zero_out();
		assert(!this->create(aType, aName, 0, NULL));
	}

	variable::variable(type aType, const char* aName, int aArraySize) {
		this->Root			= this;
		this->Parent		= nullptr;
		this->Type.Master	= this;
		this->zero_out();
		int ArrayList[1] = { aArraySize };
		assert(!this->create(aType, aName, 1, ArrayList));
	}

	variable::variable(type aType, const char* aName, int aDimensionCount, int* aDimensionList) {
		this->Root			= this;
		this->Parent		= nullptr;
		this->Type.Master	= this;
		this->zero_out();
		assert(!this->create(aType, aName, aDimensionCount, aDimensionList));
	}

	variable::variable(const variable& aInput) {
		this->Root			= this;
		this->Parent		= nullptr;
		this->Type.Master	= this;
		this->zero_out();
		*this				= aInput;
	}

	variable::variable(variable&& aInput) noexcept {
		this->Root				= this;
		this->Parent			= nullptr;
		this->Type.Master		= this;
		this->Type				= aInput.Type;
		this->Name				= aInput.Name;
		this->DimensionCount	= aInput.DimensionCount;
		this->Dimension			= aInput.Dimension;
		aInput.zero_out();
	}

	variable::~variable() {
		this->clear();
	}

	variable& variable::operator[](int aIndex) {
		return this->Type.Member[aIndex];
	}

	variable& variable::operator[](const char* aFieldName) {
		int Index = -1;
		for (int i = 0; i < this->Type.MemberCount; i++) {
			if (Type.Member[i].Name == aFieldName) {
				Index = i;
				break;
			}
		}
		return this->Type.Member[Index];
	}

	variable& variable::operator=(const variable& aRhs) {
		if (this == &aRhs) return *this;
		assert(!this->resize(aRhs.DimensionCount));
		this->Type.Master		= this;
		this->Type.ID			= aRhs.Type.ID;
		assert(!this->Type.resize(aRhs.Type.MemberCount));
		for (int i = 0; i < this->Type.MemberCount; i++) {
			this->Type.Member[i].Root		= this->Root;
			this->Type.Member[i].Parent		= this;
			this->Type.Member[i]			= aRhs.Type.Member[i];
		}
		this->Type.Name			= aRhs.Type.Name;
		this->Name				= aRhs.Name;
		memcpy(this->Dimension, aRhs.Dimension, aRhs.DimensionCount * sizeof(int));
		return *this;
	}

	variable& variable::operator=(variable&& aRhs) noexcept {
		this->clear();
		this->Type				= aRhs.Type;
		this->Name				= aRhs.Name;
		this->DimensionCount	= aRhs.DimensionCount;
		this->Dimension			= aRhs.Dimension;
		aRhs.zero_out();
		return *this;
	}

	size_t variable::size() const {
		size_t TotalSize = this->Type.size();
		for (int i = 0; i < this->DimensionCount; i++) {
			TotalSize *= this->Dimension[i];
		}
		return TotalSize;
	}

	size_t variable::offset() const {
		if (this->Root->exists(this)) {
			return this->offset(this->Root, this);
		}
		else {
			return 0;
		}
	}

	util::string variable::to_string(bool aSizesAndOffsets) const {
		static int DepthCounter = -1;
		util::string Return		= "";
		util::string DepthShift = "";
		util::string TabShift	= "\t\t";
		util::string LineShift	= "                    |";
		util::string OtherShift = "";
		util::string Header		= "Size:     Offset:   \n";
		util::string Size		= "";
		util::string Offset		= "";
		DepthCounter += 1;
		if (aSizesAndOffsets && (DepthCounter == 0)) {
			Return += Header;
		}

		for (int i = 0; i < DepthCounter; i++) {
			DepthShift += "   ";
		}

		{
			char Str[64];
			memset(Str, 0x00, sizeof(Str));
			sprintf(Str, "%zu", this->size());
			Size = Str;
			memset(Str, 0x00, sizeof(Str));
			sprintf(Str, "%zu", this->offset());
			Offset = Str;

			OtherShift.resize(21);
			memset((void*)OtherShift.ptr(), ' ', 21 * sizeof(char));
			OtherShift[20] = '|';
			OtherShift[21] = '\0';
			for (int i = 0; i < Size.size(); i++) {
				OtherShift[i] = Size[i];
			}

			for (int i = 0; i < Offset.size(); i++) {
				OtherShift[i + 10/* + DepthCounter*/] = Offset[i];
			}

		}

		if (this->Type.ID == type::id::STRUCT) {
			if (aSizesAndOffsets) {
				Return += LineShift;
			}
			Return += DepthShift + util::string("struct ") + this->Type.Name + " {\n";
			for (int i = 0; i < this->Type.MemberCount; i++) {
				Return += this->Type.Member[i].to_string(aSizesAndOffsets) + "\n";
			}
			if (aSizesAndOffsets) {
				Return += OtherShift;
			}
			Return += DepthShift + "} ";
		}
		else {
			if (aSizesAndOffsets) {
				Return += OtherShift;
			}
			Return += (DepthShift + this->Type.Name + " ");
		}
		Return += this->Name;
		for (int i = 0; i < this->DimensionCount; i++) {
			char NumString[64];
			sprintf(NumString, "%d", this->Dimension[i]);
			Return += util::string("[") + NumString + "]";
		}
		// Array stuff here.
		Return += ";";
		DepthCounter -= 1;
		return Return;
	}

	void variable::set_root(variable* aRoot) {
		this->Root = aRoot;
		for (int i = 0; i < this->Type.MemberCount; i++) {
			this->Type.Member[i].set_root(aRoot);
		}
	}

	size_t variable::offset(const variable* aTree, const variable* aVariable) const {
		size_t TotalOffset = 0;
		int DetectedLocation = -1;
		if (aTree == aVariable) return TotalOffset;
		for (int i = 0; i < aTree->Type.MemberCount; i++) {
			if (aTree->Type.Member[i].exists(aVariable)) {
				DetectedLocation = i;
				break;
			}

		}

		// Calculate Total Offsets
		for (int i = 0; i < DetectedLocation; i++) {
			TotalOffset += aTree->Type.Member[i].size();
		}

		TotalOffset += this->offset(&aTree->Type.Member[DetectedLocation], aVariable);
		return TotalOffset;
	}

	bool variable::create(type aType, const char* aName, int aDimensionCount, int* aDimensionList) {
		if (this->resize(aDimensionCount)) return true;
		this->Type.Master		= this;
		this->Type				= aType;
		this->Name				= aName;
		if ((aDimensionCount > 0) && (aDimensionList != NULL)) {
			this->DimensionCount = aDimensionCount;
			memcpy(this->Dimension, aDimensionList, aDimensionCount * sizeof(int));
		}
		return false;
	}

	bool variable::exists(const variable* aVariable) const {
		bool Exists = (this == aVariable);
		if (Exists) return true;
		for (int i = 0; i < this->Type.MemberCount; i++) {
			Exists |= this->Type.Member[i].exists(aVariable);
			if (Exists) break;
		}
		return Exists;
	}

	bool variable::resize(int aNewDimensionCount) {
		if (this->DimensionCount == aNewDimensionCount) return false;
		if (aNewDimensionCount > 0) {
			size_t TypeSize = sizeof(int);
			void* nptr = NULL;
			if (this->Dimension == NULL) {
				nptr = malloc(aNewDimensionCount * TypeSize);
			}
			else {
				nptr = realloc(this->Dimension, aNewDimensionCount * TypeSize);
			}
			if (nptr == NULL) return true;
			if (aNewDimensionCount > this->DimensionCount) {
				memset(nptr, 0x00, (aNewDimensionCount - this->DimensionCount) * TypeSize);
			}
			this->DimensionCount	= aNewDimensionCount;
			this->Dimension			= (int*)nptr;
		}
		else {
			free(this->Dimension);
			this->Dimension = NULL;
			this->DimensionCount = 0;
		}
		return false;
	}

	void variable::clear() {
		free(this->Dimension);
		this->zero_out();
	}

	void variable::zero_out() {
		//this->Root				= this;
		//this->Parent			= nullptr;
		this->Type				= type();
		this->Name				= "";
		this->DimensionCount	= 0;
		this->Dimension			= NULL;
	}

}