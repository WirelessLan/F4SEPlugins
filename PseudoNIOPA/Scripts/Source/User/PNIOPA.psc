ScriptName PNIOPA native hidden

Bool Function IsActorFrozen(Actor akTarget) global native
Function ShowMenuNative() global native
Function ClearNodeSelection() global native
Function ResetNode() global native

Function ShowMenu() global
	Actor akTarget = Game.GetCurrentConsoleRef() as Actor
	if !akTarget
		Debug.Notification("콘솔에서 액터를 선택해주세요!")
		Return
	EndIf

	ShowMenuNative();
EndFunction