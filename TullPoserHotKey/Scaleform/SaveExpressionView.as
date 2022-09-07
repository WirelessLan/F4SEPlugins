package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.TextInput;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class SaveExpressionView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 165;
		
		private var descLbl:Label;
		private var inputSaveName:TextInput;
		private var saveBtn:Button;

		public function SaveExpressionView() {
        	super(menuWidth, menuHeight, "표정 저장");
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			descLbl = new Label(menuWidth, 30, "저장할 파일 이름을 입력하세요", 18);
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
			saveBtn.text = "저장";
			saveBtn.addEventListener(UIEvent.ACTIVATE, saveBtn_Activated);
			this.AddComponent(saveBtn);
		}
		
		private function saveBtn_Activated(evn:UIEvent) : * {
			var saveName:String = inputSaveName.text.replace(/^\s+|\s+$/g, '');
			if (saveName.length == 0) {
				Shared.ShowMessageBox('오류', '저장할 이름을 입력해주세요');
				return;
			}
			
			var invalidChars:String = "<>:\"/\\|?*";
			for (var ii = 0; ii < invalidChars.length; ii++) {
				if (saveName.indexOf(invalidChars.charAt(ii)) >= 0) {
					Shared.ShowMessageBox('오류', '잘못된 파일 이름입니다');
					return;
				}
			}
			
			if (Shared.F4SEPlugin) {
				if (!Shared.F4SEPlugin.SaveExpression(saveName)) {
					Shared.ShowMessageBox('오류', '표정을 저장할 수 없습니다');
					return;
				}
			}
			
			Shared.CloseMenu(1);
		}
	}
}