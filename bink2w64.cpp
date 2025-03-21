#include <Windows.h>
#include <DbgHelp.h>
#include <vector>
#include <fstream>
#include "ReplaceImport.h"
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "detours.h"
#include <ctime>
#include <Windows.h>
#include <Psapi.h>
#include <string>
using namespace std;

std::vector<const char*> sImportFunctionNames =
{
"BinkAllocateFrameBuffers",
"BinkClose",
"BinkCloseTrack",
"BinkControlBackgroundIO",
"BinkCopyToBuffer",
"BinkCopyToBufferRect",
"BinkDoFrame",
"BinkDoFrameAsync",
"BinkDoFrameAsyncMulti",
"BinkDoFrameAsyncWait",
"BinkDoFramePlane",
"BinkFreeGlobals",
"BinkGetError",
"BinkGetFrameBuffersInfo",
"BinkGetGPUDataBuffersInfo",
"BinkGetKeyFrame",
"BinkGetPlatformInfo",
"BinkGetRealtime",
"BinkGetRects",
"BinkGetSummary",
"BinkGetTrackData",
"BinkGetTrackID",
"BinkGetTrackMaxSize",
"BinkGetTrackType",
"BinkGoto",
"BinkLogoAddress",
"BinkNextFrame",
"BinkOpen",
"BinkOpenDirectSound",
"BinkOpenMiles",
"BinkOpenTrack",
"BinkOpenWaveOut",
"BinkOpenWithOptions",
"BinkOpenXAudio2",
"BinkOpenXAudio27",
"BinkOpenXAudio28",
"BinkPause",
"BinkRegisterFrameBuffers",
"BinkRegisterGPUDataBuffers",
"BinkRequestStopAsyncThread",
"BinkRequestStopAsyncThreadsMulti",
"BinkService",
"BinkSetError",
"BinkSetFileOffset",
"BinkSetFrameRate",
"BinkSetIO",
"BinkSetIOSize",
"BinkSetMemory",
"BinkSetOSFileCallbacks",
"BinkSetPan",
"BinkSetSimulate",
"BinkSetSoundOnOff",
"BinkSetSoundSystem",
"BinkSetSoundSystem2",
"BinkSetSoundTrack",
"BinkSetSpeakerVolumes",
"BinkSetVideoOnOff",
"BinkSetVolume",
"BinkSetWillLoop",
"BinkShouldSkip",
"BinkStartAsyncThread",
"BinkUtilCPUs",
"BinkUtilFree",
"BinkUtilMalloc",
"BinkUtilMutexCreate",
"BinkUtilMutexDestroy",
"BinkUtilMutexLock",
"BinkUtilMutexLockTimeOut",
"BinkUtilMutexUnlock",
"BinkWait",
"BinkWaitStopAsyncThread",
"BinkWaitStopAsyncThreadsMulti",
"RADTimerRead"
};

extern "C" uintptr_t	iImportFunctions[73] = { 0 };

HINSTANCE				pOriginalHinst = nullptr;
HINSTANCE				pWarpperHinst = nullptr;
std::vector<HINSTANCE>  loadedPlugins;

PROC					Init_Original = nullptr;


std::string GetPluginsDirectory()
{
	return "NativeMods\\";
}

void Error(const char* msg)
{
	MessageBoxA(nullptr, msg, "NativeModLoader", 0);
}

int LoadDLLPlugin(const char* path)
{
	int state = -1;
	__try
	{
		HINSTANCE plugin = LoadLibraryA(path);
		if (!plugin)
			return 0;

		int ok = 1;
		FARPROC fnInit = GetProcAddress(plugin, "Init");
		if (fnInit != nullptr)
		{
			state = -2;
			((void(__cdecl*)())fnInit)();
			ok = 2;
		}

		loadedPlugins.push_back(plugin);
		return ok;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}

	return state;
}

//
//void LoadLib()
//{
//	static bool isLoaded = false;
//	if (!isLoaded)
//	{
//		isLoaded = true;
//
//		std::ofstream fLog = std::ofstream("NativeModLoader.log");
//		WIN32_FIND_DATA wfd;
//		std::string dir = GetPluginsDirectory();
//		std::string search_dir = dir + "*.dll";
//		HANDLE hFind = FindFirstFileA(search_dir.c_str(), &wfd);
//		if (hFind != INVALID_HANDLE_VALUE)
//		{
//			int i_error = 0;
//			do
//			{
//				if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
//					continue;
//
//				std::string name = wfd.cFileName;
//				name = dir + name;
//
//				if (fLog.good())
//					fLog << "Checking \"" << name.c_str() << "\" ... ";
//
//				int result = LoadDLLPlugin(name.c_str());
//				switch (result)
//				{
//				case 2:
//				{
//					if (fLog.good())
//						fLog << "OK - loaded and called Init().\n";
//					break;
//				}
//
//				case 1:
//				{
//					if (fLog.good())
//						fLog << "OK - loaded.\n";
//					break;
//				}
//
//				case 0:
//				{
//					if (fLog.good())
//						fLog << "LoadLibrary failed!\n";
//					i_error = 1;
//					std::string err = "LoadLibrary failed on ";
//					err = err + name;
//					Error(err.c_str());
//					break;
//				}
//
//				case -1:
//				{
//					if (fLog.good())
//						fLog << "LoadLibrary crashed! This means there's a problem in the plugin DLL file.\n";
//					i_error = 1;
//					std::string err = "LoadLibrary crashed on ";
//					err = err + name;
//					err = err + ". This means there's a problem in the plugin DLL file. Contact the author of that plugin.";
//					Error(err.c_str());
//					break;
//				}
//
//				case -2:
//				{
//					if (fLog.good())
//						fLog << "Init() crashed! This means there's a problem in the plugin DLL file.\n";
//					i_error = 1;
//					std::string err = "Init() crashed on ";
//					err = err + name;
//					err = err + ". This means there's a problem in the plugin DLL file. Contact the author of that plugin.";
//					Error(err.c_str());
//					break;
//				}
//				}
//			} while (i_error == 0 && FindNextFileA(hFind, &wfd));
//
//			FindClose(hFind);
//		}
//		else
//		{
//			if (fLog.good())
//				fLog << "Failed to get search handle to \"" << search_dir.c_str() << "\"!\n";
//		}
//	}
//}

PVOID Init_Hook(PVOID arg1, PVOID arg2)
{
	//LoadLib();

	return ((PVOID(*)(PVOID, PVOID))Init_Original)(arg1, arg2);
}




// Define the original function pointer type
typedef __int64(__fastcall* IsSessionMode)();
typedef const char* (__fastcall* OriginalSubFunction)(__int64);
typedef __int64(__fastcall* OriginalLUI_CoD_LuaCall_IsUserSignedIn_Func)(long long, __int64);

// Declare the original function pointer
IsSessionMode SessionMode;
OriginalLUI_CoD_LuaCall_IsUserSignedIn_Func originalIsUserSignedIn_Func;



#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;
std::string userInput;

DWORD Old;

struct msg_t
{
	int overflowed;
	int readOnly;
	unsigned __int8* data;
	unsigned __int8* splitData;
	int maxsize;
	int cursize;
	int splitSize;
	int readcount;
	int bit;
	int lastEntityRef;
	int targetLocalNetID;
	unsigned int compressionFlags;
};

struct XUID {
	unsigned __int64 m_id;
	void Deserialize(const msg_t* msg);
	void Serialize(const msg_t* msg);
	XUID* FromHexString(const char* xuidString);
	XUID* FromMsg(const msg_t* msg);
	XUID* FromString(const char* xuidString);
	XUID* FromUInt64(unsigned __int64 id);
	XUID* FromUniversalId(unsigned __int64 id);

	unsigned __int64 GetUniversalId();

	bool IsNull();
	bool IsValid();

	XUID* NullXUID();
	XUID* RandomXUID();

	const char* ToDevString();
	const char* ToHexString();
	const char* ToString();

	bool operator !=(const XUID* xuid);
	XUID* operator =(const XUID* xuid);
	bool operator ==(const XUID* xuid);

};


union DvarValue
{
	bool enabled;
	int integer;
	unsigned int unsignedInt;
	__int64 integer64;
	unsigned __int64 unsignedInt64;
	float value;
	float vector[4];
	const char* string;
	unsigned __int8 color[4];
};

struct BbConstUsageFlags
{
	bool initialized;
	DvarValue codeValue;
};


struct dvar_t
{
	const char* name;
	unsigned int checksum;
	const char* description;
	unsigned int flags;
	char level[1];
	unsigned __int8 type;
	bool modified;
	unsigned __int16 hashNext;
	DvarValue current;
	DvarValue latched;
	DvarValue reset;
	char domain[0x10];
	BbConstUsageFlags BbConstUsageFlags;
};

uintptr_t baseAddress = 0;

//MW



//MW End
//dvar_t* Dvar_RegisterBool_Detour(const char* dvarName, bool value, unsigned int flags, const char* description)
typedef dvar_t* (__fastcall* Dvar_RegistBool)(const char*, bool, unsigned int, const char*);


Dvar_RegistBool Dvar_RegisterBoolean = reinterpret_cast<Dvar_RegistBool>(reinterpret_cast<BYTE*>(baseAddress) + 0x2D5F460);

HMODULE currentModule = 0;
DWORD WINAPI InitzializeMemoryBaseAddress(LPVOID)
{
	Sleep(3000);
	currentModule = GetModuleHandle(NULL);
	DWORD oldProtect;
	int value;
	int value1 = 0x1;
	if (currentModule != NULL) {
		MODULEINFO moduleInfo;
		if (GetModuleInformation(GetCurrentProcess(), currentModule, &moduleInfo, sizeof(moduleInfo))) {
			baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
			std::cout << "Base Address: " << std::hex << "0x" << baseAddress << std::endl;

			Dvar_RegisterBoolean = reinterpret_cast<Dvar_RegistBool>(reinterpret_cast<BYTE*>(baseAddress) + 0x2D5F460);

			return 0;

		}
	}
}



unsigned int* GetRandSeed() {
	auto func = reinterpret_cast<unsigned int* (*)()>(baseAddress + 0x2D4E350);
	return func();
}

unsigned __int64 Sys_Microseconds() {
	auto func = reinterpret_cast<unsigned __int64(*)()>(baseAddress + 0x2E67190);
	return func();
}

int I_irand(int min, int max) {
	auto func = reinterpret_cast<int(*)(int, int)>(baseAddress + 0x2D4E5D0);
	return func(min, max);
}

XUID* XUID::RandomXUID() {
	unsigned int* RandSeed;
	unsigned int BackupRandSeed;
	this->m_id = 0;
	RandSeed = GetRandSeed();
	BackupRandSeed = *RandSeed;
	*RandSeed = Sys_Microseconds();
	this->m_id = I_irand(1, 0x7FFFFFFF);
	*RandSeed = BackupRandSeed;
	return this;
}

void lua_remove(uintptr_t L, int idx) {
	auto func = reinterpret_cast<void(*)(uintptr_t, int)>(baseAddress + 0x5B57650);
	func(L, idx);
}

void lua_call(uintptr_t L, int nargs, int nresults) {
	auto func = reinterpret_cast<void(*)(uintptr_t, int, int)>(baseAddress + 0x5B56400);
	func(L, nargs, nresults);
}

void lua_getfield(uintptr_t L, int idx, const char* k) {
	auto func = reinterpret_cast<void(*)(uintptr_t, int, const char*)>(baseAddress + 0x5B56910);
	func(L, idx, k);
}


void lua_pushstring(uintptr_t L, const char* str) {
	auto func = reinterpret_cast<void(*)(uintptr_t, const char*)>(baseAddress + 0x5B57350);
	func(L, str);
}

int LuaShared_PCall(uintptr_t luaVM, int nargs, int nresults) {
	auto func = reinterpret_cast<int(*)(uintptr_t, int, int)>(baseAddress + 0x51F7F60);
	return func(luaVM, nargs, nresults);
}


void LUI_CoD_LuaCall_ExecNow(uintptr_t luaVM, const char* str) {
	lua_getfield(luaVM, -10002, "Engine");
	lua_getfield(luaVM, -1, "DAGFFDGFII");
	lua_remove(luaVM, -2);
	lua_pushstring(luaVM, str);
	LuaShared_PCall(luaVM, 1, 1);
}




void lua_pushboolean(uintptr_t L, int b) {
	auto func = reinterpret_cast<void(*)(uintptr_t, int)>(baseAddress + 0x5B570B0);
	func(L, b);
}

void Cbuf_AddText(const char* cmd) {
	uintptr_t luaVM = *reinterpret_cast<uintptr_t*>(baseAddress + 0x196349B8);
	if (luaVM) {
		LUI_CoD_LuaCall_ExecNow(luaVM, cmd);
	}
}

struct CmdArgs
{
	int nesting;
	int localClientNum[8];
	int controllerIndex[8];
	int argc[8];
	const char** argv[8];
	char textPool[16384];
	const char* argvPool[512];
	int usedTextPool[8];
	int totalUsedArgvPool;
	int totalUsedTextPool;
};

CmdArgs* cmd_args;



enum XAssetType
{
	ASSET_TYPE_PHYSICSLIBRARY = 0x0,
	ASSET_TYPE_PHYSICSSFXEVENTASSET = 0x1,
	ASSET_TYPE_PHYSICSVFXEVENTASSET = 0x2,
	ASSET_TYPE_PHYSICSASSET = 0x3,
	ASSET_TYPE_PHYSICSFXPIPELINE = 0x4,
	ASSET_TYPE_PHYSICSFXSHAPE = 0x5,
	ASSET_TYPE_PHYSICSDEBUGDATA = 0x6,
	ASSET_TYPE_XANIM = 0x7,
	ASSET_TYPE_XMODELSURFS = 0x8,
	ASSET_TYPE_XMODEL = 0x9,
	ASSET_TYPE_MAYHEM = 0xA,
	ASSET_TYPE_MATERIAL = 0xB,
	ASSET_TYPE_COMPUTESHADER = 0xC,
	ASSET_TYPE_LIBSHADER = 0xD,
	ASSET_TYPE_VERTEXSHADER = 0xE,
	ASSET_TYPE_HULLSHADER = 0xF,
	ASSET_TYPE_DOMAINSHADER = 0x10,
	ASSET_TYPE_PIXELSHADER = 0x11,
	ASSET_TYPE_TECHSET = 0x12,
	ASSET_TYPE_IMAGE = 0x13,
	ASSET_TYPE_SOUNDGLOBALS = 0x14,
	ASSET_TYPE_SOUNDBANK = 0x15,
	ASSET_TYPE_SOUNDBANKTRANSIENT = 0x16,
	ASSET_TYPE_COL_MAP = 0x17,
	ASSET_TYPE_COM_MAP = 0x18,
	ASSET_TYPE_GLASS_MAP = 0x19,
	ASSET_TYPE_AIPATHS = 0x1A,
	ASSET_TYPE_NAVMESH = 0x1B,
	ASSET_TYPE_TACGRAPH = 0x1C,
	ASSET_TYPE_MAP_ENTS = 0x1D,
	ASSET_TYPE_FX_MAP = 0x1E,
	ASSET_TYPE_GFX_MAP = 0x1F,
	ASSET_TYPE_GFX_MAP_TRZONE = 0x20,
	ASSET_TYPE_IESPROFILE = 0x21,
	ASSET_TYPE_LIGHTDEF = 0x22,
	ASSET_TYPE_GRADINGCLUT = 0x23,
	ASSET_TYPE_UI_MAP = 0x24,
	ASSET_TYPE_FOGSPLINE = 0x25,
	ASSET_TYPE_ANIMCLASS = 0x26,
	ASSET_TYPE_PLAYERANIM = 0x27,
	ASSET_TYPE_GESTURE = 0x28,
	ASSET_TYPE_LOCALIZE = 0x29,
	ASSET_TYPE_ATTACHMENT = 0x2A,
	ASSET_TYPE_WEAPON = 0x2B,
	ASSET_TYPE_VFX = 0x2C,
	ASSET_TYPE_IMPACTFX = 0x2D,
	ASSET_TYPE_SURFACEFX = 0x2E,
	ASSET_TYPE_AITYPE = 0x2F,
	ASSET_TYPE_MPTYPE = 0x30,
	ASSET_TYPE_CHARACTER = 0x31,
	ASSET_TYPE_XMODELALIAS = 0x32,
	ASSET_TYPE_RAWFILE = 0x33,
	ASSET_TYPE_SCRIPTFILE = 0x34,
	ASSET_TYPE_SCRIPTDEBUGDATA = 0x35,
	ASSET_TYPE_STRINGTABLE = 0x36,
	ASSET_TYPE_LEADERBOARDDEF = 0x37,
	ASSET_TYPE_VIRTUALLEADERBOARDDEF = 0x38,
	ASSET_TYPE_DDL = 0x39,
	ASSET_TYPE_TRACER = 0x3A,
	ASSET_TYPE_VEHICLE = 0x3B,
	ASSET_TYPE_ADDON_MAP_ENTS = 0x3C,
	ASSET_TYPE_NETCONSTSTRINGS = 0x3D,
	ASSET_TYPE_LUAFILE = 0x3E,
	ASSET_TYPE_SCRIPTABLE = 0x3F,
	ASSET_TYPE_EQUIPSNDTABLE = 0x40,
	ASSET_TYPE_VECTORFIELD = 0x41,
	ASSET_TYPE_PARTICLESIMANIMATION = 0x42,
	ASSET_TYPE_STREAMINGINFO = 0x43,
	ASSET_TYPE_LASER = 0x44,
	ASSET_TYPE_TTF = 0x45,
	ASSET_TYPE_SUIT = 0x46,
	ASSET_TYPE_SUITANIMPACKAGE = 0x47,
	ASSET_TYPE_CAMERA = 0x48,
	ASSET_TYPE_HUDOUTLINE = 0x49,
	ASSET_TYPE_SPACESHIPTARGET = 0x4A,
	ASSET_TYPE_RUMBLE = 0x4B,
	ASSET_TYPE_RUMBLEGRAPH = 0x4C,
	ASSET_TYPE_ANIMPKG = 0x4D,
	ASSET_TYPE_SFXPKG = 0x4E,
	ASSET_TYPE_VFXPKG = 0x4F,
	ASSET_TYPE_FOOTSTEPVFX = 0x50,
	ASSET_TYPE_BEHAVIORTREE = 0x51,
	ASSET_TYPE_AIANIMSET = 0x52,
	ASSET_TYPE_AIASM = 0x53,
	ASSET_TYPE_PROCEDURALBONES = 0x54,
	ASSET_TYPE_DYNAMICBONES = 0x55,
	ASSET_TYPE_RETICLE = 0x56,
	ASSET_TYPE_XANIMCURVE = 0x57,
	ASSET_TYPE_COVERSELECTOR = 0x58,
	ASSET_TYPE_ENEMYSELECTOR = 0x59,
	ASSET_TYPE_CLIENTCHARACTER = 0x5A,
	ASSET_TYPE_CLOTHASSET = 0x5B,
	ASSET_TYPE_CINEMATICMOTION = 0x5C,
	ASSET_TYPE_ACCESSORY = 0x5D,
	ASSET_TYPE_LOCDMGTABLE = 0x5E,
	ASSET_TYPE_BULLETPENETRATION = 0x5F,
	ASSET_TYPE_SCRIPTBUNDLE = 0x60,
	ASSET_TYPE_BLENDSPACE2D = 0x61,
	ASSET_TYPE_XCAM = 0x62,
	ASSET_TYPE_CAMO = 0x63,
	ASSET_TYPE_XCOMPOSITEMODEL = 0x64,
	ASSET_TYPE_XMODELDETAILCOLLISION = 0x65,
	ASSET_TYPE_STREAMKEY = 0x66,
	ASSET_TYPE_STREAMTREEOVERRIDE = 0x67,
	ASSET_TYPE_KEYVALUEPAIRS = 0x68,
	ASSET_TYPE_STTERRAIN = 0x69,
	ASSET_TYPE_NATIVESCRIPTPATCH = 0x6A,
	ASSET_TYPE_COLLISIONTILE = 0x6B,
	ASSET_TYPE_EXECUTION = 0x6C,
	ASSET_TYPE_CARRYOBJECT = 0x6D,
	ASSET_TYPE_SOUNDBANKLIST = 0x6E,
	ASSET_TYPE_DECALVOLUMEMATERIAL = 0x6F,
	ASSET_TYPE_DECALVOLUMEMASK = 0x70,
	ASSET_TYPE_DYNENTITYLIST = 0x71,
	ASSET_TYPE_FX_MAP_TRZONE = 0x72,
	ASSET_TYPE_DLOGSCHEMA = 0x73,
	ASSET_TYPE_EDGELIST = 0x74,
};


uintptr_t FS_ReadFile(const char* qpath, const char** buffer) {
	auto func = reinterpret_cast<uintptr_t(*)(const char* qpath, const char** buffer)>(baseAddress+0x2D49ED0);
	return func(qpath, buffer);
}
//
//
//void Cmd_Exec_Internal(bool isSuperUser)
//{
//	const char* cmdbuf;
//	const char* file;
//	auto DB_FindXAssetHeader = reinterpret_cast<uintptr_t(*)(XAssetType type, const char* givenName, int allowCreateDefault)>(baseAddress+0x288BF40);
//	auto DB_ReadRawFile = reinterpret_cast<const char* (*)(unsigned int a1, unsigned int a2, const char* a3, char a4)>(baseAddress+0x2A07220);
//	auto Core_strcpy_truncate = reinterpret_cast<bool(*)(char* dest, unsigned __int64 destsize, const char* src)>(baseAddress + 0x5B080F0);
//	auto Com_DefaultExtension = reinterpret_cast<void(*)(char* path, int maxSize, const char* extension)>(baseAddress+0x2D752D0);
//	char path[64];
//
//	if (cmd_args->argc[cmd_args->nesting] == 2)
//	{
//		Core_strcpy_truncate(path, 64, *(cmd_args->argv[cmd_args->nesting] + 1));
//		Com_DefaultExtension(path, 64, ".cfg");
//		if (DB_FindXAssetHeader(ASSET_TYPE_RAWFILE, path, 0))
//		{
//			if (!DB_ReadRawFile(0, cmd_args->controllerIndex[cmd_args->nesting], path, isSuperUser))
//			{
//				if (cmd_args->argc[cmd_args->nesting] <= 1)
//					file = "";
//				else
//					file = *(cmd_args->argv[cmd_args->nesting] + 1);
//				printf("couldn't exec %s\n", file);
//			}
//		}
//		else
//		{
//			FS_ReadFile(path, &cmdbuf);
//
//			LUI_CoD_LuaCall_ExecNow(*reinterpret_cast<uintptr_t*>(baseAddress + 0x196349B8), cmdbuf);
//		}
//	}
//	else
//	{
//		printf(0, "exec <filename> : execute a script file\n");
//	}
//}





// Detoured function for sub_60334950
__int64 IsConnectedToDWOrg(__int64 a1) {
	std::cout << "Detoured IsConnectedToDW" << std::endl;
	return true;
}

__int64 IsProfileSignedInOrg(uintptr_t luaState) {
	std::cout << "Detoured IsProfileSignedIn" << std::endl;
	lua_pushboolean(luaState, 1);
	return true;
}


__int64 IsUserSignedInToDemonwareOrg(uintptr_t luaState) {
	std::cout << "Detoured IsUserSignedInToDemonware" << std::endl;
	lua_pushboolean(luaState, 1);
	return true;
}

__int64 IsUserSignedInOrg(__int64 a1) {
	//std::cout << "Detoured IsUserSignedIn" << std::endl;
	return true;
}

__int64 IsUserSignedInToLiveOrg(uintptr_t luaState) {
	std::cout << "Detoured IsUserSignedInToLive" << std::endl;
	lua_pushboolean(luaState, 1);
	return true;
}

__int64 IsAnyUserSignedInOrg(__int64 a1) {
	std::cout << "Detoured IsAnyUserSignedIn" << std::endl;
	return true;
}

__int64 IsNetworkConnectedOrg(__int64 a1) {
	std::cout << "Detoured IsNetworkConnected" << std::endl;
	return false;
}

__int64 HasDisconnectedAfterSigninOrg(__int64 a1) {
	std::cout << "Detoured HasDisconnectedAfterSignin" << std::endl;
	return false;
}

__int64 IsConnectedToGameServerOrg(__int64 a1) {
	std::cout << "Detoured IsConnectedToGameServer" << std::endl;
	return true;
}

__int64 GetBattleNetConnectionStateOrg(__int64 a1) {
	std::cout << "Detoured GetBattleNetConnectionState" << std::endl;
	return 1;
}

__int64 HasOnlineServiceSubscriptionOrg(__int64 a1) {
	std::cout << "Detoured HasOnlineServiceSubscription" << std::endl;
	return true;
}

__int64 IsFullAccountOrg(__int64 a1) {
	std::cout << "Detoured IsFullAccount" << std::endl;
	return true;
}

__int64 IsOwnedOrg(__int64 a1) {
	//lua_pushboolean(luaState, 1);
	std::cout << "Detoured IsOwned" << std::endl;
	return true;
}

__int64 IsGuestOrg(__int64 a1) {
	std::cout << "Detoured IsGuest" << std::endl;
	return false;
}

__int64 DoWeHaveOnlineStatsOrg(__int64 a1) {
	std::cout << "Detoured DoWeHaveOnlineStats" << std::endl;
	return false;
}


__int64 DoWeHaveOnlineSubscriptionInformationOrg(__int64 a1) {
	std::cout << "Detoured DoWeHaveOnlineSubscriptionInformation" << std::endl;
	return false;
}

__int64 GetActiveStatsSourceOrg(__int64 a1) {
	std::cout << "Detoured GetActiveStatsSource" << std::endl;
	return 1;
}

__int64 OwnsBaseGameOrg(__int64 a1) {
	std::cout << "Detoured OwnsBaseGame" << std::endl;
	return true;
}

__int64 UserCanPlayOnlineOrg(__int64 a1) {
	std::cout << "Detoured UserCanPlayOnline" << std::endl;
	return true;
}

__int64 UserOwnsAppOrg(__int64 a1) {
	std::cout << "Detoured UserOwnsApp" << std::endl;
	return true;
}

__int64 Social_GetOnlineStatusForBNetOrg(__int64 a1) {
	std::cout << "Detoured Social_GetOnlineStatusForBNet" << std::endl;
	return 1;
}

__int64 GetGameBattleAccountStateOrg(__int64 a1) {
	std::cout << "Detoured GetGameBattleAccountState" << std::endl;
	return 1;
}

__int64 OnlineDataGetStateOrg(__int64 a1) {
	std::cout << "Detoured OnlineDataGetState" << std::endl;
	return 1;
}

__int64 OnlineServicesGetStateOrg(__int64 a1) {
	//std::cout << "Detoured OnlineServicesGetState" << std::endl;
	return 1;
}

__int64 ShouldBeInOnlineAreaOrg(__int64 a1) {
	std::cout << "Detoured ShouldBeInOnlineArea" << std::endl;
	return false;
}

__int64 IsInSignInQueueOrg(__int64 a1) {
	std::cout << "Detoured IsInSignInQueue" << std::endl;
	return false;
}

__int64 IsBattleNetAuthReadyOrg(__int64 a1) {
	//std::cout << "Detoured IsBattleNetAuthReady" << std::endl;
	return 1;
}

__int64 IsDemoBuildOrg(__int64 a1) {
	std::cout << "Detoured IsDemoBuildOrg" << std::endl;
	return false;
}

__int64 GetSignInStateOrg(__int64 a1) {
	std::cout << "Detoured GetSignInState" << std::endl;
	return 1;
}

__int64 IsBattleNetLanOnlyOrg(__int64 a1) {
	//a1 = 1;
	std::cout << "Detoured IsBattleNetLanOnly" << std::endl;
	return 1;
}

__int64 IsBattleNetOrg(__int64 a1) {
	//a1 = 1;
	// std::cout << "Detoured IsBattleNetOrg" << std::endl;
	return 1;
}

__int64 GetBattleNetFenceStateOrg(__int64 a1) {
	//a1 = 1;
	std::cout << "Detoured GetBattleNetFenceState" << std::endl;
	return 1;
}
__int64 OfflineDataFetchedOrg(__int64 a1) {
	//a1 = 1;
	std::cout << "Detoured OfflineDataFetched" << std::endl;
	return 1;
}

__int64 PlaylistsGetStateOrg(__int64 a1) {
	//a1 = 1;
	std::cout << "Detoured PlaylistsGetState" << std::endl;
	return 1;
}

__int64 FpGetLogonStatus() {
	//a1 = 1;
	//std::cout << "Detoured FpGetLogonStatus" << std::endl;
	return 2;
}


DWORD WINAPI DETTACH(LPVOID)
{
	

	return 0;
}

/*
Short note from me this address are all from 1.38 
*/

DWORD WINAPI HOOKS(LPVOID)
{
	Sleep(10000);

	std::cout << "[Debug] Hook Part ..." << std::endl;
	uintptr_t IsConnectedToDW = baseAddress + 0x22755C0;
	uintptr_t IsProfileSignedIn = baseAddress + 0x52834D0;
	uintptr_t IsUserSignedInToDemonware = baseAddress + 0x5282D60;
	uintptr_t IsUserSignedIn = baseAddress + 0x5282A90;
	uintptr_t IsUserSignedInToLive = baseAddress + 0x5282C00;
	uintptr_t IsAnyUserSignedIn = baseAddress + 0x5282830;
	uintptr_t IsNetworkConnected = baseAddress + 0x5282660;
	uintptr_t HasDisconnectedAfterSignin = baseAddress + 0x527FA70;
	uintptr_t IsConnectedToGameServer = baseAddress + 0x527ED60;
	uintptr_t GetBattleNetConnectionState = baseAddress + 0x527F910;
	uintptr_t HasOnlineServiceSubscription = baseAddress + 0x52836E0;
	uintptr_t IsFullAccount = baseAddress + 0x22C7810;
	uintptr_t IsOwned = baseAddress + 0x22D02E0;
	uintptr_t IsGuest = baseAddress + 0x28F3E20;
	uintptr_t DoWeHaveOnlineStats = baseAddress + 0x5282EF0;
	uintptr_t DoWeHaveOnlineSubscriptionInformation = baseAddress + 0x52837F0;
	uintptr_t GetActiveStatsSource = baseAddress + 0x52809F0;
	uintptr_t OwnsBaseGame = baseAddress + 0x5280E70;
	uintptr_t UserCanPlayOnline = baseAddress + 0x5283560;
	uintptr_t UserOwnsApp = baseAddress + 0x527F6D0;
	uintptr_t Social_GetOnlineStatusForBNet = baseAddress + 0x226AA00;
	uintptr_t GetGameBattleAccountState = baseAddress + 0x526B770;
	uintptr_t OnlineDataGetState = baseAddress + 0x54047E0;
	uintptr_t OnlineServicesGetState = baseAddress + 0x5404560;
	uintptr_t ShouldBeInOnlineArea = baseAddress + 0x52826E0;
	uintptr_t IsInSignInQueue = baseAddress + 0x527E620;
	uintptr_t IsBattleNetAuthReady = baseAddress + 0x527DFC0;
	uintptr_t GetSignInState = baseAddress + 0x527F880;
	uintptr_t IsBattleNetLanOnly = baseAddress + 0x5287E20;
	uintptr_t IsBattleNet = baseAddress + 0x5287B80;
	uintptr_t GetBattleNetFenceState = baseAddress + 0x527FB10;
	uintptr_t OfflineDataFetched = baseAddress + 0x5404F20;
	uintptr_t PlaylistsGetState = baseAddress + 0x5404FD0;
	uintptr_t fpGetLogonStatus = baseAddress + 0x4DA6180;
	uintptr_t ISPremiumPlayer = baseAddress + 0x5288040;
	uintptr_t IsDemoBuild = baseAddress + 0x51FBEB0;
	uintptr_t IsMapPackOwned = baseAddress + 0x527BB80;
	uintptr_t HasMultiLoginErrror = baseAddress + 0x54051C0;
	uintptr_t ShouldFenceSignInShowErrorPopup = baseAddress + 0x5405200;
	uintptr_t HAsErrors = baseAddress + 0x54E4B00;
	uintptr_t DoWeHaveContentPack = baseAddress + 0x2D557D0;
	uintptr_t CmdExecInternal = baseAddress + 0x2D557D0;
	


	//// Initialize Detours
	if (DetourTransactionBegin() == NO_ERROR &&
		DetourUpdateThread(GetCurrentThread()) == NO_ERROR &&
		/*DetourAttach(&(PVOID&)IsConnectedToDW, IsConnectedToDWOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsProfileSignedIn, IsProfileSignedInOrg) == NO_ERROR &&*/
		DetourAttach(&(PVOID&)IsUserSignedInToDemonware, IsUserSignedInToDemonwareOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsUserSignedIn, IsUserSignedInOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsUserSignedInToLive, IsUserSignedInToLiveOrg) == NO_ERROR &&
		/*DetourAttach(&(PVOID&)IsNetworkConnected, IsNetworkConnectedOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)HasDisconnectedAfterSignin, HasDisconnectedAfterSigninOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsConnectedToGameServer, IsConnectedToGameServerOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)GetBattleNetConnectionState, GetBattleNetConnectionStateOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)HasOnlineServiceSubscription, HasOnlineServiceSubscriptionOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsFullAccount, IsFullAccountOrg) == NO_ERROR &&*/
		//DetourAttach(&(PVOID&)IsOwned, IsOwnedOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)DoWeHaveContentPack, IsOwnedOrg) == NO_ERROR &&
	//	DetourAttach(&(PVOID&)IsGuest, IsGuestOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)GetActiveStatsSource, GetActiveStatsSourceOrg) == NO_ERROR &&
		/*DetourAttach(&(PVOID&)DoWeHaveOnlineSubscriptionInformation, DoWeHaveOnlineSubscriptionInformationOrg) == NO_ERROR &&*/
		//DetourAttach(&(PVOID&)OwnsBaseGame, OwnsBaseGameOrg) == NO_ERROR &&
		//DetourAttach(&(PVOID&)UserCanPlayOnline, UserCanPlayOnlineOrg) == NO_ERROR &&
		//DetourAttach(&(PVOID&)UserOwnsApp, UserOwnsAppOrg) == NO_ERROR &&
		/*DetourAttach(&(PVOID&)Social_GetOnlineStatusForBNet, Social_GetOnlineStatusForBNetOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)GetGameBattleAccountState, GetGameBattleAccountStateOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)DoWeHaveOnlineStats, DoWeHaveOnlineStatsOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)OnlineDataGetState, OnlineDataGetStateOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)OnlineServicesGetState, OnlineServicesGetStateOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsInSignInQueue, IsInSignInQueueOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsBattleNetAuthReady, IsBattleNetAuthReadyOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)GetSignInState, GetSignInStateOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)ShouldBeInOnlineArea, ShouldBeInOnlineAreaOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsBattleNetLanOnly, IsBattleNetLanOnlyOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsBattleNet, IsBattleNetOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)GetBattleNetFenceState, GetBattleNetFenceStateOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)OfflineDataFetched, OfflineDataFetchedOrg) == NO_ERROR &&
		DetourAttach(&(PVOID&)PlaylistsGetState, PlaylistsGetStateOrg) == NO_ERROR &&*/
		DetourAttach(&(PVOID&)fpGetLogonStatus, FpGetLogonStatus) == NO_ERROR &&
		DetourAttach(&(PVOID&)IsDemoBuild, IsDemoBuildOrg) == NO_ERROR &&
		//DetourAttach(&(PVOID&)HasMultiLoginErrror, IsDemoBuildOrg) == NO_ERROR &&
		//DetourAttach(&(PVOID&)ShouldFenceSignInShowErrorPopup, IsDemoBuildOrg) == NO_ERROR &&
		//DetourAttach(&(PVOID&)HAsErrors, IsDemoBuildOrg) == NO_ERROR &&
		// YOU NEED TO DETOUR THIS IN ORDER TO UNLOCK THE MAINMENU BUTTONS MP,CP,COOP
		DetourAttach(&(PVOID&)ISPremiumPlayer, IsBattleNetAuthReadyOrg) == NO_ERROR
		//DetourAttach(&(PVOID&)IsMapPackOwned, IsBattleNetAuthReadyOrg) == NO_ERROR
		//DetourAttach(&(PVOID&)Dvar_RegisterBool, Dvar_RegisterBool_Detour) == NO_ERROR &&
		//DetourAttach(&(PVOID&)IsAnyUserSignedIn, IsAnyUserSignedInOrg) == NO_ERROR 
		)
	{

		std::cout << "Attaching detour Successfully" << std::endl;

	}
	else {
		std::cout << "Error attaching detour." << std::endl;
		return 1;
	}

	// Commit the detour
	if (DetourTransactionCommit() == NO_ERROR) {
		std::cout << "Committing detour Successfully" << std::endl;
	}
	else {
		std::cout << "Error committing detour." << std::endl;
		return 1;

	}
	std::cout << "[Debug] PatchGame Part ..." << std::endl;

	while (1)
	{
		if (GetAsyncKeyState(VK_F1) & 1) {

			auto LUI_OpenMenu = reinterpret_cast<void(*)(int localClientNum, const char* menuName, int isPopup, int isModal, int isExclusive)>(baseAddress + 0x5622C10);
			LUI_OpenMenu(0, "MPMainMenu", true, false, false);
		}
		else if (GetAsyncKeyState(VK_F2) & 1) {

			LUI_CoD_LuaCall_ExecNow(*reinterpret_cast<uintptr_t*>(baseAddress + 0x196349B8), "xstartprivateparty");

		}
		else if (GetAsyncKeyState(VK_F3) & 1) {
			Cbuf_AddText("exec autoexec.cfg");
		}
		else if (GetAsyncKeyState(VK_F4) & 1) {
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&(PVOID&)IsUserSignedInToDemonware, IsUserSignedInToDemonwareOrg);
			DetourDetach(&(PVOID&)IsUserSignedIn, IsUserSignedInOrg);
			DetourDetach(&(PVOID&)IsUserSignedInToLive, IsUserSignedInToLiveOrg);
			DetourDetach(&(PVOID&)DoWeHaveContentPack, IsOwnedOrg);
			DetourDetach(&(PVOID&)GetActiveStatsSource, GetActiveStatsSourceOrg);
			DetourDetach(&(PVOID&)fpGetLogonStatus, FpGetLogonStatus);
			DetourDetach(&(PVOID&)IsDemoBuild, IsDemoBuildOrg);
			DetourDetach(&(PVOID&)ISPremiumPlayer, IsBattleNetAuthReadyOrg);
			DetourTransactionCommit();
			std::cout << "[Debug] Detoured Removed" << std::endl;
		}
	}



	return 0;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	AllocConsole();
	freopen("CONOUT$", "w+", stdout);

	pWarpperHinst = hinstDLL;
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		pOriginalHinst = LoadLibraryA("bink2w64_original.dll");
		if (!pOriginalHinst)
		{
			Error("Failed to load bink2w64_.dll!");
			return FALSE;
		}



		CreateThread(NULL, 0, InitzializeMemoryBaseAddress, NULL, 0, NULL);
		CreateThread(NULL, 0, HOOKS, NULL, 0, NULL);


		for (size_t i = 0; i < sImportFunctionNames.size(); i++)
			iImportFunctions[i] = reinterpret_cast<uintptr_t>(GetProcAddress(pOriginalHinst, sImportFunctionNames[i]));

		int result = ReplaceImport::Replace("api-ms-win-crt-runtime-l1-1-0.dll", "_initterm_e", (PROC)Init_Hook, &Init_Original);
		/*switch (result)
		{
		case 0: break;
		case 1: Error("Failed to get handle to main module!"); break;
		case 2: Error("Failed to find import table in executable!"); break;
		case 3: Error("Failed to change protection flags on memory page!"); break;
		case 4: Error("Failed to find API function in module!"); break;
		case 5: Error("Failed to find module!"); break;
		default: Error("Unknown error occurred!"); break;
		}*/

	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		FreeLibrary(pOriginalHinst);

		if (!loadedPlugins.empty())
		{
			for (auto plugin : loadedPlugins)
				FreeLibrary(plugin);
			loadedPlugins.clear();
		}
	}
	return TRUE;
}

extern "C" void BinkAllocateFrameBuffers();
extern "C" void BinkClose();
extern "C" void BinkCloseTrack();
extern "C" void BinkControlBackgroundIO();
extern "C" void BinkCopyToBuffer();
extern "C" void BinkCopyToBufferRect();
extern "C" void BinkDoFrame();
extern "C" void BinkDoFrameAsync();
extern "C" void BinkDoFrameAsyncMulti();
extern "C" void BinkDoFrameAsyncWait();
extern "C" void BinkDoFramePlane();
extern "C" void BinkFreeGlobals();
extern "C" void BinkGetError();
extern "C" void BinkGetFrameBuffersInfo();
extern "C" void BinkGetGPUDataBuffersInfo();
extern "C" void BinkGetKeyFrame();
extern "C" void BinkGetPlatformInfo();
extern "C" void BinkGetRealtime();
extern "C" void BinkGetRects();
extern "C" void BinkGetSummary();
extern "C" void BinkGetTrackData();
extern "C" void BinkGetTrackID();
extern "C" void BinkGetTrackMaxSize();
extern "C" void BinkGetTrackType();
extern "C" void BinkGoto();
extern "C" void BinkLogoAddress();
extern "C" void BinkNextFrame();
extern "C" void BinkOpen();
extern "C" void BinkOpenDirectSound();
extern "C" void BinkOpenMiles();
extern "C" void BinkOpenTrack();
extern "C" void BinkOpenWaveOut();
extern "C" void BinkOpenWithOptions();
extern "C" void BinkOpenXAudio2();
extern "C" void BinkOpenXAudio27();
extern "C" void BinkOpenXAudio28();
extern "C" void BinkPause();
extern "C" void BinkRegisterFrameBuffers();
extern "C" void BinkRegisterGPUDataBuffers();
extern "C" void BinkRequestStopAsyncThread();
extern "C" void BinkRequestStopAsyncThreadsMulti();
extern "C" void BinkService();
extern "C" void BinkSetError();
extern "C" void BinkSetFileOffset();
extern "C" void BinkSetFrameRate();
extern "C" void BinkSetIO();
extern "C" void BinkSetIOSize();
extern "C" void BinkSetMemory();
extern "C" void BinkSetOSFileCallbacks();
extern "C" void BinkSetPan();
extern "C" void BinkSetSimulate();
extern "C" void BinkSetSoundOnOff();
extern "C" void BinkSetSoundSystem();
extern "C" void BinkSetSoundSystem2();
extern "C" void BinkSetSoundTrack();
extern "C" void BinkSetSpeakerVolumes();
extern "C" void BinkSetVideoOnOff();
extern "C" void BinkSetVolume();
extern "C" void BinkSetWillLoop();
extern "C" void BinkShouldSkip();
extern "C" void BinkStartAsyncThread();
extern "C" void BinkUtilCPUs();
extern "C" void BinkUtilFree();
extern "C" void BinkUtilMalloc();
extern "C" void BinkUtilMutexCreate();
extern "C" void BinkUtilMutexDestroy();
extern "C" void BinkUtilMutexLock();
extern "C" void BinkUtilMutexLockTimeOut();
extern "C" void BinkUtilMutexUnlock();
extern "C" void BinkWait();
extern "C" void BinkWaitStopAsyncThread();
extern "C" void BinkWaitStopAsyncThreadsMulti();
extern "C" void RADTimerRead();
