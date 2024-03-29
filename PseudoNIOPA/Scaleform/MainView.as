﻿package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	import UIComponent.Button;

	public class MainView extends View {
		private var menuWidth:uint = 320;
		private var menuHeight:uint = 640;

		private var selectNodeBtn:Button;
		private var saveBtn:Button;
		private var loadSaveBtn:Button;
		private var toggleFreezeBtn:Button;
		private var resetActorBtn:Button;

		private var selectedNode:String;

		public function MainView(selectedNode:String) {
			this.selectedNode = selectedNode;

			var titleText = "_$PNIOPAMenuMainTitle";
			if (Shared.Localizations.$PNIOPAMenuMainTitle)
				titleText = Shared.Localizations.$PNIOPAMenuMainTitle;

			super(menuWidth, menuHeight, titleText);
		}

		protected override function InitializeView() : void {
			super.InitializeView();

			this.x = 20;
			this.y = 10;

			var nextBtnPos:Number = 55;

			selectNodeBtn = new Button(menuWidth - 20, 30);
			selectNodeBtn.x = 10;
			selectNodeBtn.y = nextBtnPos;
			var selectNodeBtnText = "_$PNIOPAMenuSelectNode";
			if (Shared.Localizations.$PNIOPAMenuSelectNode)
				selectNodeBtnText = Shared.Localizations.$PNIOPAMenuSelectNode;
			selectNodeBtn.text = selectNodeBtnText;
			selectNodeBtn.addEventListener(UIEvent.ACTIVATE, selectNodeBtn_Activated);
			this.AddComponent(selectNodeBtn);

			nextBtnPos += 35

			saveBtn = new Button(menuWidth - 20, 30);
			saveBtn.x = 10;
			saveBtn.y = nextBtnPos;
			var saveBtnText = "_$PNIOPAMenuSave";
			if (Shared.Localizations.$PNIOPAMenuSave)
				saveBtnText = Shared.Localizations.$PNIOPAMenuSave;
			saveBtn.text = saveBtnText;
			saveBtn.addEventListener(UIEvent.ACTIVATE, saveBtn_Activated);
			this.AddComponent(saveBtn);

			nextBtnPos += 35

			loadSaveBtn = new Button(menuWidth - 20, 30);
			loadSaveBtn.x = 10;
			loadSaveBtn.y = nextBtnPos;
			var loadSaveBtnText = "_$PNIOPAMenuLoadSave";
			if (Shared.Localizations.$PNIOPAMenuLoadSave)
				loadSaveBtnText = Shared.Localizations.$PNIOPAMenuLoadSave;
			loadSaveBtn.text = loadSaveBtnText;
			loadSaveBtn.addEventListener(UIEvent.ACTIVATE, loadSaveBtn_Activated);
			this.AddComponent(loadSaveBtn);

			nextBtnPos += 35

			toggleFreezeBtn = new Button(menuWidth - 20, 30);
			toggleFreezeBtn.x = 10;
			toggleFreezeBtn.y = nextBtnPos;
			var toggleFreezeBtnText = "_$PNIOPAMenuToggleFreeze";
			if (Shared.Localizations.$PNIOPAMenuToggleFreeze)
				toggleFreezeBtnText = Shared.Localizations.$PNIOPAMenuToggleFreeze;
			toggleFreezeBtn.text = toggleFreezeBtnText;
			toggleFreezeBtn.addEventListener(UIEvent.ACTIVATE, toggleFreezeBtn_Activated);
			this.AddComponent(toggleFreezeBtn);

			nextBtnPos += 35

			resetActorBtn = new Button(menuWidth - 20, 30);
			resetActorBtn.x = 10;
			resetActorBtn.y = nextBtnPos;
			var resetActorBtnText = "_$PNIOPAMenuResetActor";
			if (Shared.Localizations.$PNIOPAMenuResetActor)
				resetActorBtnText = Shared.Localizations.$PNIOPAMenuResetActor;
			resetActorBtn.text = resetActorBtnText;
			resetActorBtn.addEventListener(UIEvent.ACTIVATE, resetActorBtn_Activated);
			this.AddComponent(resetActorBtn);
		}

		private function selectNodeBtn_Activated(evn:UIEvent) : * {
			var view = new SelectNodeGroupView(this.selectedNode);
			Shared.ShowView(view);
		}

		private function saveBtn_Activated(evn:UIEvent) : * {
			var view = new SaveView();
			Shared.ShowView(view);
		}

		private function loadSaveBtn_Activated(evn:UIEvent) : * {
			var view = new SelectSavesView();
			Shared.ShowView(view);
		}

		private function toggleFreezeBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.ToggleFreeze();
			Shared.CloseView(0);
		}

		private function resetActorBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.ResetActor();
			Shared.CloseView(0);
		}
	}
}