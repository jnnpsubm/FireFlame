#include "M2SkinBuilder.h"
#include "M2I.h"
#include "M2.h"
#include "M2Skin.h"
#include <cstring>
#include <iostream>


extern int g_Verbose;

bool M2Lib::M2SkinBuilder::CBonePartition::AddTriangle(CVertex* GlobalVertexList, CTriangle* pTriangle)
{
	// put all the bones used by the input triangle into a 1D list for easy iteration
	SInt16 TriBones[BONES_PER_TRIANGLE];

	for (int i = 0; i < VERTEX_PER_TRIANGLE; ++i)
	{
		UInt32 TotalWeight = 0;
		SInt16* TriBonesSub = &TriBones[i * BONES_PER_VERTEX];
		CVertex* pTriVertex = &GlobalVertexList[pTriangle->Vertices[i]];

		for (int j = 0; j < BONES_PER_VERTEX; ++j)
		{
			TriBonesSub[j] = pTriVertex->BoneWeights[j] ? pTriVertex->BoneIndices[j] : -1;
			TotalWeight += pTriVertex->BoneWeights[j];
		}

		//assert(TotalWeight == 255);
	}

	// count the number of bones used by the triangle
	UInt32 ExtraBones = 0;
	for (UInt32 i = 0; i < BONES_PER_TRIANGLE; ++i)
	{
		if (TriBones[i] != -1)
			++ExtraBones;
	}

	// count the number of bones from the triangle that already exist in this bone partition
	for (UInt32 i = 0; i < BONES_PER_TRIANGLE; ++i)
	{
		if (TriBones[i] != -1)
			if (HasBone(TriBones[i], NULL))
				--ExtraBones;
	}

	if (ExtraBones != 0)
	{
		// there are some bones from the input triangle that are not contained in this bone partition
		UInt32 ExtraRoom = (*pBoneLoD) - Bones.size();
		if (ExtraBones > ExtraRoom)
		{
			// there isn't enough room for them
			return false;
		}

		// there's room for them
		for (UInt32 i = 0; i < BONES_PER_TRIANGLE; ++i)
		{
			if (TriBones[i] != -1)
			{
				if (!HasBone(TriBones[i], NULL))
				{
					// add the bone that isn't already contained
					Bones.push_back(TriBones[i]);
				}
			}
		}
	}

	// add triangle index and triangle to the triangle map
	assert(!HasTriangle(pTriangle->TriangleIndex));
	TrianglesMap[pTriangle->TriangleIndex] = pTriangle;

	// triangle successfully added
	return true;
}


bool M2Lib::M2SkinBuilder::CBonePartition::HasBone(UInt16 BoneIndex, UInt16* pIndexOut)
{
	UInt32 Count = Bones.size();
	for (UInt32 i = 0; i < Count; i++)
	{
		if (Bones[i] == BoneIndex)
		{
			if (pIndexOut)
				*pIndexOut = i;
			return true;
		}
	}

	return false;
}


bool M2Lib::M2SkinBuilder::CBonePartition::HasTriangle(UInt32 TriangleIndex)
{
	return (TrianglesMap.find(TriangleIndex) != TrianglesMap.end());
}


M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::CSubsetPartition(CBonePartition* pBonePartitionIn)
{
	pBonePartition = pBonePartitionIn;

	VertexStart = 0;
	VertexCount = 0;
	TriangleIndexStart = 0;
	TriangleIndexCount = 0;

	Unknown1 = 0;
	Unknown2 = 0;

	HasFlags = 0;
	FlagsValue1 = 0;
	FlagsValue2 = 0;
	FlagsValue3 = 0;
	FlagsValue4 = 0;
	FlagsValue5 = 0;
	FlagsValue6 = 0;
}

bool M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::AddTriangle(CTriangle* pTriangle)
{
	if (pBonePartition->HasTriangle(pTriangle->TriangleIndex))
	{
		Triangles.push_back(pTriangle);

		return true;
	}

	return false;
}


//UInt32 M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::AddVertex( UInt32 VertexTriangleIndex )
//{
//	UInt32 Count = Vertices.size();
//	for ( UInt32 i = 0; i < Count; i++ )
//	{
//		if ( Vertices[i] == VertexTriangleIndex )
//		{
//			return i;
//		}
//	}
//
//	Vertices.push_back( VertexTriangleIndex );
//	return Count;
//}


//void M2Lib::M2SkinBuilder::CSubMesh::CSubsetPartition::FixVertexOffsets( SInt32 Delta )
//{
//	for ( UInt32 i = 0; i < Triangles.size(); i++ )
//	{
//		CTriangle* pTriangle = &Triangles[i];
//
//		pTriangle->Vertices[0] += Delta;
//		pTriangle->Vertices[1] += Delta;
//		pTriangle->Vertices[2] += Delta;
//	}
//}


void M2Lib::M2SkinBuilder::CSubMesh::AddSubsetPartition(CBonePartition* pBonePartition)
{
	SubsetPartitions.push_back(new CSubsetPartition(pBonePartition));
}


bool M2Lib::M2SkinBuilder::CSubMesh::AddTriangle(CTriangle* pTriangle)
{
	for (UInt32 i = 0; i < SubsetPartitions.size(); i++)
	{
		if (SubsetPartitions[i]->AddTriangle(pTriangle))
			return true;
	}

	return false;
}


void M2Lib::M2SkinBuilder::Clear()
{
	m_Vertices.clear();
	m_Bones.clear();
	m_Indices.clear();

	for (UInt32 i = 0; i < m_SubMeshList.size(); i++)
	{
		delete m_SubMeshList[i];
	}
	m_SubMeshList.clear();

	for (UInt32 i = 0; i < m_BonePartitions.size(); i++)
	{
		delete m_BonePartitions[i];
	}
	m_BonePartitions.clear();
}


bool M2Lib::M2SkinBuilder::Build(M2Skin* pResult, UInt32 BoneLoD, M2I* pM2I, CVertex* pGlobalVertexList, UInt32 BoneStart)
{
	m_MaxBones = BoneLoD;
	Clear();

	// build bone partitions
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tbuilding bone partitions..." << std::endl;
	}

	for (UInt32 i = 0; i < pM2I->SubMeshList.size(); ++i)
	{
		auto& SubMesh = pM2I->SubMeshList[i];
		for (UInt32 j = 0; j < SubMesh->Triangles.size(); ++j)
		{
			bool Added = false;
			for (UInt32 k = 0; k < m_BonePartitions.size(); ++k)
			{
				if (m_BonePartitions[k]->AddTriangle(pGlobalVertexList, &SubMesh->Triangles[j]))
				{
					Added = true;
					break;
				}
			}

			if (!Added)
			{
				m_BonePartitions.push_back(new CBonePartition(&m_MaxBones));
				assert(m_BonePartitions.back()->AddTriangle(pGlobalVertexList, &SubMesh->Triangles[j]));
			}
		}
	}

	UInt32 iBoneStart = BoneStart;
	for (UInt32 i = 0; i < m_BonePartitions.size(); ++i)
	{
		m_BonePartitions[i]->BoneStart = iBoneStart;
		iBoneStart += m_BonePartitions[i]->Bones.size();
	}

	// build sub mesh list
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tbuilding subsets..." << std::endl;
	}

	for (UInt32 i = 0; i < pM2I->SubMeshList.size(); ++i)
	{
		CSubMesh* pNewSubset = new CSubMesh();
		pNewSubset->ID = pM2I->SubMeshList[i]->ID;
		pNewSubset->pExtraData = &pM2I->SubMeshList[i]->ExtraData;
		pNewSubset->Level = pM2I->SubMeshList[i]->Level;

		// add sub mesh partitions
		for (UInt32 k = 0; k < m_BonePartitions.size(); k++)
		{
			pNewSubset->AddSubsetPartition(m_BonePartitions[k]);
		}

		m_SubMeshList.push_back(pNewSubset);
	}


	// build sub meshes, deal out triangles between subsets and their partitions
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tdealing out triangles to sub meshes..." << std::endl;
	}

	for (UInt32 i = 0; i < m_SubMeshList.size(); ++i)
	{
		M2I::CSubMesh* pSubMeshM2I = pM2I->SubMeshList[i];
		for (UInt32 j = 0; j < pSubMeshM2I->Triangles.size(); ++j)
		{
			assert(m_SubMeshList[i]->AddTriangle(&pSubMeshM2I->Triangles[j]));
		}
	}


	// build index and triangle lists
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tbuilding index and triangle lists..." << std::endl;
	}

	UInt32 VertexStart = 0;
	UInt32 TriangleIndexStart = 0;
	for (UInt32 i = 0; i < m_SubMeshList.size(); ++i)
	{
		for (UInt32 j = 0; j < m_SubMeshList[i]->SubsetPartitions.size(); ++j)
		{
			CSubMesh::CSubsetPartition * pSubsetPartition = m_SubMeshList[i]->SubsetPartitions[j];
			if (pSubsetPartition->Triangles.empty())
				continue;

			std::map< UInt16, UInt16 > GlobalToSkinIndexMap;
			UInt32 VertexCount = 0;
			UInt32 TriangleIndexCount = 0;
			for (UInt32 k = 0; k < pSubsetPartition->Triangles.size(); ++k)
			{
				for (UInt32 iVert = 0; iVert < VERTEX_PER_TRIANGLE; ++iVert)
				{
					UInt16 VertexToMap = pSubsetPartition->Triangles[k]->Vertices[iVert];	// this is the global vertex index
					UInt16 VertexMapped = 0;
					if (GlobalToSkinIndexMap.find(VertexToMap) != GlobalToSkinIndexMap.end())
					{
						VertexMapped = GlobalToSkinIndexMap[VertexToMap];
					}
					else
					{
						VertexMapped = (UInt16)m_Vertices.size();
						m_Vertices.push_back(VertexToMap);
						GlobalToSkinIndexMap[VertexToMap] = VertexMapped;
						++VertexCount;
					}
					m_Indices.push_back(VertexMapped);
					++TriangleIndexCount;
				}
			}
			pSubsetPartition->VertexStart = VertexStart;
			pSubsetPartition->VertexCount = VertexCount;
			VertexStart += VertexCount;
			pSubsetPartition->TriangleIndexStart = TriangleIndexStart;
			pSubsetPartition->TriangleIndexCount = TriangleIndexCount;
			TriangleIndexStart += TriangleIndexCount;
		}
	}


	// build skin bone list
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tbuilding skin bone lookup list..." << std::endl;
	}

	for (auto& BonePartition : m_BonePartitions)
	{
		for (auto Bone : BonePartition->Bones)
			m_Bones.push_back(Bone);
	}

	// fill result
	pResult->Header.ID[0] = 'S';
	pResult->Header.ID[1] = 'K';
	pResult->Header.ID[2] = 'I';
	pResult->Header.ID[3] = 'N';

	//pResult->Header.LevelOfDetail = m_MaxBones;
	pResult->Header.Unknown1 = 0;
	pResult->Header.Unknown2 = 0;
	pResult->Header.Unknown3 = 0;

	// copy indices
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tcopying indices to element..." << std::endl;
	}

	pResult->Elements[M2SkinElement::EElement_VertexLookup].SetDataSize(m_Vertices.size(), m_Vertices.size() * sizeof(UInt16), false);
	UInt16* Indices = pResult->Elements[M2SkinElement::EElement_VertexLookup].as<UInt16>();
	memcpy(Indices, m_Vertices.data(), m_Vertices.size() * sizeof(UInt16));

	// copy triangles
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tcopying triangles to element..." << std::endl;
	}

	pResult->Elements[M2SkinElement::EElement_TriangleIndex].SetDataSize(m_Indices.size(), m_Indices.size() * sizeof(UInt16), false);
	UInt16* Triangles = pResult->Elements[M2SkinElement::EElement_TriangleIndex].as<UInt16>();
	memcpy(Triangles, m_Indices.data(), m_Indices.size() * sizeof(UInt16)); // triangles

	// copy subsets
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tcopying subsets to element..." << std::endl;
	}

	UInt32 TotalPartitionCount = 0;
	for (UInt32 i = 0; i < m_SubMeshList.size(); ++i)
		for (UInt32 j = 0; j < m_SubMeshList[i]->SubsetPartitions.size(); ++j)
			if (m_SubMeshList[i]->SubsetPartitions[j]->Triangles.size())
				++TotalPartitionCount;

	pResult->Elements[M2SkinElement::EElement_SubMesh].SetDataSize(TotalPartitionCount, TotalPartitionCount * sizeof(M2SkinElement::CElement_SubMesh), false);
	M2SkinElement::CElement_SubMesh* SubsetsOut = pResult->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	UInt32 iSubsetPartition = 0;

	for (UInt32 i = 0; i < m_SubMeshList.size(); i++)
	{
		for (UInt32 j = 0; j < m_SubMeshList[i]->SubsetPartitions.size(); j++)
		{
			CSubMesh::CSubsetPartition* pSubsetPartitionIn = m_SubMeshList[i]->SubsetPartitions[j];
			if (m_SubMeshList[i]->SubsetPartitions[j]->Triangles.size())
			{
				M2SkinElement::CElement_SubMesh* pSubsetOut = &SubsetsOut[iSubsetPartition];

				pSubsetOut->ID = m_SubMeshList[i]->ID;
				pSubsetOut->Level = m_SubMeshList[i]->Level;
				pSubsetOut->VertexStart = pSubsetPartitionIn->VertexStart;
				pSubsetOut->VertexCount = pSubsetPartitionIn->VertexCount;
				pSubsetOut->TriangleIndexStart = pSubsetPartitionIn->TriangleIndexStart;
				pSubsetOut->TriangleIndexCount = pSubsetPartitionIn->TriangleIndexCount;
				pSubsetOut->BoneStart = pSubsetPartitionIn->pBonePartition->BoneStart;
				pSubsetOut->BoneCount = (UInt16)pSubsetPartitionIn->pBonePartition->Bones.size();

				// we do the real calculation for this later
				pSubsetOut->MaxBonesPerVertex = 0;

				// don't know what this is
				//pSubsetOut->Unknown2 = pSubsetPartitionIn->Unknown2;

				// store comparison data that is calculated from original mesh before it was separated
				pResult->ExtraDataBySubmeshIndex[iSubsetPartition] = m_SubMeshList[i]->pExtraData;

				++iSubsetPartition;
			}
		}
	}

	// build bounding data
	if (g_Verbose >= 2)
	{
		std::cout << "\t\tbuilding subset bounds data..." << std::endl;
	}

	pResult->BuildBoundingData();

	return true;
}

