Scriptname CoordiSaver Native Hidden

Form[] Function LoadCoordiFromFile(string slot) global native
bool Function SaveCoordiToFile(string slot) global native
int Function HexToInt(string hex) global native

Function UnequipAllExceptPipboy(Actor actor) global
    Int ii = 0
    While ii <= 28
        actor.UnequipItemSlot(ii)
        ii += 1
    EndWhile
EndFunction

Function LoadCoordi(string slot) global
    EquipSlot("14", slot)
EndFunction

Function SaveCoordi(string slot) global
    if SaveCoordiToFile(slot) != true
        Debug.MessageBox("슬롯 " + slot + "에 의상들을 저장할 수 없습니다.")
    Else
        Debug.MessageBox("슬롯 " + slot + "에 의상들을 저장했습니다.")
    EndIf
EndFunction

Function EquipSlot(string refId, string slot) global
    Form[] armorList = LoadCoordiFromFile(slot)

    If (armorList.Length == 0)
        Debug.MessageBox("슬롯 " + slot + "의 의상들을 불러올 수 없습니다.")
        Return
    EndIf

    int intId = HexToInt(refId)
    Actor actor = Game.GetForm(intId) as Actor
    if actor == None
        Debug.MessageBox("Cannot Load Actor " + intId + " " + refId + "!")
        Return
    EndIf

    UnequipAllExceptPipboy(actor)

    int ii = 0
    While ii < armorList.Length
        actor.EquipItem(armorList[ii], false, true)
        ii += 1
    EndWhile

    Debug.MessageBox("슬롯 " + slot + "의 의상들을 불러왔습니다.")
EndFunction