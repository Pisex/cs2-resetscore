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

IUtilsApi* g_pUtils;

CGameEntitySystem* GameEntitySystem()
{
	return g_pUtils->GetCGameEntitySystem();
}

void ClientPrint(int iSlot, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[256], buf2[256];
	V_vsnprintf(buf, sizeof(buf), msg, args);
	va_end(args);
	
	g_SMAPI->Format(buf2, sizeof(buf2), "%s %s", g_vecPhrases[std::string("Prefix")].c_str(), buf);
	g_pUtils->PrintToChat(iSlot, buf2);
}

bool OnRsCommand(int iSlot, const char* szContent)
{
	ClientPrint(iSlot, g_vecPhrases[std::string("Success")].c_str());
	auto pController = (CCSPlayerController*)g_pEntitySystem->GetBaseEntity((CEntityIndex)(iSlot + 1));
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
	g_pUtils->SetStateChanged(pController, "CCSPlayerController", "m_pInventoryServices");
	return false;
}

void StartupServer()
{
	g_pGameEntitySystem = GameEntitySystem();
	g_pEntitySystem = g_pUtils->GetCEntitySystem();
	gpGlobals = g_pUtils->GetCGlobalVars();
}

void RS::AllPluginsLoaded()
{
	char error[64] = { 0 };
	int ret;
	g_pUtils = (IUtilsApi *)g_SMAPI->MetaFactory(Utils_INTERFACE, &ret, NULL);
	if (ret == META_IFACE_FAILED)
	{
		V_strncpy(error, "Missing Utils system plugin", 64);
		ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", GetLogTag(), error);
		std::string sBuffer = "meta unload "+std::to_string(g_PLID);
		engine->ServerCommand(sBuffer.c_str());
		return;
	}
	g_pUtils->RegCommand(g_PLID, {"mm_rs", "sm_rs", "rs"}, {"!rs", "rs"}, OnRsCommand);
	g_pUtils->StartupServer(g_PLID, StartupServer);
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

	return true;
}

bool RS::Unload(char *error, size_t maxlen)
{
	ConVar_Unregister();
	
	return true;
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
