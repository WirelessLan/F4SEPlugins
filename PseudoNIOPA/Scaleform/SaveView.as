package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Button;
	import UIComponent.TextInput;
	import UIComponent.UIEvent;

	public class SaveView extends View {
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 120;

		private var descLbl:Label;
		private var inputSaveName:TextInput;
		private var saveBtn:Button;

		public function SaveView() {
			var titleText = "_$PNIOPAMenuSave";
			if (Shared.Localizations.$PNIOPAMenuSave)
				titleText = Shared.Localizations.$PNIOPAMenuSave;

			super(menuWidth, menuHeight, titleText);
		}

		protected override function InitializeView() : void {
			super.InitializeView();

			var descLblText = "_$PNIOPAMenuSaveDesc";
			if (Shared.Localizations.$PNIOPAMenuSaveDesc)
				descLblText = Shared.Localizations.$PNIOPAMenuSaveDesc;
			descLbl = new Label(menuWidth, 30, descLblText, 18);
			descLbl.x = 0;
			descLbl.y = 50;
			this.AddComponent(descLbl);

			inputSaveName = new TextInput(menuWidth - 20, 40, 30);
			inputSaveName.x = 10;
			inputSaveName.y = 80;
			this.AddComponent(inputSaveName);

			saveBtn = new Button(menuWidth - 20, 30);
			saveBtn.x = 10;
			saveBtn.y = 125;
			var saveBtnText = "_$PNIOPAMenuSaveBtn";
				if (Shared.Localizations.$PNIOPAMenuSaveBtn)
			saveBtnText = Shared.Localizations.$PNIOPAMenuSaveBtn;
			saveBtn.text = saveBtnText;
			saveBtn.addEventListener(UIEvent.ACTIVATE, saveBtn_Activated);
			this.AddComponent(saveBtn);
		}

		private function saveBtn_Activated(evn:UIEvent) : * {
			var saveName:String = inputSaveName.text.replace(/^\s+|\s+$/g, '');
			if (saveName.length == 0) {
				var errorTitle = "_$PNIOPAMenuError";
				if (Shared.Localizations.$PNIOPAMenuError)
					errorTitle = Shared.Localizations.$PNIOPAMenuError;

				var errMsg = "_$PNIOPAMenuNoSaveNameError";
				if (Shared.Localizations.$PNIOPAMenuNoSaveNameError)
					errMsg = Shared.Localizations.$PNIOPAMenuNoSaveNameError;

				Shared.ShowMessageBox(errorTitle, errMsg);
				return;
			}

			var invalidChars:String = "<>:\"/\\|?*";
			for (var ii = 0; ii < invalidChars.length; ii++) {
				if (saveName.indexOf(invalidChars.charAt(ii)) >= 0) {
					errorTitle = "_$PNIOPAMenuError";
					if (Shared.Localizations.$PNIOPAMenuError)
						errorTitle = Shared.Localizations.$PNIOPAMenuError;

					errMsg = "_$PNIOPAMenuWrongSaveNameError";
					if (Shared.Localizations.$PNIOPAMenuWrongSaveNameError)
						errMsg = Shared.Localizations.$PNIOPAMenuWrongSaveNameError;

					Shared.ShowMessageBox(errorTitle, errMsg);
					return;
				}
			}

			if (Shared.F4SEPlugin) {
				var result:String = Shared.F4SEPlugin.Save(saveName);
				if (result.length != 0) {
					errorTitle = "_$PNIOPAMenuError";
					if (Shared.Localizations.$PNIOPAMenuError)
						errorTitle = Shared.Localizations.$PNIOPAMenuError;

					errMsg = "";

					if (result == "ESAVENAME") {
						errMsg = "_$PNIOPAMenuNoSaveNameError";
						if (Shared.Localizations.$PNIOPAMenuNoSaveNameError)
							errMsg = Shared.Localizations.$PNIOPAMenuNoSaveNameError;
					}
					else if (result == "ENOMOD") {
						errMsg = "_$PNIOPAMenuNoModifiedNodeError";
						if (Shared.Localizations.$PNIOPAMenuNoModifiedNodeError)
							errMsg = Shared.Localizations.$PNIOPAMenuNoModifiedNodeError;
					}
					else if (result == "ENOPATH") {
						errMsg = "_$PNIOPAMenuWrongSaveNameError";
						if (Shared.Localizations.$PNIOPAMenuWrongSaveNameError)
							errMsg = Shared.Localizations.$PNIOPAMenuWrongSaveNameError;
					}
					else {
						errMsg = "_$PNIOPAMenuCannotSaveError";
						if (Shared.Localizations.$PNIOPAMenuCannotSaveError)
							errMsg = Shared.Localizations.$PNIOPAMenuCannotSaveError;
					}

					Shared.ShowMessageBox(errorTitle, errMsg);
					return;
				}
			}

			Shared.CloseView(1);
		}
	}
}