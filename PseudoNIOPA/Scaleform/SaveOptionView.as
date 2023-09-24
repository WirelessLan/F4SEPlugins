package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Button;
	import UIComponent.UIEvent;

	public class SaveOptionView extends View {
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 140;

		private var saveNameLbl:Label;
		private var loadSaveBtn:Button;
		private var deleteSaveBtn:Button;

		private var saveName:String;

		public function SaveOptionView(saveName:String) {
			this.saveName = saveName;

			var titleText = "_$PNIOPAMenuLoadSaveOption";
			if (Shared.Localizations.$PNIOPAMenuLoadSaveOption)
				titleText = Shared.Localizations.$PNIOPAMenuLoadSaveOption;

			super(menuWidth, menuHeight, titleText);
		}

		protected override function InitializeView() : void {
			super.InitializeView();

			saveNameLbl = new Label(menuWidth, 20, this.saveName, 16);
			saveNameLbl.x = 0;
			saveNameLbl.y = 60;
			this.AddComponent(saveNameLbl);

			var nextBtnPos:Number = 110;

			loadSaveBtn = new Button(menuWidth - 20, 30);
			loadSaveBtn.x = 10;
			loadSaveBtn.y = nextBtnPos;
			var loadSaveBtnText = "_$PNIOPAMenuLoadSave";
			if (Shared.Localizations.$PNIOPAMenuLoadSave)
				loadSaveBtnText = Shared.Localizations.$PNIOPAMenuLoadSave;
			loadSaveBtn.text = loadSaveBtnText;
			loadSaveBtn.addEventListener(UIEvent.ACTIVATE, loadSaveBtn_Activated);
			this.AddComponent(loadSaveBtn);

			nextBtnPos += 35;

			deleteSaveBtn = new Button(menuWidth - 20, 30);
			deleteSaveBtn.x = 10;
			deleteSaveBtn.y = nextBtnPos;
			var deleteSaveBtnText = "_$PNIOPAMenuDelete";
			if (Shared.Localizations.$PNIOPAMenuDelete)
				deleteSaveBtnText = Shared.Localizations.$PNIOPAMenuDelete;
			deleteSaveBtn.text = deleteSaveBtnText;
			deleteSaveBtn.addEventListener(UIEvent.ACTIVATE, deleteSaveBtn_Activated);
			this.AddComponent(deleteSaveBtn);
		}

		private function loadSaveBtn_Activated(evn:UIEvent) : * {
			if (!Shared.F4SEPlugin) {
				Shared.CloseView(0);
				return;
			}
			
			var result:String = Shared.F4SEPlugin.LoadSave(this.saveName);
			if (result.length != 0) {
				var errTitle = "_$PNIOPAMenuError"
				if (Shared.Localizations.$PNIOPAMenuError)
					errTitle = Shared.Localizations.$PNIOPAMenuError;

				var errMsg = "";

				if (result == "ESAVENAME" || result == "ENOPATH") {
					errMsg = "_$PNIOPAMenuWrongSaveNameError";
					if (Shared.Localizations.$PNIOPAMenuWrongSaveNameError)
						errMsg = Shared.Localizations.$PNIOPAMenuWrongSaveNameError;
				}
				else {
					errMsg = "_$PNIOPAMenuCannotLoadError";
					if (Shared.Localizations.$PNIOPAMenuCannotLoadError)
						errMsg = Shared.Localizations.$PNIOPAMenuCannotLoadError;
				}

				Shared.ShowMessageBox(errTitle, 'errMsg');
				return;
			}
			
			Shared.CloseView(0);
		}

		private function deleteSaveBtn_Activated(evn:UIEvent) : * {
			var view = new DeleteSaveView(this.saveName);
			Shared.ShowView(view);
		}
	}
}