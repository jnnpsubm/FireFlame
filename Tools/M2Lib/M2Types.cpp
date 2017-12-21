#include "M2Types.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>
#include <algorithm>

M2Lib::CTriangle::CTriangle()
{
	TriangleIndex = 0;

	Vertices[0] = 0;
	Vertices[1] = 0;
	Vertices[2] = 0;
}

M2Lib::CTriangle::CTriangle(const CTriangle& Other)
{
	*this = Other;
}

M2Lib::CTriangle& M2Lib::CTriangle::operator = (const CTriangle& Other)
{
	TriangleIndex = Other.TriangleIndex;

	Vertices[0] = Other.Vertices[0];
	Vertices[1] = Other.Vertices[1];
	Vertices[2] = Other.Vertices[2];

	return *this;
}

M2Lib::CVertex::CVertex()
{
	Position[0] = 0.0f;
	Position[1] = 0.0f;
	Position[2] = 0.0f;

	BoneWeights[0] = 0;
	BoneWeights[1] = 0;
	BoneWeights[2] = 0;
	BoneWeights[3] = 0;

	BoneIndices[0] = 0;
	BoneIndices[1] = 0;
	BoneIndices[2] = 0;
	BoneIndices[3] = 0;

	Normal[0] = 0.0f;
	Normal[1] = 0.0f;
	Normal[2] = 0.0f;

	Texture[0] = 0.0f;
	Texture[1] = 0.0f;

	Texture2[0] = 0.0f;
	Texture2[1] = 0.0f;
}

M2Lib::CVertex::CVertex(const CVertex& Other)
{
	*this = Other;
}

M2Lib::CVertex& M2Lib::CVertex::operator = (const CVertex& Other)
{
	Position[0] = Other.Position[0];
	Position[1] = Other.Position[1];
	Position[2] = Other.Position[2];

	BoneWeights[0] = Other.BoneWeights[0];
	BoneWeights[1] = Other.BoneWeights[1];
	BoneWeights[2] = Other.BoneWeights[2];
	BoneWeights[3] = Other.BoneWeights[3];

	BoneIndices[0] = Other.BoneIndices[0];
	BoneIndices[1] = Other.BoneIndices[1];
	BoneIndices[2] = Other.BoneIndices[2];
	BoneIndices[3] = Other.BoneIndices[3];

	Normal[0] = Other.Normal[0];
	Normal[1] = Other.Normal[1];
	Normal[2] = Other.Normal[2];

	Texture[0] = Other.Texture[0];
	Texture[1] = Other.Texture[1];

	Texture2[0] = Other.Texture2[0];
	Texture2[1] = Other.Texture2[1];

	return *this;
}

// compares 2 vertices to see if they have the same position, bones, and texture coordinates. vertices between subsets that pass this test are most likely duplicates.
bool M2Lib::CVertex::CompareSimilar(CVertex& A, CVertex& B, bool CompareTextures, bool CompareBones, Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// compare position
	PositionalTolerance = PositionalTolerance * PositionalTolerance;
	if (PositionalTolerance > 0.0f)
	{
		Float32 Delta[3];
		Delta[0] = A.Position[0] - B.Position[0];
		Delta[1] = A.Position[1] - B.Position[1];
		Delta[2] = A.Position[2] - B.Position[2];
		Float32 Distance = ((Delta[0] * Delta[0]) + (Delta[1] * Delta[1]) + (Delta[2] * Delta[2]));
		if (Distance > PositionalTolerance)
		{
			return false;
		}
	}
	else
	{
		if ((A.Position[0] != B.Position[0]) || (A.Position[1] != B.Position[1]) || (A.Position[2] != B.Position[2]))
		{
			return false;
		}
	}

	// compare texture coordinates
	if (CompareTextures)
	{
		if ((A.Texture[0] != B.Texture[0]) || (A.Texture[1] != B.Texture[1]))
		{
			return false;
		}
	}

	// compare bones
	if (CompareBones)
	{
		// order independent comparison
		bool SameBones[4];
		SameBones[0] = false;
		SameBones[1] = false;
		SameBones[2] = false;
		SameBones[3] = false;
		for (UInt32 i = 0; i < 4; i++)
		{
			bool HasSameBone = false;
			for (UInt32 j = 0; j < 4; j++)
			{
				if (A.BoneIndices[i] == B.BoneIndices[j] && SameBones[j] == false)
				{
					SameBones[j] = true;
					break;
				}
			}
		}
		if (!(SameBones[0] && SameBones[1] && SameBones[2] && SameBones[3]))
		{
			return false;
		}
	}

	// compare normals
	// one way to visualize this is that NormalA defines a plane and NormalB defines a point
	// we want to determine what side of the PlaneA that the PointB lies on
	// it's just as simple as getting the dot product of the two vectors and checking the sign of the result
	// arc cosine the dot product of the vectors to get the angle between them
	if (AngularTolerance > 0.0f)
	{
		Float32 Dot = (A.Normal[0] * B.Normal[0]) + (A.Normal[1] * B.Normal[1]) + (A.Normal[2] * B.Normal[2]);
		if (acosf(Dot) > AngularTolerance)	// units are radians
		{
			return false;
		}
	}
	else
	{
		if ((A.Normal[0] != B.Normal[0]) || (A.Normal[1] != B.Normal[1]) || (A.Normal[2] != B.Normal[2]))
		{
			return false;
		}
	}

	return true;
}

const char* M2Lib::GetErrorText(EError Error)
{
	switch (Error)
	{
	case EError_OK:
		return "ok";

	case EError_FAIL:
		return "error: unspecified error.";

	case EError_PathTooLong:
		return "error: file path is too long.";

	case EError_FailedToLoadM2_NoFileSpecified:
		return "error: failed to load M2, no file was specified.";

	case EError_FailedToLoadM2_CouldNotOpenFile:
		return "error: failed to load M2, could not open file.";

	case EError_FailedToLoadM2_FileCorrupt:
		return "error: failed to load M2, file is corrupt.";

	case EError_FailedToLoadM2_VersionNotSupported:
		return "error: failed to load M2, format version is not supported.";

	case EError_FailedToExportM2I_NoFileSpecified:
		return "error: failed to export M2I, no file was specified.";

	case EError_FailedToExportM2I_CouldNotOpenFile:
		return "error: failed to export M2I, could not open file.";

	case EError_FailedToExportM2I_M2NotLoaded:
		return "error: failed to export M2I, no M2 was loaded.";

	case EError_FailedToImportM2I_NoFileSpecified:
		return "error: failed to import M2I, no file was specified.";

	case EError_FailedToImportM2I_CouldNotOpenFile:
		return "error: failed to import M2I, could not open file.";

	case EError_FailedToImportM2I_FileCorrupt:
		return "error: failed to import M2I, file is corrupt.";

	case EError_FailedToImportM2I_UnsupportedVersion:
		return "error: failed to import M2I, version number unsupported.";

	case EError_FailedToImportM2I_TooManyVertices:
		return "error: failed to import M2I, it has too many vertices (more than 65535).";

	case EError_FailedToImportM2I_SkinHasTooManyIndices:
		return "error: failed to import M2I, generated skin has too many indices (more than 65535).";

	case EError_FailedToSaveM2:
		return "error: failed to save M2, unspecified error.";

	case EError_FailedToSaveM2_NoFileSpecified:
		return "error: failed to save M2, no file was specified.";

	case EError_FailedToLoadSKIN_NoFileSpecified:
		return "error: failed to load SKIN, file not specified.";

	case EError_FailedToLoadSKIN_CouldNotOpenFile:
		return "error: failed to load SKIN, could not open file. make sure you put the M2 and skins in the same folder.";

	case EError_FailedToLoadSKIN_FileMissingOrCorrupt:
		return "error: failed to load SKIN, file is missing or corrupt.";

	case EError_FailedToSaveSKIN:
		return "error: faled to save SKIN, unspecified error.";

	case EError_FailedToSaveSKIN_NoFileSpecified:
		return "error: faled to save SKIN, no file was specified.";

	}

	return "error: unrecognized error";
}

M2Lib::SubmeshExtraData::SubmeshExtraData()
{
	ID = 0;
	M2IIndex = -1;
	MaterialOverride = -1;
	SourceMaterialIndex = -1;
}

const int M2Lib::BoundaryData::ExtraData::BoundingTriangleVertexMap[BOUNDING_TRIANGLE_COUNT * 3] =
{
	0, 1, 2,	// top
	0, 2, 3,

	6, 3, 2,	// right
	6, 2, 7,

	0, 4, 1,	// left
	0, 5, 4,

	2, 1, 7,	//	back
	1, 4, 7,

	0, 3, 5,	// front
	3, 6, 5,

	4, 5, 7,	// bottom
	5, 6, 7
};

void M2Lib::BoundaryData::Calculate(std::vector<CVertex> const& vertices)
{
	CenterMass.X = 0.0f;
	CenterMass.Y = 0.0f;
	CenterMass.Z = 0.0f;

	bool FirstPass = true;
	for (auto const& Vertex : vertices)
	{
		if (FirstPass)
		{
			BoundingMin.X = Vertex.Position[0];
			BoundingMin.Y = Vertex.Position[1];
			BoundingMin.Z = Vertex.Position[2];

			BoundingMax.X = Vertex.Position[0];
			BoundingMax.Y = Vertex.Position[1];
			BoundingMax.Z = Vertex.Position[2];

			FirstPass = false;
		}
		else
		{
			if (BoundingMin.X > Vertex.Position[0])
				BoundingMin.X = Vertex.Position[0];
			if (BoundingMin.Y > Vertex.Position[1])
				BoundingMin.Y = Vertex.Position[1];
			if (BoundingMin.Z > Vertex.Position[2])
				BoundingMin.Z = Vertex.Position[2];

			if (BoundingMax.X < Vertex.Position[0])
				BoundingMax.X = Vertex.Position[0];
			if (BoundingMax.Y < Vertex.Position[1])
				BoundingMax.Y = Vertex.Position[1];
			if (BoundingMax.Z < Vertex.Position[2])
				BoundingMax.Z = Vertex.Position[2];
		}

		CenterMass.X += Vertex.Position[0];
		CenterMass.Y += Vertex.Position[1];
		CenterMass.Z += Vertex.Position[2];
	}

	CenterMass = CenterMass / (Float32)vertices.size();

	BoundingCenter = (BoundingMin + BoundingMax) / 2.0f;

	BoundingRadius = 0.0f;
	for (auto const& Vertex : vertices)
	{
		C3Vector PositionLocal;
		Float32 Distance;

		PositionLocal.X = Vertex.Position[0] - BoundingCenter.X;
		PositionLocal.Y = Vertex.Position[1] - BoundingCenter.Y;
		PositionLocal.Z = Vertex.Position[2] - BoundingCenter.Z;

		Distance = PositionLocal.Length();
		if (Distance > BoundingRadius)
			BoundingRadius = Distance;
	}
}

M2Lib::BoundaryData::ExtraData M2Lib::BoundaryData::CalculateExtra() const
{
	ExtraData Extra;

	Extra.BoundingVertices[0] = BoundingMax;
	Extra.BoundingVertices[3] = Extra.BoundingVertices[2] = Extra.BoundingVertices[1] = Extra.BoundingVertices[0];
	Extra.BoundingVertices[1].X *= -1.0f;
	Extra.BoundingVertices[2].X *= -1.0f; Extra.BoundingVertices[2].Y *= -1.0f;
	Extra.BoundingVertices[3].Y *= -1.0f;

	Extra.BoundingVertices[7] = BoundingMin;
	Extra.BoundingVertices[4] = Extra.BoundingVertices[5] = Extra.BoundingVertices[6] = Extra.BoundingVertices[7];
	Extra.BoundingVertices[4].Y *= -1.0f;
	Extra.BoundingVertices[6].X *= -1.0f;
	Extra.BoundingVertices[5].X *= -1.0f; Extra.BoundingVertices[5].Y *= -1.0f;

	for (int i = 0; i < BOUNDING_TRIANGLE_COUNT; ++i)
	{
		Extra.BoundingNormals[i] = CalculateNormal(
			Extra.BoundingVertices[ExtraData::BoundingTriangleVertexMap[i * 3]],
			Extra.BoundingVertices[ExtraData::BoundingTriangleVertexMap[i * 3 + 1]],
			Extra.BoundingVertices[ExtraData::BoundingTriangleVertexMap[i * 3 + 2]]
			);
	}

	return Extra;
}

M2Lib::C3Vector& M2Lib::C3Vector::operator = (const C3Vector& Other)
{
	X = Other.X;
	Y = Other.Y;
	Z = Other.Z;

	return *this;
}

M2Lib::C3Vector M2Lib::C3Vector::operator + (const C3Vector& Other) const
{
	C3Vector result;
	result.X = X + Other.X;
	result.Y = Y + Other.Y;
	result.Z = Z + Other.Z;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::operator - (const C3Vector& Other) const
{
	C3Vector result;
	result.X = X - Other.X;
	result.Y = Y - Other.Y;
	result.Z = Z - Other.Z;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::operator * (Float32 Value) const
{
	C3Vector result;
	result.X = X * Value;
	result.Y = Y * Value;
	result.Z = Z * Value;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::operator / (Float32 Value) const
{
	C3Vector result;
	result.X = X / Value;
	result.Y = Y / Value;
	result.Z = Z / Value;

	return result;
}

M2Lib::C3Vector M2Lib::C3Vector::CrossProduct(C3Vector const& other) const
{
	C3Vector Product;

	Product.X = Y * other.Z - Z * other.Y;
	Product.Y = Z * other.X - X * other.Z;
	Product.Z = X * other.Y - Y * other.X;

	return Product;
}

Float32 M2Lib::C3Vector::Length() const
{
	return sqrtf(X * X + Y * Y + Z * Z);
}

void M2Lib::C3Vector::Normalize()
{
	Float32 length = Length();

	X = std::min(X / length, 1.0f);
	Y = std::min(Y / length, 1.0f);
	Z = std::min(Z / length, 1.0f);
}

M2Lib::C3Vector M2Lib::CalculateNormal(C3Vector v1, C3Vector v2, C3Vector v3)
{
	C3Vector V = v2 - v1;
	C3Vector W = v3 - v1;

	C3Vector N = V.CrossProduct(W);
	N.Normalize();
	return N;
}
