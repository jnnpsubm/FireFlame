#pragma once

#include "DataElement.h"
#include "M2SkinElement.h"
#include "M2Types.h"
#include <vector>
#include <map>

#define VERTEX_PER_TRIANGLE 3
#define BONES_PER_VERTEX 4
#define BONES_PER_TRIANGLE (VERTEX_PER_TRIANGLE * BONES_PER_VERTEX)

namespace M2Lib
{
	class M2;

	namespace M2Element
	{
		class CElement_Texture;
	}

	//using namespace M2SkinElement;

	class M2Skin
	{
		friend class M2;

	public:

#pragma pack(push,1)
		// memory mapped header
		class CM2SkinHeader
		{
		public:
			Char8 ID[4];	// 'SKIN'

			UInt32 nVertex;
			UInt32 oVertex;

			UInt32 nTriangleIndex;
			UInt32 oTriangleIndex;

			UInt32 nBoneIndices;
			UInt32 oBoneIndices;

			UInt32 nSubMesh;
			UInt32 oSubMesh;

			UInt32 nMaterial;
			UInt32 oMaterial;

			UInt32 Unknown1;	// non-zero only in ChickenMount01.skin

			UInt32 nFlags;
			UInt32 oFlags;

			UInt32 Unknown2;	// non-zero only in ChickenMount01.skin
			UInt32 Unknown3;	// non-zero only in ChickenMount01.skin
		};

#pragma pack(pop)

	public:
		M2Skin(M2* pM2In)
			: pM2(pM2In)
		{
		}

		~M2Skin()
		{
		}


	public:
		Char16 _FileName[1024];

		CM2SkinHeader Header;
		DataElement Elements[M2SkinElement::EElement__Count__];

		std::map<int, SubmeshExtraData const*> ExtraDataBySubmeshIndex;

		// pointer to M2 that this skin belongs to.
		M2* pM2;


	public:
		// loads an M2 skin from a file.
		EError Load(const Char16* FileName);
		// saves this M2 skin to a file.
		EError Save(const Char16* FileName);

		void BuildVertexBoneIndices();
		void BuildBoundingData();
		void BuildMaxBones();

		// copies materials from sub meshes in another skin to equivalent sub meshes in this skin.
		void CopyMaterials(M2Skin* pOther);

		//
		void SortSubMeshes();

		bool PrintInfo();

		// returns sub mesh with ID using CenterBounds to narrow search. some times there are multiple sub meshes with the same ID, so we can narrow our search to whatever sub mesh lies closest to CenterBounds.
		M2SkinElement::CElement_SubMesh* GetSubMesh(SubmeshExtraData const& TargetSubMeshData, SInt32& SubMeshIndexOut);

		// gathers list of materials that affect a given sub mesh within this skin.
		void GetSubMeshMaterials(UInt32 SubMeshIndex, std::vector< M2SkinElement::CElement_Material* >& Result);

		// gathers list of flags that affect a given sub mesh within this skin.
		void GetSubMeshFlags(UInt32 SubMeshIndex, std::vector< M2SkinElement::CElement_Flags* >& Result);

		void MakeGlossy(Char8 const* szGlossTexturePath, std::vector<UInt32> const& MeshIndexes, M2SkinElement::TextureLookupRemap& LookupRemap);
		void MakeGlossy(UInt32 GlossTextureId, std::vector<UInt32> const& MeshIndexes, M2SkinElement::TextureLookupRemap& LookupRemap);

		void CopyMaterial(UInt32 SrcMeshIndex, UInt32 DstMeshIndex);

		class MeshInfo
		{
		public:
			MeshInfo() : ID(0), pSubMesh(NULL)
			{
			}

			struct TextureInfo
			{
				M2Element::CElement_Texture* pTexture;
				std::string Name;
			};

			UInt32 ID;
			std::string Description;

			std::vector<M2SkinElement::CElement_Material*> Materials;
			std::vector<TextureInfo> Textures;
			M2SkinElement::CElement_SubMesh* pSubMesh;
		};
		std::vector<MeshInfo> GetMeshInfo();

	private:
		void m_LoadElements_CopyHeaderToElements();
		void m_LoadElements_FindSizes(UInt32 FileSize);
		void m_SaveElements_FindOffsets();
		void m_SaveElements_CopyElementsToHeader();

		static SInt32 m_ReverseBoneLookup(UInt8 BoneID, UInt16* BoneLookupTable, UInt32 BoneLookupTableLength)
		{
			for (UInt32 i = 0; i < BoneLookupTableLength; i++)
			{
				if (BoneLookupTable[i] == BoneID)
					return i;
			}

			assert(false && "Bone lookup failed");
			return 0;
		}

	};




}
