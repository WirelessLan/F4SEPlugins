#include "Utils.h"

#include <algorithm>

#include <f4se/GameRTTI.h>
#include <f4se/GameData.h>
#include <f4se/GameReferences.h>
#include <f4se/GameExtraData.h>

extern bool bDebug;

namespace Utils {
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

	std::string ReplaceSlash(const std::string& input) {
		std::string result = input;
		std::replace(result.begin(), result.end(), '/', '\\');
		return result;
	}

	bool StartsWith(const std::string& input, const std::string& prefix) {
		if (input.length() < prefix.length())
			return false;
		return input.compare(0, prefix.length(), prefix) == 0;
	}

	std::string RemovePrefix(const std::string& input, const std::string& prefix) {
		if (StartsWith(input, prefix))
			return input.substr(prefix.length());
		return input;
	}

	void Log(const char* fmt, ...) {
		if (bDebug) {
			va_list args;
			va_start(args, fmt);
			gLog.Log(IDebugLog::kLevel_Message, fmt, args);
			va_end(args);
		}
	}

	namespace BSResource {
		struct ID {
			std::uint32_t file;	// 0
			std::uint32_t ext;	// 4
			std::uint32_t dir;	// 8

			static void GenerateID(ID& id, const char* path) {
				using func_t = void(*)(ID&, const char*);
				func_t func = RelocAddr<func_t>(0x1B6F0E0);
				func(id, path);
			}
		};

		namespace SDirectory2 {
			struct Cursor {
				std::uint64_t	unk00[0x178 >> 3];
			};

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

		class EntryDB {
		public:
			std::uint64_t	unk00[0x180 >> 3];	// 000
			std::uint64_t	unk180;				// 180
		};

		RelocPtr<EntryDB*> g_entryDB(0x58CD0A0);

		ID* AcquireIfExist(ID& id) {
			if (!*g_entryDB)
				return nullptr;

			using func_t = ID * (*)(std::uint64_t&, ID&);
			func_t func = RelocAddr<func_t>(0x196000);
			return func((*g_entryDB)->unk180, id);
		}
	}

	bool IsFileExists(const std::string& path) {
		BSResource::ID file_id;
		BSResource::SDirectory2::Cursor* cursor;

		BSResource::ID::GenerateID(file_id, path.c_str());

		BSResource::ID* exist_id = BSResource::AcquireIfExist(file_id);
		if (exist_id)
			return true;

		BSResource::SDirectory2::GetReader(&cursor);
		bool retVal = BSResource::SDirectory2::Exists(cursor, file_id);
		BSResource::SDirectory2::ReleaseReader(cursor);

		return retVal;
	}

	std::string GetFileExt(const std::string& fname) {
		size_t idx = fname.rfind('.');
		if (idx == std::string::npos || idx == fname.length() - 1)
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

	std::uint32_t GetBaseFormID(Actor* actor) {
		if (!actor)
			return 0;

		if (!actor->baseForm || (actor->baseForm->formID >> 24) == 0xFF) {
			TESForm* baseForm = GetActorBaseForm(actor);
			if (!baseForm)
				return 0;

			return baseForm->formID;
		}

		return actor->baseForm->formID;
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
}