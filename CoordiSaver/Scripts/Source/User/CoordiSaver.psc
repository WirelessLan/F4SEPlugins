Scriptname CoordiSaver Native Hidden

Form[] Function LoadSlotFromFile(string slot) global native
bool Function SaveSlotToFile(string slot) global native
Function ShowMenu() global native

Function UnequipAll(Actor akTarget) global
    Int ii = 0
    While ii <= 28
        akTarget.UnequipItemSlot(ii)
        ii += 1
    EndWhile
EndFunction

Function LoadSlot(string slot) global
    Form[] armorList = LoadSlotFromFile(slot)

    If (armorList.Length == 0)
        Debug.Notification("슬롯 " + slot + "의 의상들을 불러올 수 없습니다.")
        Return
    EndIf

	Actor akTarget = (Game.GetCurrentConsoleRef() as Actor)
	if !akTarget
        akTarget = Game.GetPlayer();
	EndIf

    UnequipAll(akTarget)

    int ii = 0
    While ii < armorList.Length
        akTarget.EquipItem(armorList[ii], false, true)
        ii += 1
    EndWhile

    Debug.Notification("슬롯 " + slot + "의 의상들을 불러왔습니다.")
EndFunction

Function SaveSlot(string slot) global
    if SaveSlotToFile(slot) != true
        Debug.Notification("슬롯 " + slot + "에 의상들을 저장할 수 없습니다.")
    Else
        Debug.Notification("슬롯 " + slot + "에 의상들을 저장했습니다.")
    EndIf
EndFunction