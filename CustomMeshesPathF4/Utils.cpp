#include "Global.h"

void Trim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}

std::string ToLower(const std::string& str) {
	std::string retStr = str;
	std::transform(retStr.begin(), retStr.end(), retStr.begin(), [](unsigned char c) {
		return std::tolower(c);
	});
	return retStr;
}

std::string RemovePrefix(const std::string& prefixStr, const std::string& str) {
	size_t prefixPos = str.find(prefixStr);
	if (prefixPos == 0)
		return str.substr(prefixStr.length());
	return str;
}

void Log(const char* fmt, ...) {
	if (bDebug) {
		va_list args;
		va_start(args, fmt);
		gLog.Log(IDebugLog::kLevel_Message, fmt, args);
		va_end(args);
	}
}

std::string GetFileExt(const std::string& fname) {
	size_t idx = fname.rfind('.');
	if (idx == std::string::npos)
		return std::string();
	return fname.substr(idx + 1);
}

class ExtraLeveledCreature : public BSExtraData {
public:
	TESForm* baseForm;              // 18
};

TESForm* GetActorBaseForm(Actor* actor) {
	if (!actor || !actor->extraDataList)
		return nullptr;

	BSExtraData* extraData = actor->extraDataList->GetByType(ExtraDataType::kExtraData_LeveledCreature);
	if (!extraData)
		return nullptr;

	ExtraLeveledCreature* extraLeveldCreature = DYNAMIC_CAST(extraData, BSExtraData, ExtraLeveledCreature);
	if (!extraLeveldCreature)
		return nullptr;

	return extraLeveldCreature->baseForm;
}

namespace BSResource {
	struct ID {
		std::uint32_t file;	// 0
		std::uint32_t ext;	// 4
		std::uint32_t dir;	// 8
	};

	namespace SDirectory2 {
		struct Cursor {
		};
	}

	void GenerateID(ID& id, const char* path) {
		using func_t = void(*)(ID&, const char*);
		func_t func = RelocAddr<func_t>(0x1B6F0E0);
		func(id, path);
	}

	void GetReader(SDirectory2::Cursor** cursor) {
		using func_t = void(*)(SDirectory2::Cursor**);
		func_t func = RelocAddr<func_t>(0x1B71700);
		func(cursor);
	}

	void ReleaseReader(SDirectory2::Cursor* cursor) {
		using func_t = void(*)(SDirectory2::Cursor*);
		func_t func = RelocAddr<func_t>(0x1B715E0);
		func(cursor);
	}

	bool Exists(SDirectory2::Cursor* cursor, ID& id) {
		using func_t = bool(*)(SDirectory2::Cursor*, ID&);
		func_t func = RelocAddr<func_t>(0x1B72000);
		return func(cursor, id);
	}
}

bool IsFileExists(const std::string& path) {
	BSResource::ID file_id;
	BSResource::SDirectory2::Cursor* cursor;

	BSResource::GenerateID(file_id, path.c_str());
	BSResource::GetReader(&cursor);
	bool retVal = BSResource::Exists(cursor, file_id);
	BSResource::ReleaseReader(cursor);

	return retVal;
}

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::string& formIdStr) {
	std::uint32_t formID = std::stoul(formIdStr, nullptr, 16) & 0xFFFFFF;
	return GetFormFromIdentifier(pluginName, formID);
}

TESForm* GetFormFromIdentifier(const std::string& pluginName, const std::uint32_t formId) {
	if (!*g_dataHandler)
		return nullptr;

	const ModInfo* mod = (*g_dataHandler)->LookupModByName(pluginName.c_str());
	if (!mod || mod->modIndex == -1)
		return nullptr;

	std::uint32_t actualFormId = formId;
	std::uint32_t pluginIndex = mod->GetPartialIndex();
	if (!mod->IsLight())
		actualFormId |= pluginIndex << 24;
	else
		actualFormId |= pluginIndex << 12;

	return LookupFormByID(actualFormId);
}