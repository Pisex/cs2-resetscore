#pragma once
#include "CCSPlayerController_InGameMoneyServices.h"
#include "CBasePlayerController.h"
#include "CCSPlayerPawn.h"
#include "ehandle.h"
#include "schemasystem.h"

class CCSPlayerController_InventoryServices;

struct CSPerRoundStats_t
{
public:
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iKills);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iDeaths);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iAssists);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iDamage);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iEquipmentValue);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iMoneySaved);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iKillReward);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iLiveTime);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iHeadShotKills);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iObjective);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iCashEarned);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iUtilityDamage);
	SCHEMA_FIELD(int, CSPerRoundStats_t, m_iEnemiesFlashed);
};

class CCSPlayerController_ActionTrackingServices
{
public:
	SCHEMA_FIELD(CSPerRoundStats_t, CCSPlayerController_ActionTrackingServices, m_matchStats);
};

class CCSPlayerController : public CBasePlayerController
{
public:
	SCHEMA_FIELD(CCSPlayerController_InventoryServices*, CCSPlayerController, m_pInventoryServices);
	SCHEMA_FIELD(int32_t, CCSPlayerController, m_iCompetitiveRanking);
	SCHEMA_FIELD(int32_t, CCSPlayerController, m_iCompetitiveRankingPredicted_Win);
	SCHEMA_FIELD(int32_t, CCSPlayerController, m_iCompetitiveRankingPredicted_Loss);
	SCHEMA_FIELD(int32_t, CCSPlayerController, m_iCompetitiveRankingPredicted_Tie);
	SCHEMA_FIELD(int8_t,  CCSPlayerController, m_iCompetitiveRankType);
	SCHEMA_FIELD(int, CCSPlayerController, m_iScore);
	SCHEMA_FIELD(int, CCSPlayerController, m_iMVPs);
	SCHEMA_FIELD(CCSPlayerController_InGameMoneyServices*, CCSPlayerController, m_pInGameMoneyServices);
	SCHEMA_FIELD(CUtlSymbolLarge, CCSPlayerController, m_szClan);
	SCHEMA_FIELD(char[32], CCSPlayerController, m_szClanName);
	SCHEMA_FIELD(CHandle<CCSPlayerPawn>, CCSPlayerController, m_hPlayerPawn);

	SCHEMA_FIELD(CCSPlayerController_ActionTrackingServices*, CCSPlayerController, m_pActionTrackingServices);

	void ChangeTeam(int iTeam)
	{
		CALL_VIRTUAL(void, 89, this, iTeam);
	}
};