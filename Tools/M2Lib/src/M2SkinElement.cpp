#include "M2SkinElement.h"
#include "M2Element.h"
#include "M2Skin.h"
#include "M2.h"

bool M2Lib::M2SkinElement::CElement_SubMesh::Same(CElement_SubMesh const& other) const
{
	return &other == this || other.VertexStart == VertexStart;
}

#define GLOSS_SHADER_ID 32769

void M2Lib::M2SkinElement::CElement_SubMesh::MakeGlossy(M2Skin* pSkin, UInt32 GlossTextureId, TextureLookupRemap& LookupRemap)
{
	auto Materials = pSkin->Elements[EElement_Material].as<CElement_Material>();
	auto Submeshes = pSkin->Elements[EElement_SubMesh].as<CElement_SubMesh>();
	auto TextureLookup = pSkin->pM2->Elements[M2Element::EElement_TextureLookup].as<M2Element::CElement_TextureLookup>();

	// loop through all materials to find ones assigned to our submesh
	for (UInt32 i = 0; i < pSkin->Elements[EElement_Material].Count; ++i)
	{
		auto& material = Materials[i];
		if (!Submeshes[material.iSubMesh].Same(*this))
			continue;

		auto textureId = TextureLookup[material.iTexture].TextureIndex;
		int newLookup;

		if (LookupRemap.find(textureId) == LookupRemap.end())
		{
			// add two new lookups successively to use with op_count
			newLookup = pSkin->pM2->AddTextureLookup(textureId, true);
			pSkin->pM2->AddTextureLookup(GlossTextureId, true);
			LookupRemap[textureId] = newLookup;
		}
		else
			newLookup = LookupRemap[textureId];

		material.iTexture = newLookup;
		material.op_count = 2;
		material.shader_id = GLOSS_SHADER_ID;
	}
}
