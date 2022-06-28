ScriptName PNIOPA_MainQuest extends Quest

Actor Property PlayerRef Auto Const
Message Property PNIOPA_MainMenu Auto Const
Message Property PNIOPA_SelectNodeMenu Auto Const
Message Property PNIOPA_SelectNodeMenu_UpperBody Auto Const
Message Property PNIOPA_SelectNodeMenu_LowerBody Auto Const
Message Property PNIOPA_SelectNodeMenu_Fingers Auto Const
Message Property PNIOPA_SelectNodeMenu_ETC Auto Const

InputEnableLayer mLayer = None

Function Initialize()
	RegisterForKey(PNIOPA_F4SE.GetMenuKey())

	If PNIOPA_F4SE.IsActorFrozen(PlayerRef)
		PNIOPA_F4SE.ToggleFreeze(PlayerRef)
	EndIf

	If mLayer != None
		mLayer.Delete()
		mLayer = None
	EndIf
EndFunction

Event OnQuestInit()
    RegisterForRemoteEvent(PlayerRef, "OnPlayerLoadGame")
	Initialize()
EndEvent

Event Actor.OnPlayerLoadGame(Actor aSender)
    RegisterForRemoteEvent(PlayerRef, "OnPlayerLoadGame")
	Initialize()
EndEvent

Event OnKeyDown(Int KeyCode)
	If KeyCode == PNIOPA_F4SE.GetMenuKey()
		ShowMainMenu()
	EndIf
EndEvent

Function ShowMainMenu()
	if Utility.IsInMenuMode()
		Return
	EndIf
	  
	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	Int selection = PNIOPA_MainMenu.Show()
	If selection == 0
		ShowSelectNodeMenu()
	ElseIf selection == 1
		If akTarget == PlayerRef
			If !PNIOPA_F4SE.IsActorFrozen(akTarget)
				mLayer = InputEnableLayer.Create()
				mLayer.DisablePlayerControls(false, true, true, false, true, true, true, true, true, true, true)
			ElseIf mLayer != None
				mLayer.Delete()
				mLayer = None
			EndIf
		EndIf

		PNIOPA_F4SE.ToggleFreeze(akTarget)
	ElseIf selection == 2
		If akTarget == PlayerRef
			mLayer.Delete()
			mLayer = None
		EndIf

		PNIOPA_F4SE.ResetActor(akTarget)
	EndIf
EndFunction

Function ShowSelectNodeMenu()
	Int selection = PNIOPA_SelectNodeMenu.Show();
	If selection == 0
		ShowMainMenu()
	ElseIf selection == 1
		ShowSelectUpperBodyMenu()
	ElseIf selection == 2
		ShowSelectLowerBodyMenu()
	ElseIf selection == 3
		ShowSelectFingersMenu()
	ElseIf selection == 4
		ShowSelectETCMenu()
	EndIf
EndFunction

Function ShowSelectUpperBodyMenu()
	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	Int selection = PNIOPA_SelectNodeMenu_UpperBody.Show()
	If selection == 0
		ShowSelectNodeMenu()
	Else
		PNIOPA_F4SE.SelectNode(akTarget, "Upper", selection)
	EndIf
EndFunction

Function ShowSelectLowerBodyMenu()
	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	Int selection = PNIOPA_SelectNodeMenu_LowerBody.Show()
	If selection == 0
		ShowSelectNodeMenu()
	Else
		PNIOPA_F4SE.SelectNode(akTarget, "Lower", selection)
	EndIf
EndFunction

Function ShowSelectFingersMenu()
	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	Int selection = PNIOPA_SelectNodeMenu_Fingers.Show()
	If selection == 0
		ShowSelectNodeMenu()
	Else
		PNIOPA_F4SE.SelectNode(akTarget, "Fingers", selection)
	EndIf
EndFunction

Function ShowSelectETCMenu()
	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	Int selection = PNIOPA_SelectNodeMenu_ETC.Show()
	If selection == 0
		ShowSelectNodeMenu()
	Else
		PNIOPA_F4SE.SelectNode(akTarget, "ETC", selection)
	EndIf
EndFunction