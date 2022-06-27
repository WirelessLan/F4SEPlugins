ScriptName PNIOPA_MainQuest extends Quest

Actor Property PlayerRef Auto Const
Message Property PNIOPA_MainMenu Auto Const
Message Property PNIOPA_SelectNodeMenu Auto Const
Message Property PNIOPA_SelectNodeMenu_UpperBody Auto Const
Message Property PNIOPA_SelectNodeMenu_LowerBody Auto Const
Message Property PNIOPA_SelectNodeMenu_Hand Auto Const

Function Initialize()
	RegisterForKey(PNIOPA_F4SE.GetMenuKey())
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
		PNIOPA_F4SE.ToggleFreeze(akTarget)
	ElseIf selection == 2
		PNIOPA_F4SE.ResetActor(akTarget)
	EndIf
EndFunction

Function ShowSelectNodeMenu()
	Int selection = PNIOPA_SelectNodeMenu.Show();
	If selection == 0
		ShowSelectUpperBodyMenu()
	ElseIf selection == 1
		ShowSelectLowerBodyMenu()
	ElseIf selection == 2
		ShowSelectHandMenu()
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
		PNIOPA_F4SE.SelectNode(akTarget, "Root")
	ElseIf selection == 1
		PNIOPA_F4SE.SelectNode(akTarget, "HEAD")
	ElseIf selection == 2
		PNIOPA_F4SE.SelectNode(akTarget, "Neck")
	ElseIf selection == 3
		PNIOPA_F4SE.SelectNode(akTarget, "SPINE1")
	ElseIf selection == 4
		PNIOPA_F4SE.SelectNode(akTarget, "SPINE2")
	ElseIf selection == 5
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Collarbone")
	ElseIf selection == 6
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Collarbone")
	ElseIf selection == 7
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_UpperArm")
	ElseIf selection == 8
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_UpperArm")
	ElseIf selection == 9
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_ForeArm1")
	ElseIf selection == 10
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_ForeArm1")
	ElseIf selection == 11
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Hand")
	ElseIf selection == 12
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Hand")
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
		PNIOPA_F4SE.SelectNode(akTarget, "Pelvis")
	ElseIf selection == 1
		PNIOPA_F4SE.SelectNode(akTarget, "LLeg_Thigh")
	ElseIf selection == 2
		PNIOPA_F4SE.SelectNode(akTarget, "RLeg_Thigh")
	ElseIf selection == 3
		PNIOPA_F4SE.SelectNode(akTarget, "LLeg_Calf")
	ElseIf selection == 4
		PNIOPA_F4SE.SelectNode(akTarget, "RLeg_Calf")
	ElseIf selection == 5
		PNIOPA_F4SE.SelectNode(akTarget, "LLeg_Foot")
	ElseIf selection == 6
		PNIOPA_F4SE.SelectNode(akTarget, "RLeg_Foot")
	EndIf
EndFunction

Function ShowSelectHandMenu()
	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	Int selection = PNIOPA_SelectNodeMenu_Hand.Show()
	If selection == 0
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger11")
	ElseIf selection == 1
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger11")
	ElseIf selection == 2
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger12")
	ElseIf selection == 3
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger12")
	ElseIf selection == 4
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger21")
	ElseIf selection == 5
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger21")
	ElseIf selection == 6
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger22")
	ElseIf selection == 7
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger22")
	ElseIf selection == 8
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger31")
	ElseIf selection == 9
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger31")
	ElseIf selection == 10
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger32")
	ElseIf selection == 11
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger32")
	ElseIf selection == 12
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger41")
	ElseIf selection == 13
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger41")
	ElseIf selection == 14
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger42")
	ElseIf selection == 15
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger42")
	ElseIf selection == 16
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger51")
	ElseIf selection == 17
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger51")
	ElseIf selection == 18
		PNIOPA_F4SE.SelectNode(akTarget, "LArm_Finger52")
	ElseIf selection == 19
		PNIOPA_F4SE.SelectNode(akTarget, "RArm_Finger52")
	EndIf
EndFunction