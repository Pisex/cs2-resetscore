#include <stdio.h>
#include "resetscore.h"
#include "metamod_oslink.h"
#include "sdk/CCSPlayerController_InventoryServices.h"

RS g_RS;
PLUGIN_EXPOSE(RS, g_RS);
IVEngineServer2* engine = nullptr;
IGameEventManager2* gameeventmanager = nullptr;
INetworkGameServer *g_pNetworkGameServer = nullptr;
IGameResourceServiceServer* g_pGameResourceService = nullptr;
CGameEntitySystem* g_pGameEntitySystem = nullptr;
CEntitySystem* g_pEntitySystem = nullptr;
CSchemaSystem* g_pCSchemaSystem = nullptr;
CGlobalVars *gpGlobals = nullptr;

std::map<std::string, std::string> g_vecPhrases;

class GameSessionConfiguration_t { };
SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);
SH_DECL_HOOK4_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0, CPlayerSlot, char const *, int, uint64);
SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandHandle, const CCommandContext&, const CCommand&);
SH_DECL_HOOK5_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0, CPlayerSlot, int, const char *, uint64, const char *);
SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&, ISource2WorldSession*, const char*);

void (*UTIL_ClientPrint)(CBasePlayerController *player, int msg_dest, const char* msg_name, const char* param1, const char* param2, const char* param3, const char* param4) = nullptr;
void (*UTIL_StateChanged)(CNetworkTransmitComponent& networkTransmitComponent, CEntityInstance *ent, int64 offset, int16 a4, int16 a5) = nullptr;
void (*UTIL_NetworkStateChanged)(int64 chainEntity, int64 offset, int64 a3) = nullptr;

std::string Colorizer(std::string str)
{
	for (int i = 0; i < std::size(colors_hex); i++)
	{
		size_t pos = 0;

		while ((pos = str.find(colors_text[i], pos)) != std::string::npos)
		{
			str.replace(pos, colors_text[i].length(), colors_hex[i]);
			pos += colors_hex[i].length();
		}
	}

	return str;
}

void ClientPrint(int iSlot, int hud_dest, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[256], buf2[256];
	V_vsnprintf(buf, sizeof(buf), msg, args);
	va_end(args);
	
	if(hud_dest == 3) g_SMAPI->Format(buf2, sizeof(buf2), "%s %s", g_vecPhrases[std::string("Prefix")].c_str(), buf);
	else g_SMAPI->Format(buf2, sizeof(buf2), "%s", buf);

	CCSPlayerController* pPlayerController = static_cast<CCSPlayerController*>(g_pEntitySystem->GetBaseEntity(static_cast<CEntityIndex>(iSlot + 1)));
	if (!pPlayerController || !pPlayerController->m_hPlayerPawn())
	{
		ConMsg("%s\n", buf2);
		return;
	}

	uint32 m_steamID = pPlayerController->m_steamID();
	
	if(m_steamID <= 0)
	{
		ConMsg("%s\n", buf2);
		return;
	}

	std::string colorizedBuf = Colorizer(buf2);

	UTIL_ClientPrint(pPlayerController, hud_dest, colorizedBuf.c_str(), nullptr, nullptr, nullptr, nullptr);
}

bool RS::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetEngineFactory, g_pCSchemaSystem, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, engine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, g_pSource2GameClients, IServerGameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceService, IGameResourceServiceServer, GAMERESOURCESERVICESERVER_INTERFACE_VERSION);

	CModule libserver(g_pSource2Server);
	UTIL_ClientPrint = libserver.FindPatternSIMD(WIN_LINUX("48 85 C9 0F 84 2A 2A 2A 2A 48 8B C4 48 89 58 18", "55 48 89 E5 41 57 49 89 CF 41 56 49 89 D6 41 55 41 89 F5 41 54 4C 8D A5 A0 FE FF FF")).RCast< decltype(UTIL_ClientPrint) >();
	if (!UTIL_ClientPrint)
	{
		V_strncpy(error, "Failed to find function to get UTIL_ClientPrint", maxlen);
		ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);
		std::string sBuffer = "meta unload "+std::to_string(g_PLID);
		engine->ServerCommand(sBuffer.c_str());
		return false;
	}

	UTIL_StateChanged = libserver.FindPatternSIMD("55 48 89 E5 41 57 41 56 41 55 41 54 53 89 D3").RCast< decltype(UTIL_StateChanged) >();
	if (!UTIL_StateChanged)
	{
		V_strncpy(error, "Failed to find function to get UTIL_StateChanged", maxlen);
		ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);
		std::string sBuffer = "meta unload "+std::to_string(g_PLID);
		engine->ServerCommand(sBuffer.c_str());
		return false;
	}

	UTIL_NetworkStateChanged = libserver.FindPatternSIMD("83 FF 07 0F 87 ? ? ? ? 55 89 FF 48 89 E5 41 56 41 55 41 54 49 89 F4 53 48 89 D3 48 8D 15 E5 C2 20 00").RCast< decltype(UTIL_NetworkStateChanged) >();
	if (!UTIL_NetworkStateChanged)
	{
		V_strncpy(error, "Failed to find function to get UTIL_NetworkStateChanged", maxlen);
		ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);
		std::string sBuffer = "meta unload "+std::to_string(g_PLID);
		engine->ServerCommand(sBuffer.c_str());
		return false;
	}
	
	KeyValues::AutoDelete g_kvPhrases("Phrases");
	const char *pszPath = "addons/translations/resetscore.phrases.txt";

	if (!g_kvPhrases->LoadFromFile(g_pFullFileSystem, pszPath))
	{
		Warning("Failed to load %s\n", pszPath);
		return false;
	}

	std::string szLanguage = std::string(g_kvPhrases->GetString("language", "en"));
	const char* g_pszLanguage = szLanguage.c_str();
	for (KeyValues *pKey = g_kvPhrases->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey())
		g_vecPhrases[std::string(pKey->GetName())] = std::string(pKey->GetString(g_pszLanguage));


	g_SMAPI->AddListener( this, this );

	SH_ADD_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &RS::StartupServer), true);
	SH_ADD_HOOK_MEMFUNC(ICvar, DispatchConCommand, g_pCVar, this, &RS::OnDispatchConCommand, false);

	if (late)
	{
		g_pGameEntitySystem = *reinterpret_cast<CGameEntitySystem**>(reinterpret_cast<uintptr_t>(g_pGameResourceService) + WIN_LINUX(0x58, 0x50));
		g_pEntitySystem = g_pGameEntitySystem;
		g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
		gpGlobals = g_pNetworkGameServer->GetGlobals();
	}

	return true;
}

bool RS::Unload(char *error, size_t maxlen)
{
	SH_REMOVE_HOOK(INetworkServerService, StartupServer, g_pNetworkServerService, SH_MEMBER(this, &RS::StartupServer), true);
	SH_REMOVE_HOOK_MEMFUNC(ICvar, DispatchConCommand, g_pCVar, this, &RS::OnDispatchConCommand, false);

	ConVar_Unregister();
	
	return true;
}

void SetStateChanged(SC_CBaseEntity* entity, const char* sClassName, const char* sFieldName, int extraOffset = 0)
{
	int offset = g_pCSchemaSystem->GetServerOffset(sClassName, sFieldName);
	int chainOffset = g_pCSchemaSystem->GetServerOffset(sClassName, "__m_pChainEntity");
	if (chainOffset != -1)
	{
		UTIL_NetworkStateChanged((uintptr_t)(entity) + chainOffset, offset, 0xFFFFFFFF);
		return;
	}
	UTIL_StateChanged(entity->m_NetworkTransmitComponent(), entity, offset, -1, -1);
	entity->m_lastNetworkChange() = gpGlobals->curtime;
	entity->m_isSteadyState().ClearAll();
}

void RS::OnDispatchConCommand(ConCommandHandle cmdHandle, const CCommandContext& ctx, const CCommand& args)
{
	if (!g_pEntitySystem)
		return;

	auto iCommandPlayerSlot = ctx.GetPlayerSlot();

	bool bSay = !V_strcmp(args.Arg(0), "say");
	bool bTeamSay = !V_strcmp(args.Arg(0), "say_team");

	if (iCommandPlayerSlot != -1 && (bSay || bTeamSay))
	{
		auto pController = (CCSPlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(iCommandPlayerSlot.Get() + 1));
		bool bCommand = *args[1] == '!' || *args[1] == '/';
		bool bSilent = *args[1] == '/';

		if (bCommand)
		{
			char *pszMessage = (char *)(args.ArgS() + 2);
			CCommand arg;
			arg.Tokenize(args.ArgS() + 2);
			if(arg[0][0])
			{
				if(!strcmp(arg[0], "rs") || !strcmp(arg[0], "кы"))
				{
					ClientPrint(iCommandPlayerSlot.Get(), 3, g_vecPhrases[std::string("Success")].c_str());
					CCSPlayerController_ActionTrackingServices* m_ATS = pController->m_pActionTrackingServices();
					pController->m_iScore() = 0;
					pController->m_iMVPs() = 0;
					m_ATS->m_matchStats().m_iKills() = 0;
					m_ATS->m_matchStats().m_iDeaths() = 0;
					m_ATS->m_matchStats().m_iAssists() = 0;
					m_ATS->m_matchStats().m_iDamage() = 0;
					m_ATS->m_matchStats().m_iEquipmentValue() = 0;
					m_ATS->m_matchStats().m_iMoneySaved() = 0;
					m_ATS->m_matchStats().m_iKillReward() = 0;
					m_ATS->m_matchStats().m_iLiveTime() = 0;
					m_ATS->m_matchStats().m_iHeadShotKills() = 0;
					m_ATS->m_matchStats().m_iObjective() = 0;
					m_ATS->m_matchStats().m_iCashEarned() = 0;
					m_ATS->m_matchStats().m_iUtilityDamage() = 0;
					m_ATS->m_matchStats().m_iEnemiesFlashed() = 0;
					SetStateChanged(pController, "CCSPlayerController", "m_pInventoryServices");
				}
			}

			RETURN_META(MRES_SUPERCEDE);
		}
	}
	SH_CALL(g_pCVar, &ICvar::DispatchConCommand)(cmdHandle, ctx, args);
}

void RS::StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*)
{
	static bool bDone = false;
	if (!bDone)
	{
		g_pGameEntitySystem = *reinterpret_cast<CGameEntitySystem**>(reinterpret_cast<uintptr_t>(g_pGameResourceService) + WIN_LINUX(0x58, 0x50));
		g_pEntitySystem = g_pGameEntitySystem;

		g_pNetworkGameServer = g_pNetworkServerService->GetIGameServer();
		gpGlobals = g_pNetworkGameServer->GetGlobals();

		bDone = true;
	}
}

///////////////////////////////////////
const char* RS::GetLicense()
{
	return "GPL";
}

const char* RS::GetVersion()
{
	return "1.0";
}

const char* RS::GetDate()
{
	return __DATE__;
}

const char *RS::GetLogTag()
{
	return "RS";
}

const char* RS::GetAuthor()
{
	return "Pisex";
}

const char* RS::GetDescription()
{
	return "ResetScore";
}

const char* RS::GetName()
{
	return "ResetScore";
}

const char* RS::GetURL()
{
	return "https://discord.gg/g798xERK5Y";
}
