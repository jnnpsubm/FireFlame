#include "DataElement.h"
#include <assert.h>
#include <string.h>
#include <algorithm>

UInt32 M2Lib::DataElement::FileOffset = 0;

M2Lib::DataElement::DataElement()
	: Count(0)
	, Offset(0)
	, Align(16)
{
}

M2Lib::DataElement::~DataElement()
{
}

void* M2Lib::DataElement::GetLocalPointer(UInt32 GlobalOffset)
{
	assert(GlobalOffset >= Offset);
	GlobalOffset -= Offset;
	assert(GlobalOffset < (UInt32)Data.size());
	return &Data[GlobalOffset];
}

bool M2Lib::DataElement::Load(std::fstream& FileStream)
{
	if (Data.empty())
		return true;

	FileStream.seekg(Offset + FileOffset, std::ios::beg);
	FileStream.read((Char8*)Data.data(), Data.size());

	return true;
}

bool M2Lib::DataElement::Load(UInt8 const* RawData)
{
	if (Data.empty())
		return true;

	memcpy(Data.data(), RawData + Offset + FileOffset, Data.size());
	return true;
}

bool M2Lib::DataElement::Save(std::fstream& FileStream)
{
	if (Data.empty())
		return true;

	FileStream.seekp(Offset + FileOffset);
	FileStream.write((Char8*)Data.data(), Data.size());

	return true;
}
void M2Lib::DataElement::Clear()
{
	Data.clear();
	Count = 0;
}

void M2Lib::DataElement::SetDataSize(UInt32 NewCount, UInt32 NewDataSize, bool CopyOldData)
{
	if (Align != 0)
	{
		UInt32 Mod = NewDataSize % Align;
		if (Mod)
			NewDataSize += Align - Mod;
	}

	std::vector<UInt8> NewData(NewDataSize, 0);
	if (CopyOldData && !Data.empty())
		memcpy(NewData.data(), Data.data(), Data.size() > NewDataSize ? NewDataSize : Data.size());

	Data = NewData;
	Count = NewCount;
}

void M2Lib::DataElement::Clone(DataElement* Source, DataElement* Destination)
{
	Destination->SetDataSize(Source->Count, Source->Data.size(), false);
	memcpy(Destination->Data.data(), Source->Data.data(), Source->Data.size());
}
