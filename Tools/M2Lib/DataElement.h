#pragma once

#include "BaseTypes.h"
#include <fstream>
#include <vector>
#include <assert.h>

namespace M2Lib
{

	class DataElement
	{
	public:
		UInt32 Count;				// number of sub-elements contained in Data. the definition of this structure depends on this element's usage.
		UInt32 Offset;				// offset in bytes from begining of file to where this element's data begins.

		UInt32 OffsetOriginal;		// offset of this element as loaded from the original file.
		UInt32 SizeOriginal;		// size of this element as loaded from the original file.

		std::vector<UInt8> Data;	// our local copy of data. note that DataSize might be greater than sizeof( DataType ) * Count if there is animation data references or padding at the end.
		SInt32 Align;				// byte alignment boundary. M2s pad the ends of elements with zeros data so they align on 16 byte boundaries.

	public:
		DataElement();
		~DataElement();

		// given a global offset, returns a pointer to the data contained in this Element.
		// asserts if GlobalOffset lies outside of this element.
		void* GetLocalPointer(UInt32 GlobalOffset);

		// loads this element's data from a file stream. assumes that Offset and DataSize have already been set.
		bool Load(std::fstream& FileStream);
		// loads this element's data from memory. assumes that Offset and DataSize have already been set.
		bool Load(UInt8 const* RawData);
		// saves this element's data to a file stream. assumes that Offset and DataSize have already been set.
		bool Save(std::fstream& FileStream);

		// reallocates Data, either erasing existing data or preserving it.
		// adds padding to NewDataSize if necessary so that new size aligns with Align.
		void SetDataSize(UInt32 NewCount, UInt32 NewDataSize, bool CopyOldData);
		// clears element
		void Clear();

		template <class T>
		T* as() { return (T*)Data.data(); }

		template <class T>
		std::vector<T> asVector()
		{
			assert(sizeof(T) * Count <= Data.size());

			std::vector<T> ret(Count);
			if (!Data.empty())
				memcpy(ret.data(), Data.data(), sizeof(T) * Count);

			return ret;
		}

		// clones this element from Source to Destination.
		static void Clone(DataElement* Source, DataElement* Destination);

		static void SetFileOffset(UInt32 offset) { FileOffset = offset; }
	private:
		static UInt32 FileOffset;

	};

}
