ScriptName AAFDynamicPositioner native Hidden

Bool Function IsPositionerEnabled() native global
Function SetPositionerState(Bool start) native global
Function SceneInit(Actor[] actors, Actor doppelganger) native global
Function AnimationChange(String position, Actor[] actors) native global
Function SceneEnd(Actor[] actors) native global
Function MoveAxis(String axis, Bool value) native global
Int Function CanMovePosition() native global
Actor Function ChangeSelectedActor() native global
Actor Function GetSelectedActor() native global
Spell Function GetHighlightSpell() native global
String Function IntToHex(Int value) native global

Function AddHighlight(Actor akTarget) global
	If akTarget == None
		Return
	EndIf

	Spell highlightSpell = GetHighlightSpell()
	If highlightSpell == None || akTarget.HasSpell(highlightSpell)
		Return
	EndIf

	akTarget.AddSpell(highlightSpell, false)
EndFunction

Function RemoveHighlight(Actor akTarget) global
	If akTarget == None
		Return
	EndIf

	Spell highlightSpell = GetHighlightSpell()
	If highlightSpell == None || !akTarget.HasSpell(highlightSpell)
		Return
	EndIf

	akTarget.RemoveSpell(highlightSpell)
EndFunction

Function TogglePositioner() global
	If IsPositionerEnabled()
		Debug.Notification("위치 조절 모드를 종료합니다.")
		RemoveHighlight(GetSelectedActor())
		SetPositionerState(false);
	Else
		Debug.Notification("위치 조절 모드를 시작합니다.")
		SetPositionerState(true);
		ChangeActor()
	EndIf
EndFunction

Function MoveActor(String axis, Bool isInc) global
	If !IsPositionerEnabled()
		Debug.Notification("위치 조절 모드가 꺼져있습니다.")
		Return
	EndIf

	Int canMove = CanMovePosition()

	; 1: 선택된 액터가 없음
	; 2: 스케일 1인 액터
	; 3: 플레이어에게만 적용
	If canMove != 0
		If canMove == 1
			Debug.Notification("선택된 액터가 없습니다.")
		ElseIf canMove == 2
			Debug.Notification("스케일이 1인 액터는 이동할 수 없습니다.")
		ElseIf canMove == 3
			Debug.Notification("플레이어의 씬에 포함된 액터가 아닙니다.")
		EndIf

		Return
	EndIf

	MoveAxis(axis, isInc)
EndFunction

Function ChangeActor() global 
	If !IsPositionerEnabled()
		Debug.Notification("위치 조절 모드가 꺼져있습니다.")
		Return
	EndIf

	RemoveHighlight(GetSelectedActor())

	Actor selectedActor = ChangeSelectedActor()
	If selectedActor == None
		Debug.Notification("선택 가능한 액터가 없습니다.")
		Return
	EndIf

	AddHighlight(selectedActor)
EndFunction