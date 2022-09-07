package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class LoadExpressionView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 185;
		
		private var saveNameLbl:Label;
		private var loadSaveBtn:Button;
		private var deleteSaveBtn:Button;
		
		private var saveName:String;

		public function LoadExpressionView(saveName:String) {
			this.saveName = saveName;
        	super(menuWidth, menuHeight, "표정 불러오기");
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
				if (!Shared.F4SEPlugin.LoadExpression(this.saveName)) {
					Shared.ShowMessageBox('오류', '표정을 불러올 수 없습니다');
					return;
				}
			}
			Shared.CloseMenu(0);
		}
		
		private function deleteSaveBtn_Activated(evn:UIEvent) : * {
			var view = new DeleteExpressionView(this.saveName);
			Shared.ShowView(view);
		}
	}
}