#pragma once
#ifndef GEODESUKA_CORE_GCL_VARIABLE_H
#define GEODESUKA_CORE_GCL_VARIABLE_H

#include "../util/string.h"

#include "../math.h"

namespace geodesuka::core::gcl {

	class variable;

	class type {
	public:

		friend class variable;

		enum class id {
			UNKNOWN = -1,
			// Composite Structure Type
			STRUCT,
			// Number Types
			UCHAR,
			USHORT,
			UINT,
			CHAR,
			SHORT,
			INT,
			//HALF,
			FLOAT,
			DOUBLE,
			// Vector Types
			UCHAR2,
			UCHAR3,
			UCHAR4,
			USHORT2,
			USHORT3,
			USHORT4,
			UINT2,
			UINT3,
			UINT4,
			CHAR2,
			CHAR3,
			CHAR4,
			SHORT2,
			SHORT3,
			SHORT4,
			INT2,
			INT3,
			INT4,
			//HALF2,
			//HALF3,
			//HALF4,
			FLOAT2,
			FLOAT3,
			FLOAT4,
			// Matrix Types
			FLOAT2X2,
			FLOAT2X3,
			FLOAT2X4,
			FLOAT3X2,
			FLOAT3X3,
			FLOAT3X4,
			FLOAT4X2,
			FLOAT4X3,
			FLOAT4X4
		};

		static id id_of_string(const char* aTypeName);
		static const char* name_of(id aID);
		static size_t size_of(id aID);

		variable*			Master;
		id					ID;
		int					MemberCount;
		variable*			Member;
		util::string		Name;

		type();
		type(type::id aID);
		type(type::id aID, const char* aName);
		type(type::id aID, const char* aName, int aMemberCount, variable* aMemberList);
		type(const type& aInput);
		type(type&& aInput) noexcept;
		~type();

		type& operator=(type::id aID);
		type& operator=(const type& aRhs);
		type& operator=(type&& aRhs) noexcept;

		size_t size() const;

		bool resize(int aNewMemberCount);

		void clear();

	private:

		bool create(type::id aID, const char* aName, int aMemberCount, variable* aMemberList);

		void zero_out();

	};

	class variable {
	public:

		friend class type;

		variable*			Root;
		variable*			Parent;
		type				Type;
		util::string		Name;
		int					DimensionCount;
		int*				Dimension;

		variable();
		variable(type aType, const char* aName);
		variable(type aType, const char* aName, int aArraySize);
		variable(type aType, const char* aName, int aDimensionCount, int* aDimensionList);
		variable(const variable& aInput);
		variable(variable&& aInput) noexcept;
		~variable();

		variable& operator[](int aIndex);
		variable& operator[](const char* aFieldName);
		variable& operator=(const variable& aRhs);
		variable& operator=(variable&& aRhs) noexcept;

		size_t size() const;

		size_t offset() const;

		// Leaf Count
		//int count() const;

		util::string to_string(bool aSizesAndOffsets) const;

		void set_root(variable* aRoot);

		bool exists(const variable* aVariable) const;

		bool resize(int aNewDimensionCount);

		void clear();

	private:

		size_t offset(const variable* aTree, const variable* aVariable) const;

		bool create(type aType, const char* aName, int aDimensionCount, int* aDimensionList);

		void zero_out();

	};

}

#endif // !GEODESUKA_CORE_GCL_VARIABLE_H
