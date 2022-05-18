#include "Global.h"

RelocPtr<uintptr_t> ExtraRefrPath_VTable(0x02C539D0);

std::unordered_map<UInt32, ActorData> actorMap;
std::unordered_map<UInt64, SceneData> sceneMap;

UInt64 sceneMapKey = 1;
UInt32 selectedActorFormId = 0;
bool positionerEnabled = false;

enum CanMove {
	kCanMove_YES = 0,
	kCanMove_NO_SELECTION,
	kCanMove_NO_SCALE,
	kCanMove_NO_PLAYER,
	kCanMove_NO_DISABLED
};

namespace Positioner {
	ActorData* GetActorDataByFormId(UInt32 formId) {
		auto actorIt = actorMap.find(formId);
		if (actorIt == actorMap.end())
			return nullptr;

		return &actorIt->second;
	}

	ActorData* GetPlayerActorData() {
		return GetActorDataByFormId((*g_player)->formID);
	}

	ActorData* GetSelectedActorData() {
		if (!selectedActorFormId)
			return nullptr;

		return GetActorDataByFormId(selectedActorFormId);
	}

	SceneData* GetSceneDataById(UInt64 sceneId) {
		auto sceneIt = sceneMap.find(sceneId);
		if (sceneIt == sceneMap.end())
			return nullptr;

		return &sceneIt->second;
	}

	void SavePosition(ActorData* actorData) {
		if (!actorData)
			return;

		SceneData* sceneData = GetSceneDataById(actorData->sceneId);
		if (!sceneData)
			return;

		PositionData::SavePositionData(sceneData);
	}

	bool GetPositionData(PositionData::Data& o_data, const std::vector<PositionData::Data> posVec, UInt32 index) {
		for (PositionData::Data data: posVec) {
			if (data.index == index) {
				o_data = data;
				return true;
			}
		}

		return false;
	}

	void SetOffset(NiPoint3& offset, const std::vector<PositionData::Data> posVec, UInt32 posIdx) {
		offset = NiPoint3();

		PositionData::Data data;
		if (GetPositionData(data, posVec, posIdx))
			offset = data.offset;
	}

	NiPoint3 GetRotatedOffset(const NiPoint3& offset, float rot) {
		float rotatedOffsetX = offset.x * cos(rot) + offset.y * sin(rot);
		float rotatedOffsetY = -offset.x * sin(rot) + offset.y * cos(rot);
		float rotatedOffsetZ = offset.z;
		return NiPoint3(rotatedOffsetX, rotatedOffsetY, rotatedOffsetZ);
	}

	bool IsPlayerInScene() {
		ActorData* playerActorData = GetPlayerActorData();
		if (!playerActorData)
			return false;

		return true;
	}

	bool IsActorInPlayerScene(ActorData* actorData) {
		ActorData* playerActorData = GetPlayerActorData();
		if (!playerActorData)
			return false;

		if (actorData->sceneId != playerActorData->sceneId)
			return false;

		return true;
	}

	bool IsActorScale1(Actor* actor) {
		float actorScale = Utility::GetActualScale(actor);
		UInt32 intScale = static_cast<UInt32>(std::round(actorScale * 100));
		if (intScale == 100)
			return true;

		return false;
	}

	void ApplyOffset(ActorData* actorData) {
		// 플레이어 씬만 위치 조절이고 현재 액터가 플레이어 씬에 들어있지 않은 경우
		if (g_pluginSettings.bAdjustPlayerSceneOnly && !IsActorInPlayerScene(actorData))
			return;

		// 스케일 기반 위치 조절일 경우 스케일이 1인 액터는 위치 조절을 적용하지 않음
		if (g_pluginSettings.iPositionerType == PositionerType::kPositionerType_Relative && IsActorScale1(actorData->actor))
			return;

		NiPoint3 rotatedOffset = GetRotatedOffset(actorData->offset, actorData->actor->rot.z);

		if (actorData->extraRefPath) {
			if (g_pluginSettings.iPositionerType == PositionerType::kPositionerType_Relative) {
				float scale = Utility::GetActualScale(actorData->actor);
				float scaleDiff = 1.0f - scale;
				actorData->extraRefPath->x = actorData->originPos.x + rotatedOffset.x * scaleDiff;
				actorData->extraRefPath->y = actorData->originPos.y + rotatedOffset.y * scaleDiff;
				actorData->extraRefPath->z = actorData->originPos.z + rotatedOffset.z * scaleDiff;
			}
			else if (g_pluginSettings.iPositionerType == PositionerType::kPositionerType_Absolute) {
				actorData->extraRefPath->x = actorData->originPos.x + rotatedOffset.x;
				actorData->extraRefPath->y = actorData->originPos.y + rotatedOffset.y;
				actorData->extraRefPath->z = actorData->originPos.z + rotatedOffset.z;
			}
		}
	}

	bool IsValidExtraRefrPath(BSExtraData* extraData) {
		uintptr_t* vtablePtr = reinterpret_cast<uintptr_t*>(extraData);
		if (*vtablePtr == ExtraRefrPath_VTable.GetUIntPtr())
			return true;
		return false;
	}

	ExtraRefrPath* GetExtraRefrPath(Actor* actor) {
		BSExtraData* extraData = actor->extraDataList->m_data;
		while (extraData) {
			if (IsValidExtraRefrPath(extraData))
				return (ExtraRefrPath*)extraData;

			extraData = extraData->next;
		}

		return nullptr;
	}

	bool IsPositionerEnabled(StaticFunctionTag*) {
		return positionerEnabled;
	}

	void SetPositionerState(StaticFunctionTag*, bool start) {
		if (start) {
			positionerEnabled = true;
		}
		else {
			positionerEnabled = false;
			selectedActorFormId = 0;
		}
	}

	void SceneInit(StaticFunctionTag*, VMArray<Actor*> actors, Actor* doppelganger) {
		SceneData newScene;

		// 새 씬 초기화
		newScene.sceneId = sceneMapKey++;

		for (UInt32 ii = 0; ii < actors.Length(); ii++) {
			Actor* actorPtr = nullptr;
			actors.Get(&actorPtr, ii);

			bool isPlayerActor = false;

			// 현재 액터가 플레이어인 경우 플레이어의 도플갱어를 불러옴
			if (actorPtr == *g_player) {
				isPlayerActor = true;
				actorPtr = doppelganger;
				if (!actorPtr)
					continue;

				if (g_pluginSettings.bUnifyAAFDoppelgangerScale)
					Utility::SetScale(actorPtr, Utility::GetActualScale(*g_player));
			}

			// 액터 정보 초기화
			ActorData actorData;
			actorData.formId = isPlayerActor ? (*g_player)->formID : actorPtr->formID;
			actorData.actor = actorPtr;
			actorData.sceneId = newScene.sceneId;
			actorData.extraRefPath = nullptr;
			actorData.offset = NiPoint3();
			actorData.originPos = NiPoint3();

			// 초기화한 액터 정보를 씬의 액터 리스트에 삽입
			newScene.actorList.push_back(actorData.formId);

			// 씬의 액터 리스트에 삽입된 액터 정보의 포인터를 액터 맵에 추가함
			actorMap.insert(std::make_pair(actorData.formId, actorData));
		}

		// 씬을 씬 맵에 삽입
		sceneMap.insert(std::make_pair(newScene.sceneId, newScene));
	}

	void AnimationChange(StaticFunctionTag*, BSFixedString position, VMArray<Actor*> actors) {
		UInt64 sceneId = 0;

		std::vector<PositionData::Data> posDataVec = PositionData::LoadPositionData(std::string(position));
		for (UInt32 ii = 0; ii < actors.Length(); ii++) {
			Actor* actorPtr = nullptr;
			actors.Get(&actorPtr, ii);

			ActorData* actorData = GetActorDataByFormId(actorPtr->formID);
			if (!actorData)
				continue;

			// 액터를 이용하여 씬의 ID를 가져옴
			if (sceneId == 0)
				sceneId = actorData->sceneId;

			// 액터의 실제 위치를 저장하는 ExtraRefrPath를 불러옴
			ExtraRefrPath* extraRefPath = GetExtraRefrPath(actorData->actor);

			actorData->posIndex = ii;
			SetOffset(actorData->offset, posDataVec, ii);	// 오프셋 불러옴

			if (!actorData->extraRefPath && !extraRefPath)
				continue;

			// ExtraRefPath는 변하지 않은 경우
			if (actorData->extraRefPath && actorData->extraRefPath == extraRefPath) {
				// 좌표로 원복
				actorData->extraRefPath->x = actorData->originPos.x;
				actorData->extraRefPath->y = actorData->originPos.y;
				actorData->extraRefPath->z = actorData->originPos.z;
			}
			// ExtraRefPath가 변한 경우
			else {
				actorData->extraRefPath = extraRefPath;
				if (extraRefPath)
					actorData->originPos = NiPoint3(extraRefPath->x, extraRefPath->y, extraRefPath->z);
			}

			// 액터 오프셋 적용
			ApplyOffset(actorData);
		}

		// 씬 ID를 이용하여 씬 맵에서 씬을 찾는다
		SceneData* sceneData = GetSceneDataById(sceneId);
		if (!sceneData)
			return;

		sceneData->position = position;
	}

	void SceneEnd(StaticFunctionTag*, VMArray<Actor*> actors) {
		UInt64 sceneId = 0;

		for (UInt32 ii = 0; ii < actors.Length(); ii++) {
			Actor* actorPtr = nullptr;
			actors.Get(&actorPtr, ii);

			ActorData* actorData = GetActorDataByFormId(actorPtr->formID);
			if (!actorData)
				continue;

			// 액터를 이용하여 씬의 ID를 가져옴
			if (sceneId == 0)
				sceneId = actorData->sceneId;

			// 선택한 액터가 종료되는 씬에 포함되어있을 경우 선택한 액터를 초기화
			if (selectedActorFormId == actorData->formId)
				selectedActorFormId = 0;

			actorMap.erase(actorData->formId);
		}

		sceneMap.erase(sceneId);
	}

	void MoveAxis(StaticFunctionTag*, BSFixedString axis, bool isInc) {
		if (!positionerEnabled)
			return;

		ActorData* actorData = GetSelectedActorData();
		if (!actorData)
			return;

		if (!actorData->actor)
			return;

		ExtraRefrPath* extraRefPath = GetExtraRefrPath(actorData->actor);
		if (!extraRefPath)
			return;

		if (actorData->extraRefPath != extraRefPath) {
			actorData->extraRefPath = extraRefPath;
			actorData->originPos = NiPoint3(extraRefPath->x, extraRefPath->y, extraRefPath->z);
		}

		if (isInc) {
			if (axis == BSFixedString("X"))
				actorData->offset.x += g_pluginSettings.fMoveAxisSize;
			else if (axis == BSFixedString("Y"))
				actorData->offset.y += g_pluginSettings.fMoveAxisSize;
			else if (axis == BSFixedString("Z"))
				actorData->offset.z += g_pluginSettings.fMoveAxisSize;
		}
		else {
			if (axis == BSFixedString("X"))
				actorData->offset.x -= g_pluginSettings.fMoveAxisSize;
			else if (axis == BSFixedString("Y"))
				actorData->offset.y -= g_pluginSettings.fMoveAxisSize;
			else if (axis == BSFixedString("Z"))
				actorData->offset.z -= g_pluginSettings.fMoveAxisSize;
		}

		ApplyOffset(actorData);

		SavePosition(actorData);
	}

	UInt32 CanMovePosition(StaticFunctionTag*) {
		if (!positionerEnabled)
			return CanMove::kCanMove_NO_DISABLED;

		ActorData* selectedActorData = GetSelectedActorData();
		if (!selectedActorData)
			return CanMove::kCanMove_NO_SELECTION;

		// 스케일이 1이면 이동 불가
		if (IsActorScale1(selectedActorData->actor)) {
			if (g_pluginSettings.iPositionerType == PositionerType::kPositionerType_Relative)
				return CanMove::kCanMove_NO_SCALE;
		}

		if (g_pluginSettings.bAdjustPlayerSceneOnly && !IsPlayerInScene())
			return CanMove::kCanMove_NO_PLAYER;

		return CanMove::kCanMove_YES;
	}

	Actor* ChangeSelectedActor(StaticFunctionTag*) {
		if (!positionerEnabled)
			return nullptr;

		if (g_pluginSettings.bAdjustPlayerSceneOnly && !IsPlayerInScene())
			return nullptr;

		// 현재 선택되어있는 액터를 가져옴
		ActorData* actorData = GetSelectedActorData();
		// 현재 선택되어있는 액터가 없을 경우
		if (!actorData) {
			// 플레이어로 진행중인 씬이 있는지 확인
			actorData = GetPlayerActorData();
			if (actorData) {
				// 플레이어로 진행중인 씬이 있을 때 씬 정보를 가져옴
				SceneData* playerScene = GetSceneDataById(actorData->sceneId);
				if (!playerScene || playerScene->actorList.size() == 0)
					return nullptr;

				// 해당 씬의 가장 첫 액터를 선택하여 반환
				actorData = GetActorDataByFormId(playerScene->actorList.front());
				if (!actorData)
					return nullptr;

				selectedActorFormId = actorData->formId;
				return actorData->actor;
			}

			// 플레이어로 진행중인 씬이 없는 경우
			// 첫 씬의 첫 액터를 선택하여 반환함
			if (sceneMap.size() == 0)
				return nullptr;

			const SceneData& sceneData = sceneMap.begin()->second;
			actorData = GetActorDataByFormId(sceneData.actorList.front());
			if (!actorData)
				return nullptr;

			selectedActorFormId = actorData->formId;
			return actorData->actor;
		}

		// 현재 선택되어있는 액터가 있을 경우 현재 액터가 포함된 씬을 가져옴
		auto sceneIt = sceneMap.find(actorData->sceneId);
		if (sceneIt == sceneMap.end())
			return nullptr;

		const SceneData& sceneData = sceneIt->second;

		// 현재 선택된 액터를 찾고 그 액터의 다음 액터를 찾아서 반환
		auto actorIt = std::find(sceneData.actorList.begin(), sceneData.actorList.end(), actorData->formId);
		if (actorIt == sceneData.actorList.end())
			return nullptr;

		actorIt = std::next(actorIt);
		if (actorIt != sceneData.actorList.end()) {
			actorData = GetActorDataByFormId(*actorIt);
			if (!actorData)
				return nullptr;

			selectedActorFormId = actorData->formId;
			return actorData->actor;
		}

		// 선택된 액터가 해당 씬의 마지막 액터였을 경우
		// 해당 씬의 다음 씬의 첫 액터를 찾아서 반환
		sceneIt = std::next(sceneIt);
		if (sceneIt != sceneMap.end()) {
			if (sceneIt->second.actorList.size() == 0)
				return nullptr;

			actorData = GetActorDataByFormId(sceneIt->second.actorList.front());
			if (!actorData)
				return nullptr;

			selectedActorFormId = actorData->formId;
			return actorData->actor;
		}

		if (sceneMap.size() == 0)
			return nullptr;

		// 선택된 액터가 마지막 씬의 마지막 액터였을 경우
		// 가장 첫 씬의 첫 액터를 찾아서 반환
		actorData = GetActorDataByFormId(sceneMap.begin()->second.actorList.front());
		if (!actorData)
			return nullptr;

		selectedActorFormId = actorData->formId;
		return actorData->actor;
	}

	Actor* GetSelectedActor(StaticFunctionTag*) {
		ActorData* selectedActorData = GetSelectedActorData();
		if (!selectedActorData)
			return nullptr;

		return selectedActorData->actor;
	}

	SpellItem* GetHighlightSpell(StaticFunctionTag*, bool isMovable) {
		static TESForm* movableSpellForm = Utility::GetFormFromIdentifier("AAFDynamicPositioner.esp", 0x00000810);
		static TESForm* immovableSpellForm = Utility::GetFormFromIdentifier("AAFDynamicPositioner.esp", 0x00000811);
		if (!movableSpellForm || !immovableSpellForm)
			return nullptr;

		SpellItem* spell = DYNAMIC_CAST(isMovable ? movableSpellForm : immovableSpellForm, TESForm, SpellItem);
		return spell;
	}

	void ClearSelectedActorOffset(StaticFunctionTag*) {
		if (!positionerEnabled)
			return;

		ActorData* selectedActorData = GetSelectedActorData();
		if (!selectedActorData)
			return;

		selectedActorData->offset = NiPoint3();

		ApplyOffset(selectedActorData);
		SavePosition(selectedActorData);
	}

	void ShowSelectedSceneOffset(StaticFunctionTag*) {
		if (!positionerEnabled)
			return;

		ActorData* selectedActorData = GetSelectedActorData();
		if (!selectedActorData)
			return;

		SceneData* selectedSceneData = GetSceneDataById(selectedActorData->sceneId);
		if (!selectedSceneData)
			return;

		std::string msg = selectedSceneData->position + "\n";
		for (UInt32 ii = 0; ii < selectedSceneData->actorList.size(); ii++) {
			for (UInt32& actorId : selectedSceneData->actorList) {
				ActorData* pActorData = GetActorDataByFormId(actorId);
				if (!pActorData)
					continue;

				if (ii == pActorData->posIndex) {
					if (pActorData == selectedActorData)
						msg += "[*]";
					else
						msg += "[" + std::to_string(ii) + "]";
					msg += std::to_string(pActorData->offset.x) + ", " + std::to_string(pActorData->offset.y) + ", " + std::to_string(pActorData->offset.z) + "\n";
				}
			}
		}
		Utility::ShowMessagebox(msg);
	}

	void ResetPositioner() {
		actorMap.clear();
		sceneMap.clear();
		sceneMapKey = 1;
		selectedActorFormId = 0;
		positionerEnabled = false;
	}

	void RegisterPositionerFunctions(VirtualMachine* vm) {
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, bool>("IsPositionerEnabled", "AAFDynamicPositioner", IsPositionerEnabled, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, bool>("SetPositionerState", "AAFDynamicPositioner", SetPositionerState, vm));
		vm->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, VMArray<Actor*>, Actor*>("SceneInit", "AAFDynamicPositioner", SceneInit, vm));
		vm->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, BSFixedString, VMArray<Actor*>>("AnimationChange", "AAFDynamicPositioner", AnimationChange, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, void, VMArray<Actor*>>("SceneEnd", "AAFDynamicPositioner", SceneEnd, vm));
		vm->RegisterFunction(
			new NativeFunction2<StaticFunctionTag, void, BSFixedString, bool>("MoveAxis", "AAFDynamicPositioner", MoveAxis, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, UInt32>("CanMovePosition", "AAFDynamicPositioner", CanMovePosition, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, Actor*>("ChangeSelectedActor", "AAFDynamicPositioner", ChangeSelectedActor, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, Actor*>("GetSelectedActor", "AAFDynamicPositioner", GetSelectedActor, vm));
		vm->RegisterFunction(
			new NativeFunction1<StaticFunctionTag, SpellItem*, bool>("GetHighlightSpell", "AAFDynamicPositioner", GetHighlightSpell, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, void>("ClearSelectedActorOffset", "AAFDynamicPositioner", ClearSelectedActorOffset, vm));
		vm->RegisterFunction(
			new NativeFunction0<StaticFunctionTag, void>("ShowSelectedSceneOffset", "AAFDynamicPositioner", ShowSelectedSceneOffset, vm));
	}
}