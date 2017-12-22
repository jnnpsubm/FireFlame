#include "M2.h"
#include "DataBinary.h"
#include "M2SkinBuilder.h"
#include <iostream>
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>

using namespace M2Lib::M2Element;

// level of detail for output messages
int g_Verbose = 1;

#define REVERSE_CC(x) \
	 ((x & 0xFF) << 24 | ((x >> 8) & 0xFF) << 16 | ((x >> 16) & 0xFF) << 8 | (x >> 24) & 0xFF)

const UInt32 M2Lib::M2::kChunkIDs[EChunk__Count__] = { 'MD21', 'PFID', 'AFID', 'SFID', 'BFID' };

M2Lib::DataElement* M2Lib::M2::GetLastElement()
{
	for (int i = M2Element::EElement__Count__ - 1; i >= 0; --i)
	{
		if (!Elements[i].Data.empty())
			return &Elements[i];
	}

	return NULL;
}

M2Lib::EError M2Lib::M2::Load(const Char16* FileName)
{
	// check path
	if (!FileName)
		return EError_FailedToLoadM2_NoFileSpecified;
	UInt32 Length = 0;
	while (FileName[Length] != '\0')
	{
		Length++;
	}

	if (Length >= 1024)
		return EError_PathTooLong;

	_FileName[Length] = '\0';
	for (UInt32 i = 0; i != Length; i++)
	{
		_FileName[i] = FileName[i];
	}

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::in | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToLoadM2_CouldNotOpenFile;

	// find file size
	FileStream.seekg(0, std::ios::end);
	UInt32 FileSize = (UInt32)FileStream.tellg();
	FileStream.seekg(0, std::ios::beg);

	DataElement::SetFileOffset(0);

	while (FileStream.tellg() < FileSize)
	{
		UInt32 ChunkId;
		UInt32 ChunkSize;

		FileStream.read((char*)&ChunkId, sizeof(ChunkId));
		FileStream.read((char*)&ChunkSize, sizeof(ChunkSize));

		// support pre-legion M2
		if (REVERSE_CC(ChunkId) == 'MD20')
		{
			SInt32 ChunkIndex = m_GetChunkIndex('MD21');

			auto& Chunk = Chunks[ChunkIndex];

			Chunk.Count = 1;
			Chunk.Data.resize(FileSize);
			Chunk.Offset = 0;
			FileStream.seekg(0, std::ios::beg);
			FileStream.read((char*)Chunk.Data.data(), Chunk.Data.size());
			break;
		}
		else
		{
			SInt32 ChunkIndex = m_GetChunkIndex(REVERSE_CC(ChunkId));
			if (ChunkIndex >= 0)
			{
				auto& Chunk = Chunks[ChunkIndex];
				Chunk.Count = 1;
				Chunk.Data.resize(ChunkSize);
				Chunk.Offset = (UInt32)FileStream.tellg();
				FileStream.read((char*)Chunk.Data.data(), Chunk.Data.size());
			}
			else
				FileStream.seekg(ChunkSize, std::ios::cur);
		}
	}

	// close file stream
	FileStream.close();

	auto& ModelChunk = Chunks[EChunk_Model];
	if (ModelChunk.Data.empty())
		return EError_FailedToLoadM2_FileCorrupt;

	m_OriginalModelChunkSize = ModelChunk.Data.size();

	// load header
	memcpy(&Header, ModelChunk.Data.data(), sizeof(Header));

	if ((263 > Header.Description.Version) || (Header.Description.Version > 274))
		return EError_FailedToLoadM2_VersionNotSupported;

	if (!Header.IsLongHeader())
	{
		Header.Elements.nUnknown1 = 0;
		Header.Elements.oUnknown1 = 0;
	}

	// fill elements header data
	m_LoadElements_CopyHeaderToElements();
	m_LoadElements_FindSizes(m_OriginalModelChunkSize);

	// load elements
	UInt32 ElementCount = EElement__Count__;
	if (!(Header.Description.Flags & 0x08))
		--ElementCount;

	for (UInt32 i = 0; i < ElementCount; ++i)
	{
		Elements[i].Align = 16;
		if (!Elements[i].Load(ModelChunk.Data.data()))
			return EError_FailedToLoadM2_FileCorrupt;
	}

	// load skins
	if ((Header.Elements.nSkin == 0) || (Header.Elements.nSkin > 4))
		return EError_FailedToLoadM2_FileCorrupt;

	for (UInt32 i = 0; i < Header.Elements.nSkin; ++i)
	{
		Char16 FileNameSkin[1024];
		GetFileSkin(FileNameSkin, FileName, i);

		// FMN 2015-02-03: LOD skins.
		/*
		if (i == 1 || i == 2)
		{
			std::string strFileName;
			std::wstring wstrFileName = (std::wstring)FileName;

			for (UInt8 j = 0; j < wstrFileName.size(); j++)
				strFileName += (char)wstrFileName.at(j);

			//std::string chrI = std::string(i);
			std::stringstream ss;
			std::string strI;
			ss << i;
			ss >> strI;

			std::string strFileNameLOD = strFileName.substr(0, strFileName.length() - 3) + "_LOD0" + strI + ".skin";
			//std::wstring  wstrFileNameLOD = (std::wstring)strFileNameLOD;

			//wchar_t wstrFileNameLOD = '\0';
			//for (UInt8 j = 0; j < strFileNameLOD.size(); j++)
			//	wstrFileNameLOD += (wchar_t)strFileNameLOD.at(j);

			//std::wstring name = (std::wstring)strFileNameLOD;
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			//std::string narrow = converter.to_bytes(wide_utf16_source_string);
			std::wstring wstrFileNameLOD = converter.from_bytes(strFileNameLOD);

			wchar_t* wchrFileNameLOD = (wchar_t*)wstrFileNameLOD.c_str();

			Char16* FileNameLOD = (Char16*)wchrFileNameLOD;

			GetFileSkin(FileNameLOD, FileName, i);

			// Add the LODs after the 4 main skins.
			Skins[i + 4] = new M2Skin(this);
			if (EError Error = Skins[i + 4]->Load(FileNameSkin))
				return Error;
		}
		*/

		Skins[i] = new M2Skin(this);
		if (EError Error = Skins[i]->Load(FileNameSkin))
		{
			delete Skins[i];
			Skins[i] = NULL;
			return Error;
		}
	}

	// don't load, since we have no use of them
	/*if (Header.Description.Flags & 0x80)
	{
		for (int i = SKIN_COUNT - 2; i < SKIN_COUNT; ++i)
		{
			Char16 FileNameSkin[1024];
			GetFileSkin(FileNameSkin, FileName, i);

			Skins[i] = new M2Skin(this);
			if (EError Error = Skins[i]->Load(FileNameSkin))
			{
				delete Skins[i];
				Skins[i] = NULL;
				return Error;
			}
		}
	}*/

	// print info
	PrintInfo();

	// done
	return EError_OK;
}


M2Lib::EError M2Lib::M2::Save(const Char16* FileName)
{
	// check path
	if (!FileName)
		return EError_FailedToSaveM2_NoFileSpecified;

	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToSaveM2;

	// fill elements header data
	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// Reserve model chunk header
	DataElement::SetFileOffset(8);
	FileStream.seekp(8, std::ios::beg);

	//Header.Description.Version = 0x0110;
	//Header.Description.Flags &= ~0x80;

	// save header
	FileStream.write((Char8*)&Header.Description, sizeof(Header.Description));
	FileStream.write((Char8*)&Header.Elements, Header.IsLongHeader() ? sizeof(Header.Elements) : sizeof(Header.Elements) - 8);

	// save elements
	UInt32 ElementCount = Header.IsLongHeader() ? EElement__Count__ : EElement__Count__ - 1;
	for (UInt32 i = 0; i < ElementCount; ++i)
	{
		if (!Elements[i].Save(FileStream))
			return EError_FailedToSaveM2;
	}

	UInt32 MD20Size = (UInt32)FileStream.tellp();
	MD20Size -= 8;

	for (UInt32 i = EChunk_Model + 1; i < EChunk__Count__; i++)
	{
		if (!Chunks[i].Data.empty())
		{
			UInt32 ChunkId = REVERSE_CC(kChunkIDs[i]);
			FileStream.write((Char8*)&ChunkId, 4);
			UInt32 size = Chunks[i].Data.size();
			FileStream.write((Char8*)&size, 4);
			FileStream.write((Char8*)Chunks[i].Data.data(), Chunks[i].Data.size());
		}
	}

	FileStream.seekp(0, std::ios::beg);
	UInt32 ChunkId = REVERSE_CC(kChunkIDs[EChunk_Model]);
	FileStream.write((Char8*)&ChunkId, 4);
	FileStream.write((Char8*)(&MD20Size), 4);

	// close file stream
	FileStream.close();

	// delete existing skin files
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		Char16 FileNameSkin[1024];
		GetFileSkin(FileNameSkin, FileName, i);
		_wremove(FileNameSkin);
	}

	// save skins
	if ((Header.Elements.nSkin == 0) || (Header.Elements.nSkin > 4))
		return EError_FailedToSaveM2;

	for (UInt32 i = 0; i < Header.Elements.nSkin; i++)
	{
		Char16 FileNameSkin[1024];
		GetFileSkin(FileNameSkin, FileName, i);

		if (i >= 4)
		{
			if (EError Error = Skins[i - 3]->Save(FileNameSkin))
				return Error;
		}
		else
		{
			if (EError Error = Skins[i]->Save(FileNameSkin))
				return Error;
		}
	}

	// 0x80 = flag_has_lod_skin_files
	//if (Header.Description.Flags & 0x80)
	// HAXX
	if (wcsstr(FileName, L"_HD"))
	{
		for (int i = 0; i < 2; ++i)
		{
			Char16 FileNameSkin[1024];
			GetFileSkin(FileNameSkin, FileName, i + 4);
			if (EError Error = (Skins[(Skins[1]) ? 1 : 0])->Save(FileNameSkin))
				return Error;
		}
	}

	return EError_OK;
}

M2Lib::EError M2Lib::M2::ExportM2Intermediate(Char16* FileName)
{
	// open file stream
	std::fstream FileStream;
	FileStream.open(FileName, std::ios::out | std::ios::trunc | std::ios::binary);
	if (FileStream.fail())
		return EError_FailedToExportM2I_CouldNotOpenFile;

	// open binary stream
	DataBinary DataBinary(&FileStream, EEndianness_Little);

	// get data to save
	M2Skin* pSkin = Skins[0];

	UInt32 SubsetCount = pSkin->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* Subsets = pSkin->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	UInt32 MaterialCount = pSkin->Elements[M2SkinElement::EElement_Material].Count;
	auto materials = pSkin->Elements[M2SkinElement::EElement_Material].as<M2SkinElement::CElement_Material>();

	CVertex* Vertices = Elements[EElement_Vertex].as<CVertex>();
	UInt16* Triangles = pSkin->Elements[M2SkinElement::EElement_TriangleIndex].as<UInt16>();
	UInt16* Indices = pSkin->Elements[M2SkinElement::EElement_VertexLookup].as<UInt16>();

	UInt32 BonesCount = Elements[EElement_Bone].Count;
	CElement_Bone* Bones = Elements[EElement_Bone].as<CElement_Bone>();

	UInt32 AttachmentsCount = Elements[EElement_Attachment].Count;
	CElement_Attachment* Attachments = Elements[EElement_Attachment].as<CElement_Attachment>();

	UInt32 CamerasCount = Elements[EElement_Camera].Count;
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();

	// save signature
	DataBinary.WriteFourCC(M2I::Signature_M2I0);

	// save version
	DataBinary.WriteUInt16(4);	// VersionMajor
	DataBinary.WriteUInt16(8);	// VersionMinor

	// save subsets
	DataBinary.WriteUInt32(SubsetCount);
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		M2SkinElement::CElement_SubMesh* pSubsetOut = &Subsets[i];

		// write subset ID

		UInt16 SubsetID = pSubsetOut->ID;
		UInt16 SubsetLevel = pSubsetOut->Level;

		DataBinary.WriteUInt16(SubsetID);
		DataBinary.WriteASCIIString("mesh #" + std::to_string(i));	// description
		DataBinary.WriteSInt16(-1);			// material override
		DataBinary.WriteASCIIString("");	// custom texture
		DataBinary.WriteASCIIString("");	// gloss texture
		auto found = false;
		for (UInt32 j = 0; j < MaterialCount; ++j)
		{
			if (materials[j].iSubMesh == i)
			{
				found = true;
				DataBinary.WriteSInt16(j);
				break;
			}
		}

		if (!found)
			DataBinary.WriteSInt16(-1);

		DataBinary.WriteUInt16(SubsetLevel);

		// write vertices
		DataBinary.WriteUInt32(pSubsetOut->VertexCount);
		UInt32 VertexEnd = pSubsetOut->VertexStart + pSubsetOut->VertexCount;
		for (UInt32 k = pSubsetOut->VertexStart; k < VertexEnd; k++)
		{
			CVertex& Vertex = Vertices[Indices[k]];

			DataBinary.WriteFloat32(Vertex.Position[0]);
			DataBinary.WriteFloat32(Vertex.Position[1]);
			DataBinary.WriteFloat32(Vertex.Position[2]);

			DataBinary.WriteUInt8(Vertex.BoneWeights[0]);
			DataBinary.WriteUInt8(Vertex.BoneWeights[1]);
			DataBinary.WriteUInt8(Vertex.BoneWeights[2]);
			DataBinary.WriteUInt8(Vertex.BoneWeights[3]);

			DataBinary.WriteUInt8(Vertex.BoneIndices[0]);
			DataBinary.WriteUInt8(Vertex.BoneIndices[1]);
			DataBinary.WriteUInt8(Vertex.BoneIndices[2]);
			DataBinary.WriteUInt8(Vertex.BoneIndices[3]);

			DataBinary.WriteFloat32(Vertex.Normal[0]);
			DataBinary.WriteFloat32(Vertex.Normal[1]);
			DataBinary.WriteFloat32(Vertex.Normal[2]);

			DataBinary.WriteFloat32(Vertex.Texture[0]);
			DataBinary.WriteFloat32(Vertex.Texture[1]);
		}

		// write triangles
		UInt32 SubsetTriangleCountOut = pSubsetOut->TriangleIndexCount / 3;
		DataBinary.WriteUInt32(SubsetTriangleCountOut);

		// FMN 2015-01-26: changing TriangleIndexstart, depending on ID. See http://forums.darknestfantasyerotica.com/showthread.php?20446-TUTORIAL-Here-is-how-WoD-.skin-works.&p=402561
		UInt32 TriangleIndexStart = 0;

		TriangleIndexStart = UInt32(pSubsetOut->TriangleIndexStart) + (pSubsetOut->Level << 16);

		//UInt32 TriangleIndexEnd = pSubsetOut->TriangleIndexStart + pSubsetOut->TriangleIndexCount;
		//for (UInt32 k = pSubsetOut->TriangleIndexStart; k < TriangleIndexEnd; k++)
		UInt32 TriangleIndexEnd = TriangleIndexStart + pSubsetOut->TriangleIndexCount;
		for (UInt32 k = TriangleIndexStart; k < TriangleIndexEnd; k++)
		{
			UInt16 TriangleIndexOut = Triangles[k] - pSubsetOut->VertexStart;
			assert(TriangleIndexOut < pSubsetOut->VertexCount);
			DataBinary.WriteUInt16(TriangleIndexOut);
		}
	}

	// write bones
	DataBinary.WriteUInt32(BonesCount);
	for (UInt16 i = 0; i < BonesCount; i++)
	{
		CElement_Bone& Bone = Bones[i];

		DataBinary.WriteUInt16(i);
		DataBinary.WriteSInt16(Bone.ParentBone);
		DataBinary.WriteFloat32(Bone.Position[0]);
		DataBinary.WriteFloat32(Bone.Position[1]);
		DataBinary.WriteFloat32(Bone.Position[2]);
	}

	// write attachments
	DataBinary.WriteUInt32(AttachmentsCount);
	for (UInt16 i = 0; i < AttachmentsCount; i++)
	{
		CElement_Attachment& Attachment = Attachments[i];

		DataBinary.WriteUInt32(Attachment.ID);
		DataBinary.WriteSInt16(Attachment.ParentBone);
		DataBinary.WriteFloat32(Attachment.Position[0]);
		DataBinary.WriteFloat32(Attachment.Position[1]);
		DataBinary.WriteFloat32(Attachment.Position[2]);
		DataBinary.WriteFloat32(1.0f);
	}

	// write cameras
	DataBinary.WriteUInt32(CamerasCount);
	for (UInt16 i = 0; i < CamerasCount; i++)
	{
		CElement_Camera& Camera = Cameras[i];

		DataBinary.WriteSInt32(Camera.Type);

		// extract field of view of camera from animation block
		if (Camera.AnimationBlock_FieldOfView.Keys.Count > 0)
		{
			auto ExternalAnimations = (M2Array*)Elements[EElement_Camera].GetLocalPointer(Camera.AnimationBlock_FieldOfView.Keys.Offset);
			auto LastElement = GetLastElement();
			assert(LastElement != NULL);
			assert(ExternalAnimations[0].Offset >= LastElement->Offset && ExternalAnimations[0].Offset < LastElement->Offset + LastElement->Data.size());

			Float32* FieldOfView_Keys = (Float32*)LastElement->GetLocalPointer(ExternalAnimations[0].Offset);
			DataBinary.WriteFloat32(FieldOfView_Keys[0]);
		}
		else
		{
			Float32 TempFoV = 0.785398163f;	// 45 degrees in radians, assuming that WoW stores camera FoV in radians. or maybe it's half FoV.
			DataBinary.WriteFloat32(TempFoV);
		}

		DataBinary.WriteFloat32(Camera.ClipFar);
		DataBinary.WriteFloat32(Camera.ClipNear);
		DataBinary.WriteFloat32(Camera.Position[0]);
		DataBinary.WriteFloat32(Camera.Position[1]);
		DataBinary.WriteFloat32(Camera.Position[2]);
		DataBinary.WriteFloat32(Camera.Target[0]);
		DataBinary.WriteFloat32(Camera.Target[1]);
		DataBinary.WriteFloat32(Camera.Target[2]);
	}

	FileStream.close();

	return EError_OK;
}

M2Lib::EError M2Lib::M2::ImportM2Intermediate(Char16* FileName, bool IgnoreBones, bool IgnoreAttachments, bool IgnoreCameras, bool FixSeams)
{
	Float32 SubmeshPositionalTolerance = 0.0001f;
	Float32 SubmeshAngularTolerance = 45.0f;
	Float32 BodyPositionalTolerance = 0.0001f;
	Float32 BodyAngularTolerance = 90.0f;
	Float32 ClothingPositionalTolerance = 0.0001f;
	Float32 ClothingAngularTolerance = 90.0f;

	if (!FileName)
		return EError_FailedToImportM2I_NoFileSpecified;

	// check that we have an M2 already loaded and ready to be injected
	if (!Header.Elements.nSkin)
		return EError_FailedToExportM2I_M2NotLoaded;

	if (pInM2I)
		delete pInM2I;
	pInM2I = new M2I();

	auto Error = pInM2I->Load(FileName, this, IgnoreBones, IgnoreAttachments, IgnoreCameras);
	if (Error != EError_OK)
		return Error;

	// copy new vertex list from M2I to M2
	auto& NewVertexList = pInM2I->VertexList;
	Elements[EElement_Vertex].SetDataSize(NewVertexList.size(), NewVertexList.size() * sizeof(CVertex), false);
	memcpy(Elements[EElement_Vertex].Data.data(), &NewVertexList[0], NewVertexList.size() * sizeof(CVertex));

	BoundaryData GlobalBoundary;
	GlobalBoundary.Calculate(NewVertexList);
	//SetGlobalBoundingData(GlobalBoundary);

	// fix seams
	// this is hacky, but we gotta fix seams first
	// build skin 0
	// only build skin 0 for now, so we can fix seams and then build skin for real later
	M2SkinBuilder SkinBuilder;
	std::vector< UInt16 > NewBoneLookup;
	M2Skin* pNewSkin0 = new M2Skin(this);
	assert(SkinBuilder.Build(pNewSkin0, 256, pInM2I, &NewVertexList[0], 0));

	// set skin 0 so we can begin seam fixing
	M2Skin* pOriginalSkin0 = Skins[0];	// save this because we will need to copy materials from it later.
	Header.Elements.nSkin = 1;
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		if (Skins[i])
		{
			if (i != 0)
				delete Skins[i];
			Skins[i] = NULL;
		}
	}

	Skins[0] = pNewSkin0;

	if (FixSeams)
	{
		// fix normals within submeshes
		FixSeamsSubMesh(SubmeshPositionalTolerance, SubmeshAngularTolerance * DegreesToRadians);

		// fix normals between body submeshes
		FixSeamsBody(BodyPositionalTolerance, BodyAngularTolerance * DegreesToRadians);

		// close gaps between clothes and body
		FixSeamsClothing(ClothingPositionalTolerance, ClothingAngularTolerance * DegreesToRadians);
	}

	//
	//
	//
	// build skins for real this time
	// because a few bone indices might have changed during seam and gap fixing
	// this list will store the new skins
	M2Skin* NewSkinList[SKIN_COUNT];
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
		NewSkinList[i] = NULL;

	// for easy looping
	UInt32 MaxBoneList[5];
	MaxBoneList[0] = 256;
	MaxBoneList[1] = 64;
	MaxBoneList[2] = 53;
	MaxBoneList[3] = 21;
	MaxBoneList[4] = 0;		// extra entry needed for LoD check
	//MaxBoneList[4] = 64;	// extracted from client
	//MaxBoneList[5] = 64;
	//MaxBoneList[6] = 64;

	NewBoneLookup.clear();
	SInt32 BoneStart = 0;
	UInt32 iSkin = 0;

	for (UInt32 iLoD = 0; iLoD < SKIN_COUNT - 2; ++iLoD)
	{
		M2Skin* pNewSkin = new M2Skin(this);
		assert(SkinBuilder.Build(pNewSkin, MaxBoneList[iLoD], pInM2I, Elements[EElement_Vertex].as<CVertex>(), BoneStart));
		// if there are more bones than the next lowest level of detail
		if (SkinBuilder.m_Bones.size() > MaxBoneList[iLoD + 1])
		{
			// copy skin to result list
			NewSkinList[iSkin] = pNewSkin;
			iSkin++;

			// copy skin's bone lookup to the global bone lookup list
			for (UInt32 i = 0; i < SkinBuilder.m_Bones.size(); i++)
				NewBoneLookup.push_back(SkinBuilder.m_Bones[i]);

			// advance for where next skin's bone lookup will begin
			BoneStart += SkinBuilder.m_Bones.size();
		}
		else
		{
			// this skin does not have enough bones and so it is not needed because the next lowest level of detail can contain the whole thing just fine, so discard this skin.
			delete pNewSkin;
		}
	}

	// set skin count
	Header.Elements.nSkin = iSkin;

	// copy materials from old sub meshes to new sub meshes
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		if (NewSkinList[i])
		{
			NewSkinList[i]->CopyMaterials(pOriginalSkin0);
			//NewSkinList[i]->SortSubMeshes();
		}
	}
	delete pOriginalSkin0;

	// replace old skins with new
	for (UInt32 i = 0; i < SKIN_COUNT; ++i)
	{
		if (Skins[i])
			delete Skins[i];

		Skins[i] = NewSkinList[i];
	}

	// copy new bone lookup
	Elements[EElement_SkinnedBoneLookup].SetDataSize(NewBoneLookup.size(), NewBoneLookup.size() * sizeof(UInt16), false);
	memcpy(Elements[EElement_SkinnedBoneLookup].Data.data(), &NewBoneLookup[0], NewBoneLookup.size() * sizeof(UInt16));

	// build vertex bone indices
	for (UInt32 i = 0; i < Header.Elements.nSkin; ++i)
	{
		if (!Skins[i])
			continue;

		Skins[i]->BuildVertexBoneIndices();
		Skins[i]->m_SaveElements_FindOffsets();
		Skins[i]->m_SaveElements_CopyElementsToHeader();
	}

	m_SaveElements_FindOffsets();
	m_SaveElements_CopyElementsToHeader();

	// done, ready to be saved
	return EError_OK;
}

void M2Lib::M2::SetGlobalBoundingData(BoundaryData& Data)
{
	auto ExtraData = Data.CalculateExtra();

	Elements[EElement_BoundingVertex].SetDataSize(BOUNDING_VERTEX_COUNT, sizeof(CElement_BoundingVertices) * BOUNDING_VERTEX_COUNT, false);
	auto boundingVertices = Elements[EElement_BoundingVertex].as<CElement_BoundingVertices>();
	for (UInt32 i = 0; i < BOUNDING_VERTEX_COUNT; ++i)
	{
		boundingVertices[i].Position[0] = ExtraData.BoundingVertices[i].X;
		boundingVertices[i].Position[1] = ExtraData.BoundingVertices[i].Y;
		boundingVertices[i].Position[2] = ExtraData.BoundingVertices[i].Z;
	}

	Elements[EElement_BoundingNormal].SetDataSize(BOUNDING_TRIANGLE_COUNT, sizeof(CElement_BoundingNormals) * BOUNDING_TRIANGLE_COUNT, false);
	auto boundingNormals = Elements[EElement_BoundingNormal].as<CElement_BoundingNormals>();
	for (UInt32 i = 0; i < BOUNDING_TRIANGLE_COUNT; ++i)
	{
		boundingNormals[i].Normal[0] = ExtraData.BoundingNormals[i].X;
		boundingNormals[i].Normal[1] = ExtraData.BoundingNormals[i].Y;
		boundingNormals[i].Normal[2] = ExtraData.BoundingNormals[i].Z;
	}

	Elements[EElement_BoundingTriangle].SetDataSize(BOUNDING_TRIANGLE_COUNT * 3, sizeof(CElement_BoundingTriangle) * BOUNDING_TRIANGLE_COUNT * 3, false);
	auto boundingTriangles = Elements[EElement_BoundingTriangle].as<CElement_BoundingTriangle>();
	for (UInt32 i = 0; i < BOUNDING_TRIANGLE_COUNT * 3; ++i)
	{
		boundingTriangles[i].Index = BoundaryData::ExtraData::BoundingTriangleVertexMap[i];
	}
}

void M2Lib::M2::PrintInfo()
{
	//
	//
	// just print out any sort of data that we want to analize when troubleshooting

	UInt32 Count = 0;

	Char16 szFileOut[1024];
	UInt32 Length = 0;
	while (_FileName[Length] != '\0')
	{
		szFileOut[Length] = _FileName[Length];
		Length++;
	}
	szFileOut[Length++] = '.';
	szFileOut[Length++] = 't';
	szFileOut[Length++] = 'x';
	szFileOut[Length++] = 't';
	szFileOut[Length++] = '\0';

	std::fstream FileStream;
	FileStream.open(szFileOut, std::ios::out | std::ios::trunc);

	FileStream << "ID       " << Header.Description.ID[0] << Header.Description.ID[1] << Header.Description.ID[2] << Header.Description.ID[3] << std::endl;		// 'MD20'
	FileStream << "Version  " << Header.Description.Version << std::endl;
	FileStream << std::endl;

	FileStream << "nName                     " << Header.Description.nName << std::endl;
	FileStream << "oName                     " << Header.Description.oName << std::endl;
	FileStream << " Value                    " << Elements[EElement_Name].as<Char8>() << std::endl;
	FileStream << std::endl;

	FileStream << "Flags                     " << Header.Description.Flags << std::endl;
	FileStream << std::endl;

	FileStream << "nGlobalSequences          " << Header.Elements.nGlobalSequence << std::endl;
	FileStream << "oGlobalSequences          " << Header.Elements.oGlobalSequence << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_GlobalSequence].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAnimations               " << Header.Elements.nAnimation << std::endl;
	FileStream << "oAnimations               " << Header.Elements.oAnimation << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Animation].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAnimationsLookup         " << Header.Elements.nAnimationLookup << std::endl;
	FileStream << "oAnimationsLookup         " << Header.Elements.oAnimationLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_AnimationLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBones                    " << Header.Elements.nBone << std::endl;
	FileStream << "oBones                    " << Header.Elements.oBone << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Bone].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nKeyBoneLookup            " << Header.Elements.nKeyBoneLookup << std::endl;
	FileStream << "oKeyBoneLookup            " << Header.Elements.oKeyBoneLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_KeyBoneLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nVertices                 " << Header.Elements.nVertex << std::endl;
	FileStream << "oVertices                 " << Header.Elements.oVertex << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Vertex].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nViews                    " << Header.Elements.nSkin << std::endl;
	FileStream << std::endl;

	FileStream << "nColors                   " << Header.Elements.nColor << std::endl;
	FileStream << "oColors                   " << Header.Elements.oColor << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Color].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextures                 " << Header.Elements.nTexture << std::endl;
	FileStream << "oTextures                 " << Header.Elements.oTexture << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Texture].Data.size() << std::endl;
	FileStream << std::endl;

    for (UInt32 i = 0; i < Header.Elements.nTexture; ++i)
    {
		CElement_Texture* texture = Elements[EElement_Texture].as<CElement_Texture>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tFlags: " << (UInt32)texture[i].Flags << std::endl;
        FileStream << "\tType: " << (UInt32)texture[i].Type << std::endl;
        if (texture[i].TexturePath.Count > 1)
			FileStream << "\tPath: " << (char*)Elements[EElement_Texture].GetLocalPointer(texture[i].TexturePath.Offset) << std::endl;
        else
            FileStream << "\tPath: unk" << std::endl;
        FileStream << std::endl;
    }

	FileStream << "nTransparencies           " << Header.Elements.nTransparency << std::endl;
	FileStream << "oTransparencies           " << Header.Elements.oTransparency << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Transparency].Data.size() << std::endl;

	CElement_Transparency* Transparencies = Elements[EElement_Transparency].as<CElement_Transparency>();
    for (UInt32 i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto transparency = Transparencies[i];
        FileStream << "\t" << i << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.InterpolationType << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.GlobalSequenceID << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Times.Count << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Times.Offset << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Keys.Count << std::endl;
        FileStream << "\t" << transparency.AnimationBlock_Transparency.Keys.Offset << std::endl;

            /*
            EInterpolationType InterpolationType;
            SInt16 GlobalSequenceID;
            UInt32 nTimes;
            UInt32 oTimes;
            UInt32 nKeys;
            UInt32 oKeys;
            */
    }
	FileStream << std::endl;

	FileStream << "nTextureAnimation         " << Header.Elements.nTextureAnimation << std::endl;
	FileStream << "oTextureAnimation         " << Header.Elements.nTextureAnimation << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureAnimation].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureReplace           " << Header.Elements.nTextureReplace << std::endl;
	FileStream << "oTextureReplace           " << Header.Elements.oTextureReplace << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureReplace].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureFlags             " << Header.Elements.nTextureFlags << std::endl;
	FileStream << "oTextureFlags             " << Header.Elements.oTextureFlags << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureFlags].Data.size() << std::endl;
	CElement_TextureFlag* TextureFlags = Elements[EElement_TextureFlags].as<CElement_TextureFlag>();
    for (UInt32 i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto flag = TextureFlags[i];
        FileStream << "\t-- " << i << std::endl;
        FileStream << "\t" << flag.Flags << std::endl;
        FileStream << "\t" << flag.Blend << std::endl;
    }
	FileStream << std::endl;

	FileStream << "nSkinnedBoneLookup        " << Header.Elements.nSkinnedBoneLookup << std::endl;
	FileStream << "oSkinnedBoneLookup        " << Header.Elements.oSkinnedBoneLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_SkinnedBoneLookup].Data.size() << std::endl;
/*    EElement_SkinnedBoneLookup* SkinnedBonesLookup = (CElement_TextuEElement_SkinnedBoneLookupreFlag*)Elements[EElement_SkinnedBoneLookup].Data;
    for (auto i = 0; i < Header.Elements.nTransparency; ++i)
    {
        auto flag = TextureFlags[i];
        FileStream << "\t-- " << i << std::endl;
        FileStream << "\t" << flag.Flags << std::endl;
        FileStream << "\t" << flag.Blend << std::endl;
    }*/
	FileStream << std::endl;

	FileStream << "nTexturesLookup           " << Header.Elements.nTextureLookup << std::endl;
	FileStream << "oTexturesLookup           " << Header.Elements.oTextureLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureLookup].Data.size() << std::endl;
	FileStream << std::endl;

    for (UInt32 i = 0; i < Header.Elements.nTexture; ++i)
    {
        CElement_TextureLookup* textureLookup = Elements[EElement_TextureLookup].as<CElement_TextureLookup>();

        FileStream << "\t" << i << std::endl;
        FileStream << "\tIndex: " << textureLookup[i].TextureIndex << std::endl;
        FileStream << std::endl;
    }

	FileStream << "nTextureUnitsLookup       " << Header.Elements.nTextureUnitLookup << std::endl;
	FileStream << "oTextureUnitsLookup       " << Header.Elements.oTextureUnitLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureUnitLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTransparenciesLookup     " << Header.Elements.nTransparencyLookup << std::endl;
	FileStream << "oTransparenciesLookup     " << Header.Elements.oTransparencyLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TransparencyLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nTextureAnimationsLookup  " << Header.Elements.nTextureAnimationLookup << std::endl;
	FileStream << "oTextureAnimationsLookup  " << Header.Elements.oTextureAnimationLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_TextureAnimationLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "Volumes " << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min[0] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min[1] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Min[2] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max[0] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max[1] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Max[2] << std::endl;
	FileStream << Header.Elements.CollisionVolume.Radius << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min[0] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min[1] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Min[2] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max[0] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max[1] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Max[2] << std::endl;
	FileStream << Header.Elements.BoundingVolume.Radius << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingTriangles        " << Header.Elements.nBoundingTriangle << std::endl;
	FileStream << "oBoundingTriangles        " << Header.Elements.oBoundingTriangle << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingTriangle].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingVertices         " << Header.Elements.nBoundingVertex << std::endl;
	FileStream << "oBoundingVertices         " << Header.Elements.oBoundingVertex << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingVertex].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nBoundingNormals          " << Header.Elements.nBoundingNormal << std::endl;
	FileStream << "oBoundingNormals          " << Header.Elements.oBoundingNormal << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_BoundingNormal].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAttachments              " << Header.Elements.nAttachment << std::endl;
	FileStream << "oAttachments              " << Header.Elements.oAttachment << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Attachment].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nAttachmentsLookup        " << Header.Elements.nAttachmentLookup << std::endl;
	FileStream << "oAttachmentsLookup        " << Header.Elements.oAttachmentLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_AttachmentLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nEvents                   " << Header.Elements.nEvent << std::endl;
	FileStream << "oEvents                   " << Header.Elements.oEvent << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Event].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nLights                   " << Header.Elements.nLight << std::endl;
	FileStream << "oLights                   " << Header.Elements.oLight << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Light].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nCameras                  " << Header.Elements.nCamera << std::endl;
	FileStream << "oCameras                  " << Header.Elements.oCamera << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Camera].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nCamerasLookup            " << Header.Elements.nCameraLookup << std::endl;
	FileStream << "oCamerasLookup            " << Header.Elements.oCameraLookup << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_CameraLookup].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nRibbonEmitters           " << Header.Elements.nRibbonEmitter << std::endl;
	FileStream << "oRibbonEmitters           " << Header.Elements.oRibbonEmitter << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_RibbonEmitter].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nParticleEmitters         " << Header.Elements.nParticleEmitter << std::endl;
	FileStream << "oParticleEmitters         " << Header.Elements.oParticleEmitter << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_ParticleEmitter].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream << "nUnknown1                 " << Header.Elements.nUnknown1 << std::endl;
	FileStream << "oUnknown1                 " << Header.Elements.oUnknown1 << std::endl;
	FileStream << " DataSize                 " << Elements[EElement_Unknown1].Data.size() << std::endl;
	FileStream << std::endl;

	FileStream.close();
}


// Gets the .skin file names.
bool M2Lib::M2::GetFileSkin(Char16* SkinFileNameResultBuffer, const Char16* M2FileName, UInt32 SkinIndex)
{
	// ghetto string ops
	SInt32 LastDot = -1;
	UInt32 Length = 0;
	for (; Length < 1024; Length++)
	{
		if (M2FileName[Length] == L'\0')
		{
			break;
		}
		else if (M2FileName[Length] == L'.')
		{
			LastDot = Length;
		}
	}
	if ((LastDot == -1) || (LastDot > 1018))
	{
		return false;
	}
	UInt32 j = 0;
	for (; j != LastDot; j++)
	{
		SkinFileNameResultBuffer[j] = M2FileName[j];
	}
	switch (SkinIndex)
	{
	case 0:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'0';
		break;
	case 1:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'1';
		break;
	case 2:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'2';
		break;
	case 3:
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'3';
		break;
	case 4:
		SkinFileNameResultBuffer[j++] = L'_';
		SkinFileNameResultBuffer[j++] = L'L';
		SkinFileNameResultBuffer[j++] = L'O';
		SkinFileNameResultBuffer[j++] = L'D';
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'1';
		break;
	case 5:
		SkinFileNameResultBuffer[j++] = L'_';
		SkinFileNameResultBuffer[j++] = L'L';
		SkinFileNameResultBuffer[j++] = L'O';
		SkinFileNameResultBuffer[j++] = L'D';
		SkinFileNameResultBuffer[j++] = L'0';
		SkinFileNameResultBuffer[j++] = L'2';
		break;
	}
	SkinFileNameResultBuffer[j++] = L'.';
	SkinFileNameResultBuffer[j++] = L's';
	SkinFileNameResultBuffer[j++] = L'k';
	SkinFileNameResultBuffer[j++] = L'i';
	SkinFileNameResultBuffer[j++] = L'n';
	SkinFileNameResultBuffer[j] = L'\0';

	return true;
}


void M2Lib::M2::FixSeamsSubMesh(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// gather up sub meshes
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > SubMeshes;

	M2SkinElement::CElement_SubMesh* Subsets = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	UInt32 SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		UInt16 ThisID = Subsets[i].ID;
		bool MakeNew = true;
		for (UInt32 j = 0; j < SubMeshes.size(); j++)
		{
			for (UInt32 k = 0; k < SubMeshes[j].size(); k++)
			{
				if (SubMeshes[j][k]->ID == ThisID)
				{
					MakeNew = false;
					SubMeshes[j].push_back(&Subsets[i]);
					break;
				}
			}
			if (!MakeNew)
			{
				break;
			}
		}
		if (MakeNew)
		{
			std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
			NewSubmeshSubsetList.push_back(&Subsets[i]);
			SubMeshes.push_back(NewSubmeshSubsetList);
		}
	}

	// find and merge duplicate vertices
	UInt32 VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (UInt32 iSubMesh1 = 0; iSubMesh1 < SubMeshes.size(); iSubMesh1++)
	{
		for (UInt32 iSubSet1 = 0; iSubSet1 < SubMeshes[iSubMesh1].size(); iSubSet1++)
		{
			M2SkinElement::CElement_SubMesh* pSubSet1 = SubMeshes[iSubMesh1][iSubSet1];

			UInt32 VertexAEnd = pSubSet1->VertexStart + pSubSet1->VertexCount;
			for (UInt32 iVertexA = pSubSet1->VertexStart; iVertexA < VertexAEnd; iVertexA++)
			{
				bool AddedVertexA = false;
				for (UInt32 iSubSet2 = 0; iSubSet2 < SubMeshes[iSubMesh1].size(); iSubSet2++)
				{
					M2SkinElement::CElement_SubMesh* pSubSet2 = SubMeshes[iSubMesh1][iSubSet2];

					UInt32 VertexBEnd = pSubSet2->VertexStart + pSubSet2->VertexCount;
					for (UInt32 iVertexB = pSubSet2->VertexStart; iVertexB < VertexBEnd; iVertexB++)
					{
						if (iVertexA == iVertexB)
						{
							continue;
						}

						if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, PositionalTolerance, AngularTolerance))
						{
							if (!AddedVertexA)
							{
								SimilarVertices.push_back(&VertexList[iVertexA]);
								AddedVertexA = true;
							}

							SimilarVertices.push_back(&VertexList[iVertexB]);
						}
					}
				}

				if (SimilarVertices.size() > 1)
				{
					// sum positions and normals
					Float32 NewPosition[3];
					NewPosition[0] = 0.0f;
					NewPosition[1] = 0.0f;
					NewPosition[2] = 0.0f;

					Float32 NewNormal[3];
					NewNormal[0] = 0.0f;
					NewNormal[1] = 0.0f;
					NewNormal[2] = 0.0f;

					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition[0] += pSimilarVertex->Position[0];
						NewPosition[1] += pSimilarVertex->Position[1];
						NewPosition[2] += pSimilarVertex->Position[2];

						NewNormal[0] += pSimilarVertex->Normal[0];
						NewNormal[1] += pSimilarVertex->Normal[1];
						NewNormal[2] += pSimilarVertex->Normal[2];
					}

					// average position and normalize normal
					Float32 invSimilarCount = 1.f / (Float32)SimilarVertices.size();

					NewPosition[0] *= invSimilarCount;
					NewPosition[1] *= invSimilarCount;
					NewPosition[2] *= invSimilarCount;

					NewNormal[0] *= invSimilarCount;
					NewNormal[1] *= invSimilarCount;
					NewNormal[2] *= invSimilarCount;

					UInt8 NewBoneWeights[4];
					NewBoneWeights[0] = SimilarVertices[0]->BoneWeights[0];
					NewBoneWeights[1] = SimilarVertices[0]->BoneWeights[1];
					NewBoneWeights[2] = SimilarVertices[0]->BoneWeights[2];
					NewBoneWeights[3] = SimilarVertices[0]->BoneWeights[3];

					UInt8 NewBoneIndices[4];
					NewBoneIndices[0] = SimilarVertices[0]->BoneIndices[0];
					NewBoneIndices[1] = SimilarVertices[0]->BoneIndices[1];
					NewBoneIndices[2] = SimilarVertices[0]->BoneIndices[2];
					NewBoneIndices[3] = SimilarVertices[0]->BoneIndices[3];

					// assign new values back into similar vertices
					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position[0] = NewPosition[0];
						pSimilarVertex->Position[1] = NewPosition[1];
						pSimilarVertex->Position[2] = NewPosition[2];

						pSimilarVertex->BoneWeights[0] = NewBoneWeights[0];
						pSimilarVertex->BoneWeights[1] = NewBoneWeights[1];
						pSimilarVertex->BoneWeights[2] = NewBoneWeights[2];
						pSimilarVertex->BoneWeights[3] = NewBoneWeights[3];

						pSimilarVertex->BoneIndices[0] = NewBoneIndices[0];
						pSimilarVertex->BoneWeights[1] = NewBoneWeights[1];
						pSimilarVertex->BoneWeights[2] = NewBoneWeights[2];
						pSimilarVertex->BoneWeights[3] = NewBoneWeights[3];

						pSimilarVertex->Normal[0] = NewNormal[0];
						pSimilarVertex->Normal[1] = NewNormal[1];
						pSimilarVertex->Normal[2] = NewNormal[2];
					}

					SimilarVertices.clear();
				}
			}
		}
	}
}


void M2Lib::M2::FixSeamsBody(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	// sub meshes that are divided up accross multiple bone partitions will have multiple sub mesh entries with the same ID in the M2.
	// we need to gather each body submesh up into a list and average normals of vertices that are similar between other sub meshes.
	// this function is designed to be used on character models, so it may not work on other models.

	// list of submeshes that make up the body of the character
	std::vector< std::vector< M2SkinElement::CElement_SubMesh* > > CompiledSubMeshList;

	// gather up the body submeshes
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();
	UInt32 SubsetCount = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	for (UInt32 i = 0; i < SubsetCount; i++)
	{
		// determine type of subset
		UInt16 ThisID = SubMeshList[i].ID;
		UInt16 Mod = ThisID;
		while (Mod > 10)
		{
			Mod %= 10;
		}
		if ((ThisID == 0) || (ThisID > 10 && Mod == 1) || (ThisID == 702))
		{
			// this subset is part of the character's body
			// add it to the list of submeshes
			bool MakeNew = true;
			for (UInt32 j = 0; j < CompiledSubMeshList.size(); j++)
			{
				for (UInt32 k = 0; k < CompiledSubMeshList[j].size(); k++)
				{
					if (CompiledSubMeshList[j][k]->ID == ThisID)
					{
						MakeNew = false;
						CompiledSubMeshList[j].push_back(&SubMeshList[i]);
						break;
					}
				}
				if (!MakeNew)
				{
					break;
				}
			}
			if (MakeNew)
			{
				std::vector< M2SkinElement::CElement_SubMesh* > NewSubmeshSubsetList;
				NewSubmeshSubsetList.push_back(&SubMeshList[i]);
				CompiledSubMeshList.push_back(NewSubmeshSubsetList);
			}
		}
	}

	// find and merge duplicate vertices
	UInt32 VertexListLength = Elements[EElement_Vertex].Count;
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
	std::vector< CVertex* > SimilarVertices;
	for (SInt32 iSubMesh1 = 0; iSubMesh1 < (SInt32)CompiledSubMeshList.size() - 1; iSubMesh1++)
	{
		for (SInt32 iSubSet1 = 0; iSubSet1 < (SInt32)CompiledSubMeshList[iSubMesh1].size(); iSubSet1++)
		{
			// gather duplicate vertices
			// for each vertex in the subset, compare it against vertices in the other subsets
			// find duplicates and sum their normals
			UInt32 iVertexAEnd = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart + CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexCount;
			for (UInt32 iVertexA = CompiledSubMeshList[iSubMesh1][iSubSet1]->VertexStart; iVertexA < iVertexAEnd; iVertexA++)
			{
				// gather duplicate vertices from other submeshes
				bool AddedVertex1 = false;
				for (SInt32 iSubMesh2 = iSubMesh1 + 1; iSubMesh2 < (SInt32)CompiledSubMeshList.size(); iSubMesh2++)
				{
					// check that we don't check against ourselves
					if (iSubMesh2 == iSubMesh1)
					{
						// other submesh is same as this submesh
						continue;
					}
					// go through subsets
					for (SInt32 iSubSet2 = 0; iSubSet2 < (SInt32)CompiledSubMeshList[iSubMesh2].size(); iSubSet2++)
					{
						// go through vertices in subset
						UInt32 iVertexBEnd = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart + CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexCount;
						for (UInt32 iVertexB = CompiledSubMeshList[iSubMesh2][iSubSet2]->VertexStart; iVertexB < iVertexBEnd; iVertexB++)
						{
							if (CVertex::CompareSimilar(VertexList[iVertexA], VertexList[iVertexB], false, false, PositionalTolerance, AngularTolerance))
							{
								// found a duplicate
								if (!AddedVertex1)
								{
									SimilarVertices.push_back(&VertexList[iVertexA]);
									AddedVertex1 = true;
								}
								// add the vertex from the other sub mesh to the list of similar vertices
								SimilarVertices.push_back(&VertexList[iVertexB]);
							}
						}
					}
				}

				// average normals of similar vertices
				if (SimilarVertices.size())
				{
					// sum positions and normals
					Float32 NewPosition[3];
					NewPosition[0] = 0.0f;
					NewPosition[1] = 0.0f;
					NewPosition[2] = 0.0f;

					Float32 NewNormal[3];
					NewNormal[0] = 0.0f;
					NewNormal[1] = 0.0f;
					NewNormal[2] = 0.0f;

					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						NewPosition[0] += pSimilarVertex->Position[0];
						NewPosition[1] += pSimilarVertex->Position[1];
						NewPosition[2] += pSimilarVertex->Position[2];

						NewNormal[0] += pSimilarVertex->Normal[0];
						NewNormal[1] += pSimilarVertex->Normal[1];
						NewNormal[2] += pSimilarVertex->Normal[2];
					}

					// average position and normalize normal
					Float32 invSimilarCount = 1.0f / (Float32)SimilarVertices.size();

					NewPosition[0] *= invSimilarCount;
					NewPosition[1] *= invSimilarCount;
					NewPosition[2] *= invSimilarCount;

					NewNormal[0] *= invSimilarCount;
					NewNormal[1] *= invSimilarCount;
					NewNormal[2] *= invSimilarCount;

					UInt8 NewBoneWeights[4];
					NewBoneWeights[0] = SimilarVertices[0]->BoneWeights[0];
					NewBoneWeights[1] = SimilarVertices[0]->BoneWeights[1];
					NewBoneWeights[2] = SimilarVertices[0]->BoneWeights[2];
					NewBoneWeights[3] = SimilarVertices[0]->BoneWeights[3];

					UInt8 NewBoneIndices[4];
					NewBoneIndices[0] = SimilarVertices[0]->BoneIndices[0];
					NewBoneIndices[1] = SimilarVertices[0]->BoneIndices[1];
					NewBoneIndices[2] = SimilarVertices[0]->BoneIndices[2];
					NewBoneIndices[3] = SimilarVertices[0]->BoneIndices[3];

					// assign new values back into similar vertices
					for (UInt32 iSimilarVertex = 0; iSimilarVertex < SimilarVertices.size(); iSimilarVertex++)
					{
						CVertex* pSimilarVertex = SimilarVertices[iSimilarVertex];

						pSimilarVertex->Position[0] = NewPosition[0];
						pSimilarVertex->Position[1] = NewPosition[1];
						pSimilarVertex->Position[2] = NewPosition[2];

						pSimilarVertex->Normal[0] = NewNormal[0];
						pSimilarVertex->Normal[1] = NewNormal[1];
						pSimilarVertex->Normal[2] = NewNormal[2];

						pSimilarVertex->BoneWeights[0] = NewBoneWeights[0];
						pSimilarVertex->BoneWeights[1] = NewBoneWeights[1];
						pSimilarVertex->BoneWeights[2] = NewBoneWeights[2];
						pSimilarVertex->BoneWeights[3] = NewBoneWeights[3];

						pSimilarVertex->BoneIndices[0] = NewBoneIndices[0];
						pSimilarVertex->BoneIndices[1] = NewBoneIndices[1];
						pSimilarVertex->BoneIndices[2] = NewBoneIndices[2];
						pSimilarVertex->BoneIndices[3] = NewBoneIndices[3];
					}

					// clear list
					SimilarVertices.clear();
				}
			}
		}
	}
}


void M2Lib::M2::FixSeamsClothing(Float32 PositionalTolerance, Float32 AngularTolerance)
{
	CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();

	UInt32 SubMeshListLength = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].Count;
	M2SkinElement::CElement_SubMesh* SubMeshList = Skins[0]->Elements[M2SkinElement::EElement_SubMesh].as<M2SkinElement::CElement_SubMesh>();

	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshBodyList;	// gathered body sub meshes
	std::vector< M2SkinElement::CElement_SubMesh* > SubMeshGarbList;	// gathered clothing sub meshes

	for (UInt32 i = 0; i < SubMeshListLength; i++)
	{
		UInt16 ThisID = SubMeshList[i].ID;

		// gather body sub meshes
		UInt16 Mod = ThisID;
		if (Mod > 10)
		{
			Mod %= 10;
		}
		if (ThisID == 0 || (ThisID > 10 && Mod == 1))
		{
			SubMeshBodyList.push_back(&SubMeshList[i]);
		}
		// gather clothing sub meshes
		else if (
			ThisID == 402 ||	// cloth glove
			ThisID == 403 ||	// leather glove
			ThisID == 404 ||	// plate glove
			ThisID == 802 ||	// straight sleeve
			ThisID == 803 ||	// shaped sleeve
			ThisID == 902 ||	// low pant
			ThisID == 903 ||	// hight pant
			ThisID == 502 ||	// cloth boot
			ThisID == 503 ||	// leather boot
			ThisID == 504 ||	// plate boot
			ThisID == 505 ||	// plate boot 2
			ThisID == 1002 ||	// shirt frill short
			ThisID == 1102 ||	// shirt frill long
			ThisID == 1104 ||	// plate leg
			ThisID == 1202 ||	// tabard
			ThisID == 1302		// skirt
			//ThisID == 1802	// plate belt
			)
		{
			SubMeshGarbList.push_back(&SubMeshList[i]);
		}
	}

	// copy vertex properties from main body vertex to duplicate clothing vertices
	for (UInt32 iSubMeshGarb = 0; iSubMeshGarb < SubMeshGarbList.size(); iSubMeshGarb++)
	{
		M2SkinElement::CElement_SubMesh* pSubMeshGarb = SubMeshGarbList[iSubMeshGarb];
		for (UInt32 iSubMeshBody = 0; iSubMeshBody < SubMeshBodyList.size(); iSubMeshBody++)
		{
			M2SkinElement::CElement_SubMesh* pSubMeshBody = SubMeshBodyList[iSubMeshBody];

			for (SInt32 iVertexGarb = pSubMeshGarb->VertexStart; iVertexGarb < pSubMeshGarb->VertexStart + pSubMeshGarb->VertexCount; iVertexGarb++)
			{
				for (SInt32 iVertexBody = pSubMeshBody->VertexStart; iVertexBody < pSubMeshBody->VertexStart + pSubMeshBody->VertexCount; iVertexBody++)
				{
					if (CVertex::CompareSimilar(VertexList[iVertexGarb], VertexList[iVertexBody], false, false, PositionalTolerance, AngularTolerance))
					{
						// copy position, normal, and bone weights, and bone indices from body vertex to other(clothing) vertex
						CVertex* pVertexOther = &VertexList[iVertexGarb];
						CVertex* pVertexBody = &VertexList[iVertexBody];

						pVertexOther->Position[0] = pVertexBody->Position[0];
						pVertexOther->Position[1] = pVertexBody->Position[1];
						pVertexOther->Position[2] = pVertexBody->Position[2];

						pVertexOther->Normal[0] = pVertexBody->Normal[0];
						pVertexOther->Normal[1] = pVertexBody->Normal[1];
						pVertexOther->Normal[2] = pVertexBody->Normal[2];

						pVertexOther->BoneWeights[0] = pVertexBody->BoneWeights[0];
						pVertexOther->BoneWeights[1] = pVertexBody->BoneWeights[1];
						pVertexOther->BoneWeights[2] = pVertexBody->BoneWeights[2];
						pVertexOther->BoneWeights[3] = pVertexBody->BoneWeights[3];

						pVertexOther->BoneIndices[0] = pVertexBody->BoneIndices[0];
						pVertexOther->BoneIndices[1] = pVertexBody->BoneIndices[1];
						pVertexOther->BoneIndices[2] = pVertexBody->BoneIndices[2];
						pVertexOther->BoneIndices[3] = pVertexBody->BoneIndices[3];
					}
				}
			}
		}
	}
}

void M2Lib::M2::Scale(Float32 Scale)
{
	// vertices
	{
		UInt32 VertexListLength = Elements[EElement_Vertex].Count;
		CVertex* VertexList = Elements[EElement_Vertex].as<CVertex>();
		for (UInt32 i = 0; i < VertexListLength; i++)
		{
			CVertex& Vertex = VertexList[i];
			Vertex.Position[0] *= Scale;
			Vertex.Position[1] *= Scale;
			Vertex.Position[2] *= Scale;
		}
	}

	// bones
	{
		UInt32 BoneListLength = Elements[EElement_Bone].Count;
		CElement_Bone* BoneList = Elements[EElement_Bone].as<CElement_Bone>();
		for (UInt32 i = 0; i < BoneListLength; i++)
		{
			CElement_Bone& Bone = BoneList[i];
			Bone.Position[0] *= Scale;
			Bone.Position[1] *= Scale;
			Bone.Position[2] *= Scale;
		}
	}

	// attachments
	{
		UInt32 AttachmentListLength = Elements[EElement_Attachment].Count;
		CElement_Attachment* AttachmentList = Elements[EElement_Attachment].as<CElement_Attachment>();
		for (UInt32 i = 0; i < AttachmentListLength; i++)
		{
			CElement_Attachment& Attachment = AttachmentList[i];
			Attachment.Position[0] *= Scale;
			Attachment.Position[1] *= Scale;
			Attachment.Position[2] *= Scale;
		}
	}

	// events
	{
		UInt32 EventListLength = Elements[EElement_Event].Count;
		CElement_Event* EventList = Elements[EElement_Event].as<CElement_Event>();
		for (UInt32 i = 0; i < EventListLength; i++)
		{
			CElement_Event& Event = EventList[i];
			Event.Position[0] *= Scale;
			Event.Position[1] *= Scale;
			Event.Position[2] *= Scale;
		}
	}

	// lights
	{
		UInt32 LightListLength = Elements[EElement_Light].Count;
		CElement_Light* LightList = Elements[EElement_Light].as<CElement_Light>();
		for (UInt32 i = 0; i < LightListLength; i++)
		{
			CElement_Light& Light = LightList[i];
			Light.Position[0] *= Scale;
			Light.Position[1] *= Scale;
			Light.Position[2] *= Scale;
		}
	}

	// cameras
	{
		UInt32 CameraListLength = Elements[EElement_Camera].Count;
		CElement_Camera* CameraList = Elements[EElement_Camera].as<CElement_Camera>();
		for (UInt32 i = 0; i < CameraListLength; i++)
		{
			CElement_Camera& Camera = CameraList[i];
			Camera.Position[0] *= Scale;
			Camera.Position[1] *= Scale;
			Camera.Position[2] *= Scale;
			Camera.Target[0] *= Scale;
			Camera.Target[1] *= Scale;
			Camera.Target[2] *= Scale;
		}
	}

	// ribbon emitters
	{
		UInt32 RibbonEmitterListLength = Elements[EElement_RibbonEmitter].Count;
		CElement_RibbonEmitter* RibbonEmitterList = Elements[EElement_RibbonEmitter].as<CElement_RibbonEmitter>();
		for (UInt32 i = 0; i < RibbonEmitterListLength; i++)
		{
			CElement_RibbonEmitter& RibbonEmitter = RibbonEmitterList[i];
			RibbonEmitter.Position[0] *= Scale;
			RibbonEmitter.Position[1] *= Scale;
			RibbonEmitter.Position[2] *= Scale;
		}
	}

	// particle emitters
	{
		UInt32 ParticleEmitterListLength = Elements[EElement_ParticleEmitter].Count;
		CElement_ParticleEmitter* ParticleEmitterList = Elements[EElement_ParticleEmitter].as<CElement_ParticleEmitter>();
		for (UInt32 i = 0; i < ParticleEmitterListLength; i++)
		{
			CElement_ParticleEmitter& ParticleEmitter = ParticleEmitterList[i];
			ParticleEmitter.Position[0] *= Scale;
			ParticleEmitter.Position[1] *= Scale;
			ParticleEmitter.Position[2] *= Scale;
		}
	}
}


void M2Lib::M2::MirrorCamera()
{
	CElement_Camera* Cameras = Elements[EElement_Camera].as<CElement_Camera>();
	UInt32 CameraCount = Elements[EElement_Camera].Count;
	for (UInt32 iCamera = 0; iCamera < CameraCount; iCamera++)
	{
		if (Cameras->Type == 0)
		{
			Cameras->Position[1] = -Cameras->Position[1];
			Cameras->Target[1] = -Cameras->Target[1];
			break;
		}
	}

}


void M2Lib::M2::m_LoadElements_CopyHeaderToElements()
{
	Elements[EElement_Name].Count = Header.Description.nName;
	Elements[EElement_Name].Offset = Header.Description.oName;

	Elements[EElement_GlobalSequence].Count = Header.Elements.nGlobalSequence;
	Elements[EElement_GlobalSequence].Offset = Header.Elements.oGlobalSequence;

	Elements[EElement_Animation].Count = Header.Elements.nAnimation;
	Elements[EElement_Animation].Offset = Header.Elements.oAnimation;

	Elements[EElement_AnimationLookup].Count = Header.Elements.nAnimationLookup;
	Elements[EElement_AnimationLookup].Offset = Header.Elements.oAnimationLookup;

	Elements[EElement_Bone].Count = Header.Elements.nBone;
	Elements[EElement_Bone].Offset = Header.Elements.oBone;

	Elements[EElement_KeyBoneLookup].Count = Header.Elements.nKeyBoneLookup;
	Elements[EElement_KeyBoneLookup].Offset = Header.Elements.oKeyBoneLookup;

	Elements[EElement_Vertex].Count = Header.Elements.nVertex;
	Elements[EElement_Vertex].Offset = Header.Elements.oVertex;

	Elements[EElement_Color].Count = Header.Elements.nColor;
	Elements[EElement_Color].Offset = Header.Elements.oColor;

	Elements[EElement_Texture].Count = Header.Elements.nTexture;
	Elements[EElement_Texture].Offset = Header.Elements.oTexture;

	Elements[EElement_Transparency].Count = Header.Elements.nTransparency;
	Elements[EElement_Transparency].Offset = Header.Elements.oTransparency;

	Elements[EElement_TextureAnimation].Count = Header.Elements.nTextureAnimation;
	Elements[EElement_TextureAnimation].Offset = Header.Elements.oTextureAnimation;

	Elements[EElement_TextureReplace].Count = Header.Elements.nTextureReplace;
	Elements[EElement_TextureReplace].Offset = Header.Elements.oTextureReplace;

	Elements[EElement_TextureFlags].Count = Header.Elements.nTextureFlags;
	Elements[EElement_TextureFlags].Offset = Header.Elements.oTextureFlags;

	Elements[EElement_SkinnedBoneLookup].Count = Header.Elements.nSkinnedBoneLookup;
	Elements[EElement_SkinnedBoneLookup].Offset = Header.Elements.oSkinnedBoneLookup;

	Elements[EElement_TextureLookup].Count = Header.Elements.nTextureLookup;
	Elements[EElement_TextureLookup].Offset = Header.Elements.oTextureLookup;

	Elements[EElement_TextureUnitLookup].Count = Header.Elements.nTextureUnitLookup;
	Elements[EElement_TextureUnitLookup].Offset = Header.Elements.oTextureUnitLookup;

	Elements[EElement_TransparencyLookup].Count = Header.Elements.nTransparencyLookup;
	Elements[EElement_TransparencyLookup].Offset = Header.Elements.oTransparencyLookup;

	Elements[EElement_TextureAnimationLookup].Count = Header.Elements.nTextureAnimationLookup;
	Elements[EElement_TextureAnimationLookup].Offset = Header.Elements.oTextureAnimationLookup;

	Elements[EElement_BoundingTriangle].Count = Header.Elements.nBoundingTriangle;
	Elements[EElement_BoundingTriangle].Offset = Header.Elements.oBoundingTriangle;

	Elements[EElement_BoundingVertex].Count = Header.Elements.nBoundingVertex;
	Elements[EElement_BoundingVertex].Offset = Header.Elements.oBoundingVertex;

	Elements[EElement_BoundingNormal].Count = Header.Elements.nBoundingNormal;
	Elements[EElement_BoundingNormal].Offset = Header.Elements.oBoundingNormal;

	Elements[EElement_Attachment].Count = Header.Elements.nAttachment;
	Elements[EElement_Attachment].Offset = Header.Elements.oAttachment;

	Elements[EElement_AttachmentLookup].Count = Header.Elements.nAttachmentLookup;
	Elements[EElement_AttachmentLookup].Offset = Header.Elements.oAttachmentLookup;

	Elements[EElement_Event].Count = Header.Elements.nEvent;
	Elements[EElement_Event].Offset = Header.Elements.oEvent;

	Elements[EElement_Light].Count = Header.Elements.nLight;
	Elements[EElement_Light].Offset = Header.Elements.oLight;

	Elements[EElement_Camera].Count = Header.Elements.nCamera;
	Elements[EElement_Camera].Offset = Header.Elements.oCamera;

	Elements[EElement_CameraLookup].Count = Header.Elements.nCameraLookup;
	Elements[EElement_CameraLookup].Offset = Header.Elements.oCameraLookup;

	Elements[EElement_RibbonEmitter].Count = Header.Elements.nRibbonEmitter;
	Elements[EElement_RibbonEmitter].Offset = Header.Elements.oRibbonEmitter;

	Elements[EElement_ParticleEmitter].Count = Header.Elements.nParticleEmitter;
	Elements[EElement_ParticleEmitter].Offset = Header.Elements.oParticleEmitter;

	Elements[EElement_Unknown1].Count = Header.Elements.nUnknown1;
	Elements[EElement_Unknown1].Offset = Header.Elements.oUnknown1;
}


void M2Lib::M2::m_LoadElements_FindSizes(UInt32 ChunkSize)
{
	for (UInt32 i = 0; i < EElement__Count__; ++i)
	{
		auto& Element = Elements[i];

		Element.OffsetOriginal = Elements[i].Offset;

		if (!Element.Count || !Element.Offset)
		{
			Element.Data.clear();
			Element.SizeOriginal = 0;
			continue;
		}

		UInt32 NextOffset = ChunkSize;
		for (UInt32 j = 0; j < EElement__Count__; ++j)
		{
			if (Elements[j].Count && Elements[j].Offset > Element.Offset)
			{
				if (Elements[j].Offset < NextOffset)
					NextOffset = Elements[j].Offset;
				break;
			}
		}
		Element.Data.resize(NextOffset - Element.Offset);
		Element.SizeOriginal = Element.Data.size();
	}
}


#define VERIFY_OFFSET_LOCAL( offset ) \
	assert( !offset || Elements[iElement].Offset <= offset && offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size() );
#define VERIFY_OFFSET_NOTLOCAL( offset ) \
	assert( !offset || offset >= Elements[iElement].OffsetOriginal + Elements[iElement].Data.size() );

void M2Lib::M2::m_SaveElements_FindOffsets()
{
	// fix animation offsets and find element offsets
	SInt32 CurrentOffset = 0;
	if (Header.IsLongHeader())
		CurrentOffset = sizeof(CM2Header) + 8;	// +8 to align data at 16 byte bounds
	else
		CurrentOffset = sizeof(CM2Header) - 8;	// -8 because last 2 UInt32s are not saved

	// totaldiff needed to fix animations that are in the end of a chunk
	SInt32 totalDiff = -(SInt32)m_OriginalModelChunkSize + (Header.IsLongHeader() ? sizeof(Header) : sizeof(Header) - 8);
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
		totalDiff += Elements[iElement].Data.size();

	SInt32 OffsetDelta = 0;
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
	{
		// if this element has data...
		if (Elements[iElement].Data.empty())
		{
			Elements[iElement].Offset = 0;
			continue;
		}

		// if the current element's current offset doesn't match the calculated offset, some data has resized and we need to fix...
		OffsetDelta = CurrentOffset - Elements[iElement].Offset;

		switch (iElement)
		{
			case EElement_Name:
			case EElement_GlobalSequence:
			case EElement_Animation:
			case EElement_AnimationLookup:
				break;

			case EElement_Bone:
			{
				CElement_Bone* Bones = Elements[iElement].as<CElement_Bone>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Rotation, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Bones[j].AnimationBlock_Scale, iElement);
				}
				break;
			}
			case EElement_KeyBoneLookup:
			case EElement_Vertex:
				break;

			case EElement_Color:
			{
				CElement_Color* Colors = Elements[iElement].as<CElement_Color>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Colors[j].AnimationBlock_Opacity, iElement);
				}
				break;
			}

			case EElement_Texture:
			{
				CElement_Texture* Textures = Elements[iElement].as<CElement_Texture>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					VERIFY_OFFSET_LOCAL(Textures[j].TexturePath.Offset);
					Textures[j].TexturePath.Offset += OffsetDelta;
				}
				break;
			}

			case EElement_Transparency:
			{
				CElement_Transparency* Transparencies = Elements[iElement].as<CElement_Transparency>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Transparencies[j].AnimationBlock_Transparency, iElement);
				}
				break;
			}
			case EElement_TextureAnimation:
			{
				CElement_UVAnimation* Animations = Elements[iElement].as<CElement_UVAnimation>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Rotation, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Animations[j].AnimationBlock_Scale, iElement);
				}
				break;
			}

			case EElement_TextureReplace:
			case EElement_TextureFlags:
			case EElement_SkinnedBoneLookup:
			case EElement_TextureLookup:
			case EElement_TextureUnitLookup:
			case EElement_TransparencyLookup:
			case EElement_TextureAnimationLookup:
			case EElement_BoundingTriangle:
			case EElement_BoundingVertex:
			case EElement_BoundingNormal:
				break;

			case EElement_Attachment:
			{
				CElement_Attachment* Attachments = Elements[iElement].as<CElement_Attachment>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Attachments[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_AttachmentLookup:
				break;

			case EElement_Event:
			{
				CElement_Event* Events = Elements[iElement].as<CElement_Event>();
				auto animations = Elements[EElement_Animation].as<CElement_Animation>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
					m_FixAnimationM2Array(OffsetDelta, totalDiff, Events[j].GlobalSequenceID, Events[j].TimeLines, iElement);

				break;
			}

			case EElement_Light:
			{
				CElement_Light* Lights = Elements[iElement].as<CElement_Light>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AmbientColor, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AmbientIntensity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_DiffuseColor, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_DiffuseIntensity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AttenuationStart, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_AttenuationEnd, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Lights[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_Camera:
			{
				CElement_Camera* Cameras = Elements[iElement].as<CElement_Camera>();
				for (UInt32 j = 0; j < Elements[iElement].Count; ++j)
				{
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Position, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Target, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_Roll, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, Cameras[j].AnimationBlock_FieldOfView, iElement);
				}
				break;
			}

			case EElement_CameraLookup:
				break;

			case EElement_RibbonEmitter:
			{
				// untested!
				CElement_RibbonEmitter* RibbonEmitters = Elements[iElement].as<CElement_RibbonEmitter>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					VERIFY_OFFSET_LOCAL(RibbonEmitters[j].Texture.Offset);
					RibbonEmitters[j].Texture.Offset += OffsetDelta;
					VERIFY_OFFSET_LOCAL(RibbonEmitters[j].RenderFlag.Offset);
					RibbonEmitters[j].RenderFlag.Offset += OffsetDelta;

					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Color, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Opacity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Above, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Below, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Unknown1, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, RibbonEmitters[j].AnimationBlock_Visibility, iElement);
				}
				break;
			}

			case EElement_ParticleEmitter:
			{
				CElement_ParticleEmitter* ParticleEmitters = Elements[iElement].as<CElement_ParticleEmitter>();
				for (UInt32 j = 0; j < Elements[iElement].Count; j++)
				{
					if (ParticleEmitters[j].FileNameModel.Count)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].FileNameModel.Offset);
						ParticleEmitters[j].FileNameModel.Offset += OffsetDelta;
					}
					else
						ParticleEmitters[j].FileNameModel.Offset = 0;

					if (ParticleEmitters[j].ChildEmitter.Count)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].ChildEmitter.Offset);
						ParticleEmitters[j].ChildEmitter.Offset += OffsetDelta;
					}
					else
						ParticleEmitters[j].ChildEmitter.Offset = 0;

					if (ParticleEmitters[j].Unk.Count)
					{
						VERIFY_OFFSET_LOCAL(ParticleEmitters[j].Unk.Offset);
						ParticleEmitters[j].Unk.Offset += OffsetDelta;
					}
					else
						ParticleEmitters[j].Unk.Offset = 0;

					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitSpeed, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_SpeedVariance, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_VerticalRange, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_HorizontalRange, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Gravity, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Lifespan, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitRate, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitLength, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_EmitWidth, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_GravityStrong, iElement);
					m_FixAnimationOffsets(OffsetDelta, totalDiff, ParticleEmitters[j].AnimationBlock_Visibility, iElement);

					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].ColorTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].AlphaTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].ScaleTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].HeadCellTrack, iElement);
					m_FixFakeAnimationBlockOffsets(OffsetDelta, ParticleEmitters[j].TailCellTrack, iElement);
				}
				break;
			}
		}

		// set the element's new offset
		Elements[iElement].Offset = CurrentOffset;
		Elements[iElement].SizeOriginal = Elements[iElement].Data.size();
		Elements[iElement].OffsetOriginal = CurrentOffset;
		CurrentOffset += Elements[iElement].Data.size();
	}

	m_OriginalModelChunkSize = (Header.IsLongHeader() ? sizeof(Header) : sizeof(Header) - 8);
	for (UInt32 iElement = 0; iElement < EElement__Count__; ++iElement)
		m_OriginalModelChunkSize += Elements[iElement].Data.size();
}

void M2Lib::M2::m_FixAnimationM2Array(SInt32 OffsetDelta, SInt32 TotalDelta, SInt16 GlobalSequenceID, M2Array& Array, SInt32 iElement)
{
#define IS_LOCAL_ANIMATION(Offset) \
	(Offset >= Elements[iElement].OffsetOriginal && (Offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size()))

	auto animations = Elements[EElement_Animation].as<CElement_Animation>();

	VERIFY_OFFSET_LOCAL(Array.Offset);

	if (GlobalSequenceID == -1)
	{
		if (Array.Count)
		{
			auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);

			assert(Elements[EElement_Animation].Count != 0);
			for (SInt32 i = 0; i < Array.Count; ++i)
			{
				if (!animations[i].IsInternal())
					continue;

				//SubArrays[i].Shift(IS_LOCAL_ANIMATION(SubArrays[i].Offset) ? OffsetDelta : TotalDelta);
				if (SubArrays[i].Offset >= Elements[iElement].OffsetOriginal && (SubArrays[i].Offset < Elements[iElement].OffsetOriginal + Elements[iElement].Data.size()))
					SubArrays[i].Shift(OffsetDelta);
				else
					SubArrays[i].Shift(TotalDelta);
			}
		}

		Array.Shift(IS_LOCAL_ANIMATION(Array.Offset) ? OffsetDelta : TotalDelta);
	}
	else
	{
		auto SubArrays = (M2Array*)Elements[iElement].GetLocalPointer(Array.Offset);
		for (SInt32 i = 0; i < Array.Count; ++i)
			SubArrays[i].Shift(IS_LOCAL_ANIMATION(SubArrays[i].Offset) ? OffsetDelta : TotalDelta);

		Array.Shift(IS_LOCAL_ANIMATION(Array.Offset) ? OffsetDelta : TotalDelta);
	}
}

void M2Lib::M2::m_FixAnimationOffsets(SInt32 OffsetDelta, SInt32 TotalDelta, CElement_AnimationBlock& AnimationBlock, SInt32 iElement)
{
	m_FixAnimationM2Array(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Times, iElement);
	m_FixAnimationM2Array(OffsetDelta, TotalDelta, AnimationBlock.GlobalSequenceID, AnimationBlock.Keys, iElement);
}

void M2Lib::M2::m_FixFakeAnimationBlockOffsets(SInt32 OffsetDelta, CElement_FakeAnimationBlock& AnimationBlock, SInt32 iElement)
{
	// TP is the best
	if (AnimationBlock.Times.Count)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.Times.Offset);

		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.Times.Offset) && (AnimationBlock.Times.Offset < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.Times.Offset);
		assert(AnimationBlock.Times.Offset > 0);
		AnimationBlock.Times.Offset += OffsetDelta;
	}

	if (AnimationBlock.Keys.Count)
	{
		VERIFY_OFFSET_LOCAL(AnimationBlock.Keys.Offset);
		bool bInThisElem = (Elements[iElement].Offset < AnimationBlock.Keys.Offset) && (AnimationBlock.Keys.Offset < (Elements[iElement].Offset + Elements[iElement].Data.size()));
		assert(bInThisElem);

		VERIFY_OFFSET_LOCAL(AnimationBlock.Keys.Offset);
		assert(AnimationBlock.Keys.Offset > 0);
		AnimationBlock.Keys.Offset += OffsetDelta;
	}
}

void M2Lib::M2::m_SaveElements_CopyElementsToHeader()
{
	Header.Description.nName = Elements[EElement_Name].Count;
	Header.Description.oName = Elements[EElement_Name].Offset;

	Header.Elements.nGlobalSequence = Elements[EElement_GlobalSequence].Count;
	Header.Elements.oGlobalSequence = Elements[EElement_GlobalSequence].Offset;

	Header.Elements.nAnimation = Elements[EElement_Animation].Count;
	Header.Elements.oAnimation = Elements[EElement_Animation].Offset;

	Header.Elements.nAnimationLookup = Elements[EElement_AnimationLookup].Count;
	Header.Elements.oAnimationLookup = Elements[EElement_AnimationLookup].Offset;

	Header.Elements.nBone = Elements[EElement_Bone].Count;
	Header.Elements.oBone = Elements[EElement_Bone].Offset;

	Header.Elements.nKeyBoneLookup = Elements[EElement_KeyBoneLookup].Count;
	Header.Elements.oKeyBoneLookup = Elements[EElement_KeyBoneLookup].Offset;

	Header.Elements.nVertex = Elements[EElement_Vertex].Count;
	Header.Elements.oVertex = Elements[EElement_Vertex].Offset;

	Header.Elements.nColor = Elements[EElement_Color].Count;
	Header.Elements.oColor = Elements[EElement_Color].Offset;

	Header.Elements.nTexture = Elements[EElement_Texture].Count;
	Header.Elements.oTexture = Elements[EElement_Texture].Offset;

	Header.Elements.nTransparency = Elements[EElement_Transparency].Count;
	Header.Elements.oTransparency = Elements[EElement_Transparency].Offset;

	Header.Elements.nTextureAnimation = Elements[EElement_TextureAnimation].Count;
	Header.Elements.oTextureAnimation = Elements[EElement_TextureAnimation].Offset;

	Header.Elements.nTextureReplace = Elements[EElement_TextureReplace].Count;
	Header.Elements.oTextureReplace = Elements[EElement_TextureReplace].Offset;

	Header.Elements.nTextureFlags = Elements[EElement_TextureFlags].Count;
	Header.Elements.oTextureFlags = Elements[EElement_TextureFlags].Offset;

	Header.Elements.nSkinnedBoneLookup = Elements[EElement_SkinnedBoneLookup].Count;
	Header.Elements.oSkinnedBoneLookup = Elements[EElement_SkinnedBoneLookup].Offset;

	Header.Elements.nTextureLookup = Elements[EElement_TextureLookup].Count;
	Header.Elements.oTextureLookup = Elements[EElement_TextureLookup].Offset;

	Header.Elements.nTextureUnitLookup = Elements[EElement_TextureUnitLookup].Count;
	Header.Elements.oTextureUnitLookup = Elements[EElement_TextureUnitLookup].Offset;

	Header.Elements.nTransparencyLookup = Elements[EElement_TransparencyLookup].Count;
	Header.Elements.oTransparencyLookup = Elements[EElement_TransparencyLookup].Offset;

	Header.Elements.nTextureAnimationLookup = Elements[EElement_TextureAnimationLookup].Count;
	Header.Elements.oTextureAnimationLookup = Elements[EElement_TextureAnimationLookup].Offset;

	Header.Elements.nBoundingTriangle = Elements[EElement_BoundingTriangle].Count;
	Header.Elements.oBoundingTriangle = Elements[EElement_BoundingTriangle].Offset;

	Header.Elements.nBoundingVertex = Elements[EElement_BoundingVertex].Count;
	Header.Elements.oBoundingVertex = Elements[EElement_BoundingVertex].Offset;

	Header.Elements.nBoundingNormal = Elements[EElement_BoundingNormal].Count;
	Header.Elements.oBoundingNormal = Elements[EElement_BoundingNormal].Offset;

	Header.Elements.nAttachment = Elements[EElement_Attachment].Count;
	Header.Elements.oAttachment = Elements[EElement_Attachment].Offset;

	Header.Elements.nAttachmentLookup = Elements[EElement_AttachmentLookup].Count;
	Header.Elements.oAttachmentLookup = Elements[EElement_AttachmentLookup].Offset;

	Header.Elements.nEvent = Elements[EElement_Event].Count;
	Header.Elements.oEvent = Elements[EElement_Event].Offset;

	Header.Elements.nLight = Elements[EElement_Light].Count;
	Header.Elements.oLight = Elements[EElement_Light].Offset;

	Header.Elements.nCamera = Elements[EElement_Camera].Count;
	Header.Elements.oCamera = Elements[EElement_Camera].Offset;

	Header.Elements.nCameraLookup = Elements[EElement_CameraLookup].Count;
	Header.Elements.oCameraLookup = Elements[EElement_CameraLookup].Offset;

	Header.Elements.nRibbonEmitter = Elements[EElement_RibbonEmitter].Count;
	Header.Elements.oRibbonEmitter = Elements[EElement_RibbonEmitter].Offset;

	Header.Elements.nParticleEmitter = Elements[EElement_ParticleEmitter].Count;
	Header.Elements.oParticleEmitter = Elements[EElement_ParticleEmitter].Offset;

	Header.Elements.nUnknown1 = Elements[EElement_Unknown1].Count;
	Header.Elements.oUnknown1 = Elements[EElement_Unknown1].Offset;
}

SInt32 M2Lib::M2::m_GetChunkIndex(UInt32 ChunkID) const
{
	for (int a = 0; a < EChunk__Count__; ++a)
	{
		if (kChunkIDs[a] == ChunkID)
			return a;
	}

	return -1;
}

UInt32 M2Lib::M2::AddTexture(const Char8* szTextureSource, CElement_Texture::ETextureType Type, CElement_Texture::ETextureFlags Flags)
{
	auto& Element = Elements[EElement_Texture];

	// shift offsets for existing textures
	for (UInt32 i = 0; i < Element.Count; ++i)
	{
		auto& texture = Element.as<CElement_Texture>()[i];
		if (texture.TexturePath.Offset)
			texture.TexturePath.Offset += sizeof(CElement_Texture);
	}

	// add element placeholder for new texture
	Element.Data.insert(Element.Data.begin() + Element.Count * sizeof(CElement_Texture), sizeof(CElement_Texture), 0);
	auto texturePathPos = Element.Data.size();
	// add placeholder for texture path
	Element.Data.insert(Element.Data.end(), strlen(szTextureSource) + 1, 0);

	auto newIndex = Element.Count;

	CElement_Texture& newTexture = Element.as<CElement_Texture>()[newIndex];
	newTexture.Type = Type;
	newTexture.Flags = Flags;
	newTexture.TexturePath.Count = strlen(szTextureSource) + 1;
	newTexture.TexturePath.Offset = Element.Offset + texturePathPos;

	memcpy(&Element.Data[texturePathPos], szTextureSource, newTexture.TexturePath.Count);

	++Element.Count;

	return newIndex;
}

UInt32 M2Lib::M2::GetTexture(const Char8* szTextureSource)
{
	auto& Element = Elements[EElement_Texture];

	for (UInt32 i = 0; i < Element.Count; ++i)
	{
		auto& texture = Element.as<CElement_Texture>()[i];
		if (texture.TexturePath.Offset)
		{
			auto pTexturePath = (Char8 const*)Element.GetLocalPointer(texture.TexturePath.Offset);
			if (_strcmpi(pTexturePath, szTextureSource) == 0)
				return i;
		}
	}

	return -1;
}

UInt32 M2Lib::M2::AddTextureLookup(UInt16 TextureId, bool ForceNewIndex /*= false*/)
{
	auto& Element = Elements[EElement_TextureLookup];

	if (!ForceNewIndex)
	{
		for (UInt32 i = 0; i < Element.Count; ++i)
		{
			auto& lookup = Element.as<CElement_TextureLookup>()[i];
			if (lookup.TextureIndex == TextureId)
				return i;
		}
	}

	// add element placeholder for new lookup
	Element.Data.insert(Element.Data.begin() + Element.Count * sizeof(CElement_TextureLookup), sizeof(CElement_TextureLookup), 0);

	auto newIndex = Element.Count;
	CElement_TextureLookup& newLookup = Element.as<CElement_TextureLookup>()[newIndex];
	newLookup.TextureIndex = TextureId;

	++Element.Count;
	return newIndex;
}

