ScriptName PNIOPA_MainQuest extends Quest

Actor Property PlayerRef Auto Const
Message Property PNIOPA_MainMenu Auto Const
Message Property PNIOPA_SelectNodeMenu Auto Const
Message Property PNIOPA_SelectNodeMenu_SubMenu Auto Const

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

	While True
		Int selection = PNIOPA_MainMenu.Show()
		If selection == 0
			selection = ShowSelectNodeMenu(akTarget)
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

		If selection != 0
			Return
		EndIf
	EndWhile
EndFunction

Int Function ShowSelectNodeMenu(Actor akTarget)
	While True
		Int selection = PNIOPA_SelectNodeMenu.Show();
		If selection == 0
			Return selection
		ElseIf selection == 1
			selection = ShowSelectSubMenu(akTarget, "Upper")
		ElseIf selection == 2
			selection = ShowSelectSubMenu(akTarget, "Lower")
		ElseIf selection == 3
			selection = ShowSelectSubMenu(akTarget, "Fingers")
		ElseIf selection == 4
			selection = ShowSelectSubMenu(akTarget, "ETC")
		Else
			selection = -1
		EndIf

		If selection != 0
			Return selection
		EndIf
	EndWhile
EndFunction

Int Function ShowSelectSubMenu(Actor akTarget, String groupName)
	PNIOPA_F4SE.ClearButtons(PNIOPA_SelectNodeMenu_SubMenu)
	String[] nodes = PNIOPA_F4SE.LoadNodes(groupName)

	PNIOPA_F4SE.AddButton(PNIOPA_SelectNodeMenu_SubMenu, "[뒤로]")
	Int ii = 0
	While ii < nodes.Length
		PNIOPA_F4SE.AddButton(PNIOPA_SelectNodeMenu_SubMenu, nodes[ii])
		ii += 1
	EndWhile
	PNIOPA_F4SE.AddButton(PNIOPA_SelectNodeMenu_SubMenu, "[닫기]")

	Int selection = PNIOPA_SelectNodeMenu_SubMenu.Show()
	If selection == 0
		return selection
	ElseIf selection == nodes.Length + 1
		return -1
	EndIf

	PNIOPA_F4SE.SelectNode(akTarget, nodes[selection - 1])
	return selection
EndFunction