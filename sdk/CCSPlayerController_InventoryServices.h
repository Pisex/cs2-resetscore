#pragma once
#include "schemasystem.h"

// enum class MedalRank_t : uint32_t
// {
// 	MEDAL_RANK_NONE = 0x0,
// 	MEDAL_RANK_BRONZE = 0x1,
// 	MEDAL_RANK_SILVER = 0x2,
// 	MEDAL_RANK_GOLD = 0x3,
// 	MEDAL_RANK_COUNT = 0x4,
// };

class CCSPlayerController_InventoryServices
{
public:
	// SCHEMA_FIELD(int32_t, CCSPlayerController_InGameMoneyServices, m_iAccount);
	// SCHEMA_FIELD(int32_t, CCSPlayerController_InGameMoneyServices, m_iStartAccount);
	SCHEMA_FIELD(uint16_t, CCSPlayerController_InventoryServices, m_unMusicID);
	SCHEMA_FIELD(int32_t, CCSPlayerController_InventoryServices, m_nPersonaDataPublicLevel);
	SCHEMA_FIELD(int[6], CCSPlayerController_InventoryServices, m_rank);
};