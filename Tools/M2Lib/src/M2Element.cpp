#include "M2Element.h"
#include <string>

using namespace M2Lib;

std::string M2Element::CElement_Texture::GetTypeString(ETextureType Type)
{
#define _CASE_STR(x) case ETextureType::x: return #x;
	switch (Type)
	{
		_CASE_STR(Final_Hardcoded)
		_CASE_STR(Skin)
		_CASE_STR(ObjectSkin)
		_CASE_STR(WeaponBlade)
		_CASE_STR(WeaponHandle)
		_CASE_STR(Environment)
		_CASE_STR(Hair)
		_CASE_STR(FacialHair)
		_CASE_STR(SkinExtra)
		_CASE_STR(UiSkin)
		_CASE_STR(TaurenMane)
		_CASE_STR(Monster1)
		_CASE_STR(Monster2)
		_CASE_STR(Monster3)
		_CASE_STR(ItemIcon)
		_CASE_STR(GuildBackgroundColor)
		_CASE_STR(GuildEmblemColor)
		_CASE_STR(GuildEmblem)
		default:
			return "#" + std::to_string((UInt32)Type);
	}
}
