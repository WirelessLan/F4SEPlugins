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
Spell Function GetHighlightSpell(Bool IsMovable) native global
Function ClearSelectedActorOffset() native global
Function ShowSelectedSceneOffset() native global
Function ShowPositionerMenuNative() native global

Function AddHighlight(Actor akTarget, Bool canMove) global
	If akTarget == None
		Return
	EndIf

	Spell movableHighlightSpell = GetHighlightSpell(True)
	Spell immovableHighlightSpell = GetHighlightSpell(False)
	If !movableHighlightSpell || !immovableHighlightSpell
		Return
	EndIf

	If akTarget.HasSpell(movableHighlightSpell) || akTarget.HasSpell(immovableHighlightSpell)
		Return
	EndIf

	If canMove
		akTarget.AddSpell(movableHighlightSpell, False)
	Else
		akTarget.AddSpell(immovableHighlightSpell, False)
	EndIf
EndFunction

Function RemoveHighlight(Actor akTarget) global
	If akTarget == None
		Return
	EndIf

	Spell movableHighlightSpell = GetHighlightSpell(True)
	Spell immovableHighlightSpell = GetHighlightSpell(False)
	If !movableHighlightSpell || !immovableHighlightSpell
		Return
	EndIf

	akTarget.RemoveSpell(movableHighlightSpell)
	akTarget.RemoveSpell(immovableHighlightSpell)
EndFunction

Function TogglePositioner() global
	If IsPositionerEnabled()
		Debug.Notification("위치 조절 모드를 종료합니다.")
		RemoveHighlight(GetSelectedActor())
		SetPositionerState(False);
	Else
		Debug.Notification("위치 조절 모드를 시작합니다.")
		SetPositionerState(True);
		ChangeActor()
	EndIf
EndFunction

Function MoveActor(String axis, Bool isInc) global
	Int canMove = CanMovePosition()

	; 1: 선택된 액터가 없음
	; 2: 스케일 1인 액터
	; 3: 위치조절모드 Off
	If canMove != 0
		If canMove == 1
			Debug.Notification("선택된 액터가 없습니다.")
		ElseIf canMove == 2
			Debug.Notification("스케일이 1인 액터는 이동할 수 없습니다.")
		ElseIf canMove == 3
			Debug.Notification("위치 조절 모드가 꺼져있습니다.")
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

	AddHighlight(selectedActor, CanMovePosition() == 0)
EndFunction

Function ShowPositionerMenu() global
	Int canMove = CanMovePosition()
	If canMove != 0
		If canMove == 1
			Debug.Notification("선택된 액터가 없습니다.")
		ElseIf canMove == 2
			Debug.Notification("스케일이 1인 액터는 이동할 수 없습니다.")
		ElseIf canMove == 3
			Debug.Notification("위치 조절 모드가 꺼져있습니다.")
		EndIf

		Return
	EndIf

	ShowPositionerMenuNative()
EndFunction