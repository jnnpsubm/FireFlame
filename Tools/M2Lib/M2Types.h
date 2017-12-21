#pragma once

#include "BaseTypes.h"
#include <vector>
#include <assert.h>

namespace M2Lib
{

#pragma pack(push,1)

	// used when importing an M2I and building an M2. each triangle is given an index in addition to the 3 vertices. the index is used for quick lookup when dealing out triangles between bone partitions when building skins.
	class CTriangle
	{
	public:
		UInt32 TriangleIndex;			// this triangle's global index, or unique ID.
		UInt16 Vertices[3];		// indices into the global vertex list.

	public:
		CTriangle();
		CTriangle(const CTriangle& Other);
		CTriangle& operator = (const CTriangle& Other);

	};

	// vertices are in this format.
	class CVertex
	{
	public:
		Float32 Position[3];
		UInt8 BoneWeights[4];
		UInt8 BoneIndices[4];
		Float32 Normal[3];
		Float32 Texture[2];
		Float32 Texture2[2];


	public:
		CVertex();
		CVertex(const CVertex& Other);
		CVertex& operator = (const CVertex& Other);
		static bool CompareSimilar(CVertex& A, CVertex& B, bool CompareTextures, bool CompareBones, Float32 PositionalTolerance, Float32 AngularTolerance);	// compares 2 vertices to see if they have the same position, bones, and texture coordinates. vertices between subsets that pass this test are most likely duplicates.

	};

	//
	struct SVolume
	{
		Float32 Min[3];
		Float32 Max[3];
		Float32 Radius;
	};

	struct M2Array
	{
		SInt32 Count;
		UInt32 Offset;

		void Shift(SInt32 Delta)
		{
			if (Count/* && Offset*/)
			{
				//assert(Offset > 0);
				Offset += Delta;
			}
			else
				Offset = 0;
		}
	};

	class C2Vector
	{
	public:
		Float32 X;
		Float32 Y;
	};

	class C3Vector
	{
	public:
		Float32 X;
		Float32 Y;
		Float32 Z;

		C3Vector() : X(0.0f), Y(0.0f), Z(0.0f) { }
		C3Vector(Float32 _X, Float32 _Y, Float32 _Z) : X(_X), Y(_Y), Z(_Z) {}
		C3Vector& operator = (const C3Vector& Other);
		C3Vector operator + (const C3Vector& Other) const;
		C3Vector operator - (const C3Vector& Other) const;
		C3Vector operator * (Float32 Value) const;
		C3Vector operator / (Float32 Value) const;

		C3Vector CrossProduct(C3Vector const& other) const;
		void Normalize();
		Float32 Length() const;
	};

	C3Vector CalculateNormal(C3Vector v1, C3Vector v2, C3Vector v3);

#pragma pack(pop)

	enum EError
	{
		EError_OK = 0,
		EError_FAIL = 1,

		EError_PathTooLong,

		EError_FailedToLoadM2_NoFileSpecified,
		EError_FailedToLoadM2_CouldNotOpenFile,
		EError_FailedToLoadM2_FileCorrupt,
		EError_FailedToLoadM2_VersionNotSupported,

		EError_FailedToExportM2I_NoFileSpecified,
		EError_FailedToExportM2I_CouldNotOpenFile,
		EError_FailedToExportM2I_M2NotLoaded,

		EError_FailedToImportM2I_NoFileSpecified,
		EError_FailedToImportM2I_CouldNotOpenFile,
		EError_FailedToImportM2I_FileCorrupt,
		EError_FailedToImportM2I_UnsupportedVersion,
		EError_FailedToImportM2I_TooManyVertices,
		EError_FailedToImportM2I_SkinHasTooManyIndices,

		EError_FailedToSaveM2,
		EError_FailedToSaveM2_NoFileSpecified,

		EError_FailedToLoadSKIN_NoFileSpecified,
		EError_FailedToLoadSKIN_CouldNotOpenFile,
		EError_FailedToLoadSKIN_FileMissingOrCorrupt,

		EError_FailedToSaveSKIN,
		EError_FailedToSaveSKIN_NoFileSpecified,
	};

	const char* GetErrorText(EError Error);

	struct BoundaryData
	{
		struct ExtraData
		{
			#define BOUNDING_VERTEX_COUNT 8
			#define BOUNDING_TRIANGLE_COUNT 12
			
			static const int BoundingTriangleVertexMap[BOUNDING_TRIANGLE_COUNT * 3];

			C3Vector BoundingVertices[BOUNDING_VERTEX_COUNT];
			C3Vector BoundingNormals[BOUNDING_TRIANGLE_COUNT];
		};

		C3Vector BoundingMin;
		C3Vector BoundingMax;
		C3Vector BoundingCenter;
		Float32 BoundingRadius;

		C3Vector CenterMass;

		void Calculate(std::vector<CVertex> const& vertices);
		ExtraData CalculateExtra() const;
	};

	struct SubmeshExtraData
	{
		SubmeshExtraData();

		UInt32 ID;
		UInt32 M2IIndex;
		std::string Description;
		BoundaryData Boundary;

		std::string GlossTextureName;
		std::string CustomTextureName;
		int MaterialOverride;
		int SourceMaterialIndex;
	};
}
