#pragma once

#include <vector>
#include "BaseTypes.h"
#include "M2Types.h"
#include "M2Skin.h"

namespace M2Lib
{
	class M2I
	{
	public:
		static const UInt32 Signature_M2I0 = MakeFourCC('M', '2', 'I', '0');

		class CSubMesh
		{
		public:
			// this subset's ID.
			UInt16 ID;
			UInt16 Level;

			// vertices that make up this subset, as indices into the global vertex list.
			std::vector< UInt16 > Indices;
			// triangles that make up this subset, as indices into the global vertex list.
			std::vector< CTriangle > Triangles;

			SubmeshExtraData ExtraData;
		};

	public:
		// the global vertex list
		std::vector< CVertex > VertexList;

		// list of subsets in this M2I.
		std::vector< CSubMesh* > SubMeshList;

	public:
		M2I() { }

		EError Load(Char16* FileName, M2* pM2, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras);

		~M2I()
		{
			for (UInt32 i = 0; i < SubMeshList.size(); i++)
			{
				delete SubMeshList[i];
			}
		}
	};
}
