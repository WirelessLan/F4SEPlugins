package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.Button;
	import UIComponent.Label;
	import UIComponent.UIEvent;

	public class DeleteSaveView extends View {
		private var menuWidth:uint = 400;
		private var menuHeight:uint = 165;

		private var warningLbl:Label;
		private var saveNameLbl:Label;
		private var deleteSaveBtn:Button;
		private var cancelBtn:Button;

		private var saveName:String;

		public function DeleteSaveView(saveName:String) {
			this.saveName = saveName;

			var titleText = "_$PNIOPAMenuDelete";
			if (Shared.Localizations.$PNIOPAMenuDelete)
				titleText = Shared.Localizations.$PNIOPAMenuDelete;

			super(menuWidth, menuHeight, titleText);
		}

		protected override function InitializeView() : void {
			super.InitializeView();

			var warningLblText = "_$PNIOPAMenuConfirmDelete";
			if (Shared.Localizations.$PNIOPAMenuConfirmDelete)
				warningLblText = Shared.Localizations.$PNIOPAMenuConfirmDelete;
			warningLbl = new Label(menuWidth, 25, warningLblText, 20);
			warningLbl.x = 0;
			warningLbl.y = 60;
			this.AddComponent(warningLbl);

			saveNameLbl = new Label(menuWidth, 20, this.saveName, 16);
			saveNameLbl.x = 0;
			saveNameLbl.y = 90;
			this.AddComponent(saveNameLbl);

			var nextBtnPos:Number = 135;

			deleteSaveBtn = new Button(menuWidth - 20, 30);
			deleteSaveBtn.x = 10;
			deleteSaveBtn.y = nextBtnPos;
			var deleteSaveBtnText = "_$PNIOPAMenuDelete";
			if (Shared.Localizations.$PNIOPAMenuDelete)
				deleteSaveBtnText = Shared.Localizations.$PNIOPAMenuDelete;
			deleteSaveBtn.text = deleteSaveBtnText;
			deleteSaveBtn.addEventListener(UIEvent.ACTIVATE, deleteSaveBtn_Activated);
			this.AddComponent(deleteSaveBtn);

			nextBtnPos += 35;

			cancelBtn = new Button(menuWidth - 20, 30);
			cancelBtn.x = 10;
			cancelBtn.y = nextBtnPos;
			var cancelBtnText = "_$PNIOPAMenuCancel";
			if (Shared.Localizations.$PNIOPAMenuCancel)
				cancelBtnText = Shared.Localizations.$PNIOPAMenuCancel;
			cancelBtn.text = cancelBtnText;
			cancelBtn.addEventListener(UIEvent.ACTIVATE, cancelBtn_Activated);
			this.AddComponent(cancelBtn);
		}

		private function deleteSaveBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin) {
				var result:String = Shared.F4SEPlugin.DeleteSave(this.saveName);
				if (result.length != 0) {
					var errTitle = "_$PNIOPAMenuError";
					if (Shared.Localizations.$PNIOPAMenuError)
						errTitle = Shared.Localizations.$PNIOPAMenuError;

					var errMsg = "_$PNIOPAMenuDeleteError";
					if (Shared.Localizations.$PNIOPAMenuDeleteError)
						errMsg = Shared.Localizations.$PNIOPAMenuDeleteError;

					Shared.ShowMessageBox(errTitle, errMsg);
					return;
				}
			}

			Shared.CloseView(3);

			var view = new SelectSavesView();
			Shared.ShowView(view);
		}

		private function cancelBtn_Activated(evn:UIEvent) : * {
			Shared.CloseView(1);
		}
	}
}