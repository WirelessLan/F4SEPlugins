ScriptName AAFDynamicPositioner_Quest extends Quest

AAF:AAF_API AAF_API
Actor Property PlayerRef Auto Const
Float Property WaitDelay Auto

Function Initialize()
	AAF_API = Game.GetFormFromFile(0x00000F99, "AAF.esm") as AAF:AAF_API
	If !AAF_API
		Debug.MessageBox("Can't find AAF API.")
        Return
    Endif

    RegisterForCustomEvent(AAF_API, "OnSceneInit")
	RegisterForCustomEvent(AAF_API, "OnAnimationStart")
	RegisterForCustomEvent(AAF_API, "OnAnimationChange")
	RegisterForCustomEvent(AAF_API, "OnSceneEnd")
EndFunction

Event OnQuestInit()
    RegisterForRemoteEvent(PlayerRef, "OnPlayerLoadGame")
	Initialize()
EndEvent

Event Actor.OnPlayerLoadGame(actor aSender)
    RegisterForRemoteEvent(PlayerRef, "OnPlayerLoadGame")
    Initialize()
EndEvent

Event AAF:AAF_API.OnSceneInit(AAF:AAF_API akSender, Var[] akArgs)
    Int status = akArgs[0] as Int
    if status == 0
        Actor[] actors = Utility.VarToVarArray(akArgs[1]) as Actor[]
        Actor doppelganger = akArgs[2] as Actor

        AAFDynamicPositioner.SceneInit(actors, doppelganger)
    EndIf
EndEvent

Event AAF:AAF_API.OnAnimationStart(AAF:AAF_API akSender, Var[] akArgs)
    Int status = akArgs[0] as Int
    if status == 0
        Actor[] actors = Utility.VarToVarArray(akArgs[1]) as Actor[]
        String position = akArgs[2] as String

        Utility.Wait(WaitDelay)
        AAFDynamicPositioner.AnimationChange(position, actors)
    EndIf
EndEvent

Event AAF:AAF_API.OnAnimationChange(AAF:AAF_API akSender, Var[] akArgs)
    Int status = akArgs[0] as Int

    if status == 0
        Actor[] actors = Utility.VarToVarArray(akArgs[1]) as Actor[]
        String position = akArgs[2] as String

        Utility.Wait(WaitDelay)
        AAFDynamicPositioner.AnimationChange(position, actors)
    EndIf
EndEvent

Event AAF:AAF_API.OnSceneEnd(AAF:AAF_API akSender, Var[] akArgs)
    Actor[] actors = Utility.VarToVarArray(akArgs[1]) as Actor[]

    Int ii = 0
    While (ii < actors.Length)
        AAFDynamicPositioner.RemoveHighlight(actors[ii])
        ii = ii + 1
    EndWhile

    AAFDynamicPositioner.SceneEnd(actors)
EndEvent