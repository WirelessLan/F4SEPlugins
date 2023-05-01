#include "FurniturePositions.h"

#include <vector>
#include <fstream>
#include <cmath>

#include <f4se/GameRTTI.h>
#include <f4se/GameForms.h>
#include <f4se/GameObjects.h>
#include <f4se/GameReferences.h>
#include <f4se/xbyak/xbyak.h>
#include <f4se_common/BranchTrampoline.h>
#include <f4se_common/SafeWrite.h>

#include "Global.h"
#include "Configs.h"
#include "Utils.h"

namespace FurniturePositions {
	struct FurnitureKeywordOffset {
		BGSKeyword* keyword;
		float horizontal;
		float vertical;
	};

	std::vector<FurnitureKeywordOffset> furnitureOffsets;

	FurnitureKeywordOffset* GetFurnitureKeywordOffset(TESObjectREFR* a_furn) {
		for (std::uint32_t ii = 0; ii < furnitureOffsets.size(); ii++) {
			if (Utils::HasKeyword(a_furn->baseForm, furnitureOffsets[ii].keyword))
				return &furnitureOffsets[ii];
		}

		return nullptr;
	}

	float GetCurrentFurnitureMarkerRotation(TESObjectREFR* a_actorRef, TESObjectREFR* a_furnRef) {
		Actor* actor = DYNAMIC_CAST(a_actorRef, TESObjectREFR, Actor);
		TESFurniture* furnForm = DYNAMIC_CAST(a_furnRef->baseForm, TESForm, TESFurniture);
		if (!actor || !furnForm)
			return 0.0f;

		if (!actor->middleProcess || !actor->middleProcess->unk08)
			return 0.0f;

		std::uint32_t currentMarkerId = reinterpret_cast<std::uint32_t&>(actor->middleProcess->unk08->unk3B0[25]);

		if (furnForm->arr188.count == 0 || furnForm->arr188.count < currentMarkerId)
			return 0.0f;

		return reinterpret_cast<float&>(furnForm->arr188.entries[currentMarkerId].unk00[3]);
	}

	NiPoint3* GetModifiedFurniturePosition(TESObjectREFR* a_actorRef, TESObjectREFR* a_furnRef) {
		static NiPoint3 furniturePos = {};

		if (!a_actorRef || !a_furnRef)
			return nullptr;

		float scale = Utils::GetActualScale(a_actorRef);
		std::uint16_t castedScale = static_cast<std::int16_t>(std::round(scale * 100));
		if (castedScale == 0 || castedScale == 100)
			return nullptr;

		FurnitureKeywordOffset* offset = GetFurnitureKeywordOffset(a_furnRef);
		if (!offset)
			return nullptr;

		float horOffset = offset->horizontal * (1.0f - scale);
		float verOffset = offset->vertical * (1.0f - scale);

		float furnitureRot = a_furnRef->rot.z + GetCurrentFurnitureMarkerRotation(a_actorRef, a_furnRef);

		furniturePos = a_furnRef->pos;
		furniturePos.x += horOffset * std::sin(furnitureRot);
		furniturePos.y += horOffset * std::cos(furnitureRot);
		furniturePos.z += verOffset;

		return &furniturePos;
	}

	void Hooks_PlayFurnitureAnimation() {
		struct asm_code : Xbyak::CodeGenerator {
			asm_code(void* buf, std::uintptr_t a_funcAddr, std::uintptr_t a_targetAddr) : Xbyak::CodeGenerator(4096, buf) {
				Xbyak::Label funcLabel;
				Xbyak::Label retnLabel;

				push(rcx);
				push(rdx);
				sub(rsp, 0x10); movdqu(ptr[rsp], xmm0);
				sub(rsp, 0x10); movdqu(ptr[rsp], xmm1);
				sub(rsp, 0x10); movdqu(ptr[rsp], xmm2);
				sub(rsp, 0x20);

				mov(rcx, rbx);
				mov(rdx, r14);
				call(ptr[rip + funcLabel]);

				add(rsp, 0x20);
				movdqu(xmm2, ptr[rsp]); add(rsp, 0x10);
				movdqu(xmm1, ptr[rsp]); add(rsp, 0x10);
				movdqu(xmm0, ptr[rsp]); add(rsp, 0x10);
				pop(rdx);
				pop(rcx);

				test(rax, rax);
				je("RETURN");

				movss(xmm0, ptr[rax]);
				movss(ptr[rbp - 0x70], xmm0);
				movss(xmm0, ptr[rax + 0x04]);
				movss(ptr[rbp - 0x6C], xmm0);
				movss(xmm0, ptr[rax + 0x08]);
				movss(ptr[rbp - 0x68], xmm0);
				jmp(ptr[rip + retnLabel]);

				L("RETURN");
				movss(ptr[rbp - 0x68], xmm0);
				jmp(ptr[rip + retnLabel]);

				L(funcLabel);
				dq(a_funcAddr);

				L(retnLabel);
				dq(a_targetAddr + 0x05);
			}
		};

		RelocAddr<std::uintptr_t> target(0xE85836);

		void* codeBuf = g_localTrampoline.StartAlloc();
		asm_code code(codeBuf, (std::uintptr_t)GetModifiedFurniturePosition, target.GetUIntPtr());
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(target, (std::uintptr_t)code.getCode());
	}

	void Hooks_InitializeChairBedQuickPosition() {
		struct asm_code : Xbyak::CodeGenerator {
			asm_code(void* buf, std::uintptr_t a_funcAddr, std::uintptr_t a_targetAddr) : Xbyak::CodeGenerator(4096, buf) {
				Xbyak::Label funcLabel;
				Xbyak::Label retnLabel;

				push(rcx);
				push(rdx);
				sub(rsp, 0x20);

				mov(rcx, rsi);
				mov(rdx, rbx);
				call(ptr[rip + funcLabel]);

				add(rsp, 0x20);
				pop(rdx);
				pop(rcx);

				test(rax, rax);
				je("RETURN");

				movss(xmm0, ptr[rax]);
				movss(ptr[rbp - 0x50 + 0x30], xmm0);
				movss(xmm0, ptr[rax + 0x04]);
				movss(ptr[rbp - 0x50 + 0x34], xmm0);
				movss(xmm0, ptr[rax + 0x08]);
				movss(ptr[rbp - 0x50 + 0x38], xmm0);

				L("RETURN");
				lea(rdx, ptr[rbp + 0xD8]);
				jmp(ptr[rip + retnLabel]);

				L(funcLabel);
				dq(a_funcAddr);

				L(retnLabel);
				dq(a_targetAddr + 0x07);
			}
		};

		RelocAddr<std::uintptr_t> target(0xD79950);

		void* codeBuf = g_localTrampoline.StartAlloc();
		asm_code code(codeBuf, (std::uintptr_t)GetModifiedFurniturePosition, target.GetUIntPtr());
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(target, (std::uintptr_t)code.getCode());
	}

	const std::string furnConfigPath("Data\\F4SE\\Plugins\\"  PLUGIN_NAME  "_Furnitures.cfg");

	bool ShouldLoadFurnitureConfig() {
		struct _stat64 stat;
		if (_stat64(furnConfigPath.c_str(), &stat) != 0)
			return false;

		static time_t furnConfigLoadedTime = 0;
		if (furnConfigLoadedTime == 0 || furnConfigLoadedTime != stat.st_mtime) {
			furnConfigLoadedTime = stat.st_mtime;
			return true;
		}

		return false;
	}

	class FurnitureConfigReader : Configs::ConfigReader {
	public:
		FurnitureConfigReader(std::string path) : ConfigReader(path) {}

		std::vector<FurnitureKeywordOffset> ReadConfig() {
			std::vector<FurnitureKeywordOffset> retVec;

			std::fstream configFile(_configPath);
			if (!configFile.is_open()) {
				_MESSAGE("[Warning] Cannot read the furniture config file - %s", _configPath.c_str());
				return retVec;
			}

			std::string pluginName, formId, horOffset, verOffset;

			while (getLine(configFile)) {
				if (emptyOrCommentLine())
					continue;

				pluginName = getNextData('|');
				if (pluginName.empty()) {
					_MESSAGE("[Warning] Cannot read the Plugin Name - %s", _line.c_str());
					continue;
				}

				formId = getNextData('|');
				if (formId.empty()) {
					_MESSAGE("[Warning] Cannot read the Form ID - %s", _line.c_str());
					continue;
				}

				horOffset = getNextData(',');
				if (horOffset.empty()) {
					_MESSAGE("[Warning] Cannot read the Horizontal Offset - %s", _line.c_str());
					continue;
				}

				verOffset = getNextData(0);
				if (verOffset.empty()) {
					_MESSAGE("[Warning] Cannot read the Vertical Offset - %s", _line.c_str());
					continue;
				}

				TESForm* keyword = Utils::GetFormFromIdentifier(pluginName, formId);
				if (!keyword) {
					_MESSAGE("[Warning] Cannot find the Keyword - %s", _line.c_str());
					continue;
				}

				retVec.push_back({ (BGSKeyword*)keyword, stof(horOffset), stof(verOffset) });
			}

			return retVec;
		}
	};

	void LoadFurnitureConfig() {
		_MESSAGE("[Info] Load Furniture Config...");

		FurnitureConfigReader reader(furnConfigPath);
		furnitureOffsets = reader.ReadConfig();
	}
}