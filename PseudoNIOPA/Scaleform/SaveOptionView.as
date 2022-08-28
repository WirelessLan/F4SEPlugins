package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class SaveOptionView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 185;
		
		private var saveNameLbl:Label;
		private var loadSaveBtn:Button;
		private var deleteSaveBtn:Button;
		
		private var saveName:String;

		public function SaveOptionView(saveName:String) {
			this.saveName = saveName;
        	super(menuWidth, menuHeight, "불러오기 옵션");
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
			loadSaveBtn.text = "불러오기";
			loadSaveBtn.addEventListener(UIEvent.ACTIVATE, loadSaveBtn_Activated);
			this.AddComponent(loadSaveBtn);
			
			nextBtnPos += 35;
			
			deleteSaveBtn = new Button(menuWidth - 20, 30);
			deleteSaveBtn.x = 10;
			deleteSaveBtn.y = nextBtnPos;
			deleteSaveBtn.text = "삭제";
			deleteSaveBtn.addEventListener(UIEvent.ACTIVATE, deleteSaveBtn_Activated);
			this.AddComponent(deleteSaveBtn);
		}
		
		private function loadSaveBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin) {
				var result:String = Shared.F4SEPlugin.LoadSave(this.saveName);
				if (result.length != 0) {
					if (result == Shared.ENOACTOR)
						Shared.ShowMessageBox('오류', '선택된 액터가 없습니다');
					else if (result == Shared.ESAVENAME || result == Shared.ENOPATH)
						Shared.ShowMessageBox('오류', '잘못된 파일 이름입니다');
					else
						Shared.ShowMessageBox('오류', '불러올 수 없습니다');
					return;
				}
			}
			Shared.CloseMenu(0);
		}
		
		private function deleteSaveBtn_Activated(evn:UIEvent) : * {
			var view = new DeleteSaveView(this.saveName);
			Shared.ShowView(view);
		}
	}
}