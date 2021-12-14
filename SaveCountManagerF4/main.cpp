// F4SE
#include "common/IDebugLog.h"  // IDebugLog
#include "f4se/PluginAPI.h"
#include "f4se/GameSettings.h"

// Common
#include "f4se_common/f4se_version.h"
#include <vector>
#include <algorithm>

#include <shlobj.h>	// CSIDL_MYCODUMENTS

#define PLUGIN_NAME	"SaveCountManagerF4"

PluginHandle			g_pluginHandle = kPluginHandle_Invalid;
F4SEMessagingInterface* g_messaging = NULL;

UInt32	g_uMaxSaveCnt = 20;

const std::string& GetRuntimeDirectory(void)
{
	static std::string s_runtimeDirectory;

	if (s_runtimeDirectory.empty())
	{
		// can't determine how many bytes we'll need, hope it's not more than MAX_PATH
		char	runtimePathBuf[MAX_PATH];
		UInt32	runtimePathLength = GetModuleFileName(GetModuleHandle(NULL), runtimePathBuf, sizeof(runtimePathBuf));

		if (runtimePathLength && (runtimePathLength < sizeof(runtimePathBuf)))
		{
			std::string	runtimePath(runtimePathBuf, runtimePathLength);

			// truncate at last slash
			std::string::size_type	lastSlash = runtimePath.rfind('\\');
			if (lastSlash != std::string::npos)	// if we don't find a slash something is VERY WRONG
			{
				s_runtimeDirectory = runtimePath.substr(0, lastSlash + 1);

				_DMESSAGE("runtime root = %s", s_runtimeDirectory.c_str());
			}
			else
			{
				_WARNING("no slash in runtime path? (%s)", runtimePath.c_str());
			}
		}
		else
		{
			_WARNING("couldn't find runtime path (len = %d, err = %08X)", runtimePathLength, GetLastError());
		}
	}

	return s_runtimeDirectory;
}

const std::string& GetConfigPath(void)
{
	static std::string s_configPath;

	if (s_configPath.empty())
	{
		std::string	runtimePath = GetRuntimeDirectory();
		if (!runtimePath.empty())
		{
			std::string subPath { "Data\\F4SE\\Plugins\\" PLUGIN_NAME ".ini" };
			s_configPath = runtimePath + subPath;
			_MESSAGE("config path = %s", s_configPath.c_str());
		}
	}

	return s_configPath;
}

std::string GetConfigOption(const char *section, const char* key)
{
	std::string	result;

	const std::string& configPath = GetConfigPath();
	if (!configPath.empty())
	{
		char	resultBuf[256] = { 0 };
		UInt32	resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), configPath.c_str());
		result = resultBuf;
	}

	return result;
}

const char* GetTypeFormatting(UInt32* dataOut) { return "%d"; }

bool GetConfigValue(const char* section, const char* key, UInt32* dataOut)
{
	std::string	data = GetConfigOption(section, key);
	if (data.empty())
		return false;

	UInt32 tmp;
	bool res = (sscanf_s(data.c_str(), GetTypeFormatting(dataOut), &tmp) == 1);
	if (res) {
		*dataOut = tmp;
	}
	return res;
}

class SaveFileInfo {
public:
	// Save Types
	enum {
		Normal,
		AutoSave,
		QuickSave,
		ExitSave
	};

	std::string saveName;
	UInt32	cIdx;
	char	buf[300];

	UInt16	type;
	UInt32	number;
	std::string	id;
	char	unk1;
	std::string playerName;
	std::string location;
	std::string unk2;
	std::string time;
	UInt32 unk3;
	UInt32 unk4;

	SaveFileInfo() {}
	SaveFileInfo(std::string saveName) {
		this->saveName = saveName;
		separateSaveName();
	}

	bool operator < (const SaveFileInfo& sfi) const
	{
		return (number < sfi.number);
	}

private:
	void separateSaveName() {
		this->cIdx = 0;
		getSaveType();
		getSaveNumber();
		getSaveId();
		getUnk1();
		getPlayerName();
		getLocation();
		getUnk2();
		getTime();
		getUnk3();
		getUnk4();
	}

	void getNext() {
		UInt32 ii;
		for (ii = 0; this->cIdx < this->saveName.length(); ii++) {
			if (this->saveName[this->cIdx] == '_') {
				this->cIdx++;
				break;
			}
			buf[ii] = getNextChar();
		}
		buf[ii] = 0;
	}

	std::string getNextString() {
		getNext();
		return std::string(buf);
	}

	UInt32 getNextInt() {
		getNext();
		return atoi(buf);
	}

	char getNextChar() {
		return this->saveName[this->cIdx++];
	}

	void getSaveType() {
		char saveType[30] = { 0 };
		for (UInt32 ii = 0; this->cIdx < this->saveName.length(); ii++) {
			if (this->saveName[this->cIdx] >= '0' && this->saveName[this->cIdx] <= '9')
				break;
			saveType[ii] = getNextChar();
		}

		if (strcmp(saveType, "Save") == 0) this->type = Normal;
		else if (strcmp(saveType, "Autosave") == 0) this->type = AutoSave;
		else if (strcmp(saveType, "Quicksave") == 0) this->type = QuickSave;
		else if (strcmp(saveType, "Exitsave") == 0)	this->type = ExitSave;
		else
			_WARNING("Unknown Save Type[%s]", saveType);
	}

	void getSaveNumber() {
		this->number = getNextInt();
	}

	void getSaveId() {
		char saveId[10] = { 0 };
		for (UInt32 ii = 0; ii < 8; ii++)
			saveId[ii] = getNextChar();

		this->id = std::string(saveId);
	}

	void getUnk1() {
		this->unk1 = getNextChar();
	}

	void getPlayerName() {
		this->playerName = getNextString();
	}

	void getLocation() {
		this->location = getNextString();
	}

	void getUnk2() {
		this->unk2 = getNextString();
	}

	void getTime() {
		this->time = getNextString();
	}

	void getUnk3() {
		this->unk3 = getNextInt();
	}
	
	void getUnk4() {
		this->unk4 = getNextInt();
	}
};

class SaveCountManager {
public:
	SaveCountManager(const std::string &saveName) {
		this->sf = SaveFileInfo(saveName);
		_MESSAGE("type[%d] number[%d] id[%s] unk1[%c] playerName[%s] location[%s] unk2[%s] time[%s] unk3[%d] unk4[%d]", 
				this->sf.type, this->sf.number, this->sf.id.c_str(), this->sf.unk1, this->sf.playerName.c_str(), 
				this->sf.location.c_str(), this->sf.unk2.c_str(), this->sf.time.c_str(), this->sf.unk3, this->sf.unk4);
	}

	void DeleteOldSaveFiles() {
		WIN32_FIND_DATAA findData;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		if (this->sf.type != SaveFileInfo::Normal)
			return;

		std::string savePath = getSavePath();

		std::string targets = savePath + "\\Save*_" + this->sf.id + "*";
		hFind = FindFirstFileA(targets.c_str(), &findData);

		if (hFind == INVALID_HANDLE_VALUE)
			return;

		std::vector<SaveFileInfo> sv;

		std::string rawName = getRawName(findData.cFileName);
		SaveFileInfo find_sf = SaveFileInfo(rawName);
		if (this->sf.id == find_sf.id)
			sv.push_back(find_sf);

		while (FindNextFileA(hFind, &findData) != 0)
		{
			rawName = getRawName(findData.cFileName);
			find_sf = SaveFileInfo(rawName);
			if (this->sf.id == find_sf.id && rawName != sv[sv.size() - 1].saveName)
					sv.push_back(find_sf);
		}
		FindClose(hFind);

		if ((uint32_t)sv.size() < g_uMaxSaveCnt)
			return;

		std::sort(sv.begin(), sv.end());

		for (UInt32 ii = 0; ii < (uint32_t)sv.size() - g_uMaxSaveCnt; ii++) {
			if (!deleteSaveFile(sv[ii].saveName))
				break;
		}
	}

private:
	SaveFileInfo sf;

	std::string getSavePath() {
		char	path[MAX_PATH];
		ASSERT(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, path)));

		std::string saveDir = path;
		saveDir += "\\My Games\\Fallout4\\";

		Setting* localSavePath = GetINISetting("sLocalSavePath:General");
		if (localSavePath && (localSavePath->GetType() == Setting::kType_String))
			saveDir += localSavePath->data.s;
		else
			saveDir += "Saves\\";

		return saveDir;
	}

	std::string getRawName(const std::string& filePath) {
		size_t lastindex = filePath.find_last_of(".");
		if (lastindex == std::string::npos)
			return filePath;
		return getRawName(filePath.substr(0, lastindex));
	}

	bool deleteSaveFile(std::string& saveName) {
		std::string saveFileWild = std::string(saveName) + "*";
		std::string saveDir = getSavePath();
		std::string savePath = saveDir + saveFileWild;
		_MESSAGE("delete file: %s", savePath.c_str());

		WIN32_FIND_DATAA fd;
		HANDLE hFind = FindFirstFileA(savePath.c_str(), &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (DeleteFileA((saveDir + fd.cFileName).c_str()))
					_MESSAGE("%s deleted!", fd.cFileName);
				else {
					_MESSAGE("%s delete error!!(%d)", fd.cFileName, GetLastError());
					FindClose(hFind);
					return false;
				}
			} while (FindNextFileA(hFind, &fd));

			FindClose(hFind);
		}

		return true;
	}
};

void OnF4SEMessage(F4SEMessagingInterface::Message* msg) {
	switch (msg->type) {
	case F4SEMessagingInterface::kMessage_PostSaveGame:
		_MESSAGE("New savename: %s", msg->data);
		SaveCountManager scm((const char *)msg->data);
		scm.DeleteOldSaveFiles();
		break;
	}
}

/* Plugin Query */
extern "C" {
	bool F4SEPlugin_Query(const F4SEInterface* f4se, PluginInfo* info) {
		std::string logPath{ "\\My Games\\Fallout4\\F4SE\\" PLUGIN_NAME ".log" };
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, logPath.c_str());
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PLUGIN_NAME;
		info->version = MAKE_EXE_VERSION(1, 0, 1);

		if (f4se->runtimeVersion != RUNTIME_VERSION_1_10_163) {
			_MESSAGE("unsupported runtime version %d", f4se->runtimeVersion);
			return false;
		}

		g_pluginHandle = f4se->GetPluginHandle();

		// Get the messaging interface
		g_messaging = (F4SEMessagingInterface*)f4se->QueryInterface(kInterface_Messaging);
		if (!g_messaging) {
			_MESSAGE("couldn't get messaging interface");
			return false;
		}

		return true;
	}

	bool F4SEPlugin_Load(const F4SEInterface* f4se) {
		GetConfigValue("Settings", "uMaxSaveCnt", &g_uMaxSaveCnt);
		_MESSAGE("g_uMaxSaveCnt[%d]", g_uMaxSaveCnt);

		if (g_messaging)
			g_messaging->RegisterListener(g_pluginHandle, "F4SE", OnF4SEMessage);

		return true;
	}
};