package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Button;
	import UIComponent.TextInput;
	import UIComponent.UIEvent;
	
	public class SaveView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 165;
		
		private var descLbl:Label;
		private var inputSaveName:TextInput;
		private var saveBtn:Button;

		public function SaveView() {
        	super(menuWidth, menuHeight, "저장하기");
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
			var saveName:String = inputSaveName.text.replace(/^\s+|\s+$|\\/g, '');
			if (saveName.length == 0) {
				Shared.ShowMessageBox('오류', '저장할 이름을 입력해주세요');
				return;
			}
			
			if (Shared.F4SEPlugin) {
				var result:String = Shared.F4SEPlugin.Save(saveName);
				if (result.length != 0) {
					if (result == Shared.ENOACTOR)
						Shared.ShowMessageBox('오류', '선택된 액터가 없습니다');
					else if (result == Shared.ESAVENAME)
						Shared.ShowMessageBox('오류', '저장할 이름을 입력해주세요');
					else if (result == Shared.ENOMOD)
						Shared.ShowMessageBox('오류', '수정된 노드가 없습니다');
					else if (result == Shared.ENOPATH)
						Shared.ShowMessageBox('오류', '잘못된 이름입니다');
					else
						Shared.ShowMessageBox('오류', '저장할 수 없습니다');
					return;
				}
			}
			
			Shared.CloseMenu(1);
		}
	}
}