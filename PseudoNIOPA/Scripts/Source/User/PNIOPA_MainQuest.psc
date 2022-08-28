ScriptName PNIOPA_MainQuest extends Quest

Actor Property PlayerRef Auto Const

InputEnableLayer mLayer = None

Function Initialize()
	RegisterForExternalEvent("PNIOPA_Notification", "Notification")
	RegisterForExternalEvent("PNIOPA_ToggleFreeze", "ToggleFreeze")
	RegisterForExternalEvent("PNIOPA_ResetActor", "ResetActor")

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

Function Notification(String msg)
	Debug.Notification(msg)
EndFunction

Function ToggleFreeze()
	Actor akTarget = Game.GetCurrentConsoleRef() as Actor
	if !akTarget
		Return
	EndIf

	if akTarget != PlayerRef
		Return
	EndIf

	if PNIOPA_F4SE.IsActorFrozen(akTarget)
		mLayer = InputEnableLayer.Create()
		mLayer.DisablePlayerControls(false, false, true, false, true, true, true, true, true, true, true)
	Else
		mLayer.Delete()
		mLayer = None
	EndIf
EndFunction

Function ResetActor()
	Actor akTarget = Game.GetCurrentConsoleRef() as Actor
	if !akTarget
		Return
	EndIf

	if akTarget != PlayerRef
		Return
	EndIf

	If mLayer != None
		mLayer.Delete()
		mLayer = None
	EndIf
EndFunction

Function ShowMenu() global
	Actor akTarget = Game.GetCurrentConsoleRef() as Actor
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	PNIOPA_F4SE.ShowMenu();
EndFunction