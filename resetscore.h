#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>
#include <sh_vector.h>
#include "utlvector.h"
#include "ehandle.h"
#include <iserver.h>
#include <entity2/entitysystem.h>
#include "igameevents.h"
#include "vector.h"
#include <deque>
#include <functional>
#include "sdk/utils.hpp"
#include <utlstring.h>
#include <KeyValues.h>
#include "sdk/schemasystem.h"
#include "sdk/CBaseEntity.h"
#include "sdk/CGameRulesProxy.h"
#include "sdk/CBasePlayerPawn.h"
#include "sdk/CCSPlayerController.h"
#include "sdk/CCSPlayer_ItemServices.h"
#include "sdk/CSmokeGrenadeProjectile.h"
#include "sdk/module.h"
#include "include/mysql_mm.h"
#include "include/menus.h"
#include <map>
#include <ctime>
#include <chrono>
#include <array>
#include <thread>
#include <deque>
#include <functional>
#include <string>
#include <sstream>
#include <vector>

class RS final : public ISmmPlugin, public IMetamodListener
{
public:
	bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late);
	bool Unload(char* error, size_t maxlen);
private:
	const char* GetAuthor();
	const char* GetName();
	const char* GetDescription();
	const char* GetURL();
	const char* GetLicense();
	const char* GetVersion();
	const char* GetDate();
	const char* GetLogTag();

private:
	void StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);
    void OnDispatchConCommand(ConCommandHandle cmd, const CCommandContext& ctx, const CCommand& args);
};

const std::string colors_text[] = {
	"{DEFAULT}",
	"{RED}",
	"{LIGHTPURPLE}",
	"{GREEN}",
	"{LIME}",
	"{LIGHTGREEN}",
	"{LIGHTRED}",
	"{GRAY}",
	"{LIGHTOLIVE}",
	"{OLIVE}",
	"{LIGHTBLUE}",
	"{BLUE}",
	"{PURPLE}",
	"{GRAYBLUE}",
	"\\n"
};

const std::string colors_hex[] = {
	"\x01",
	"\x02",
	"\x03",
	"\x04",
	"\x05",
	"\x06",
	"\x07",
	"\x08",
	"\x09",
	"\x10",
	"\x0B",
	"\x0C",
	"\x0E",
	"\x0A",
	"\xe2\x80\xa9"
};

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
