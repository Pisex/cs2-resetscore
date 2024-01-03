#pragma once
#include <entity2/entityidentity.h>
#include <baseentity.h>
#include "schemasystem.h"

class CNetworkTransmitComponent {};

class SC_CBaseEntity : public CBaseEntity
{
public:
	SCHEMA_FIELD(int32_t, CBaseEntity, m_iHealth);
	SCHEMA_FIELD(int32_t, CBaseEntity, m_iMaxHealth);
	SCHEMA_FIELD(uint8_t, CBaseEntity, m_iTeamNum);
	SCHEMA_FIELD(LifeState_t, CBaseEntity, m_lifeState);
	SCHEMA_FIELD(float, CBaseEntity, m_flGravityScale);
	SCHEMA_FIELD(float, CBaseEntity, m_lastNetworkChange);
	SCHEMA_FIELD(CBitVec<64>, CBaseEntity, m_isSteadyState);
	SCHEMA_FIELD(CNetworkTransmitComponent, CBaseEntity, m_NetworkTransmitComponent);
};