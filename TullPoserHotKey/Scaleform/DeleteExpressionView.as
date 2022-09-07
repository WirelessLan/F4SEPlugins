package  {
	import UIComponent.View;
	import UIComponent.Label;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class DeleteExpressionView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 210;
		
		private var warningLbl:Label;
		private var saveNameLbl:Label;
		private var deleteSaveBtn:Button;
		private var cancelBtn:Button;
		
		private var saveName:String;

		public function DeleteExpressionView(saveName:String) {
			this.saveName = saveName;
        	super(menuWidth, menuHeight, "표정 삭제");
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			warningLbl = new Label(menuWidth, 25, "정말로 이 표정을 삭제하시겠습니까?", 20);
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
			deleteSaveBtn.text = "삭제";
			deleteSaveBtn.addEventListener(UIEvent.ACTIVATE, deleteSaveBtn_Activated);
			this.AddComponent(deleteSaveBtn);
			
			nextBtnPos += 35;
			
			cancelBtn = new Button(menuWidth - 20, 30);
			cancelBtn.x = 10;
			cancelBtn.y = nextBtnPos;
			cancelBtn.text = "취소";
			cancelBtn.addEventListener(UIEvent.ACTIVATE, cancelBtn_Activated);
			this.AddComponent(cancelBtn);
		}
		
		private function deleteSaveBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin) {
				if (!Shared.F4SEPlugin.DeleteExpression(this.saveName)) {
					Shared.ShowMessageBox('오류', "표정을 삭제할 수 없습니다");
					return;
				}
			}
			
			Shared.CloseMenu(3);
			var view = new SelectExpressionsView();
			Shared.ShowView(view);
		}
		
		private function cancelBtn_Activated(evn:UIEvent) : * {
			Shared.CloseMenu(1);
		}
	}
}