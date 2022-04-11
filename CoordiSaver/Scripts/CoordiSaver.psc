Scriptname CoordiSaver Native Hidden

Form[] Function GetCoordiFromFile(string slot) global native
bool Function SaveCoordiToFile(string slot) global native

Function UnequipAllExceptPipboy(Actor actor) global
    Int ii = 0
    While ii <= 28
        actor.UnequipItemSlot(ii)
        ii += 1
    EndWhile
EndFunction

Function GetCoordi(string slot) global
    Form[] armorList = GetCoordiFromFile(slot)

    If (armorList.Length == 0)
        Debug.MessageBox("슬롯 " + slot + "의 의상들을 불러올 수 없습니다.")
        Return
    EndIf

    Actor player = Game.GetPlayer()
    UnequipAllExceptPipboy(player)

    int ii = 0
    While ii < armorList.Length
        player.EquipItem(armorList[ii], false, true)
        ii += 1
    EndWhile

    Debug.MessageBox("슬롯 " + slot + "의 의상들을 불러왔습니다.")
EndFunction

Function SaveCoordi(string slot) global
    if SaveCoordiToFile(slot) != true
        Debug.MessageBox("슬롯 " + slot + "에 의상들을 저장할 수 없습니다.")
    Else
        Debug.MessageBox("슬롯 " + slot + "에 의상들을 저장했습니다.")
    EndIf
EndFunction