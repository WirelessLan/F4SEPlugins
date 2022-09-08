package  {
	import UIComponent.View;
	import UIComponent.ListBox;
	import UIComponent.UIEvent;
	import UIComponent.Button;
	
	public class MainView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 255;
		
		private var selectNodeBtn:Button;
		private var saveBtn:Button;
		private var loadSaveBtn:Button;
		private var toggleFreezeBtn:Button;
		private var resetActorBtn:Button;
		
		private var selectedNode:String;

		public function MainView(selectedNode:String) {
			this.selectedNode = selectedNode;
        	super(menuWidth, menuHeight, "Pseudo NIOPA");
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			var nextBtnPos:Number = 75;
			
			selectNodeBtn = new Button(menuWidth - 20, 30);
			selectNodeBtn.x = 10;
			selectNodeBtn.y = nextBtnPos;
			selectNodeBtn.text = "노드 선택";
			selectNodeBtn.addEventListener(UIEvent.ACTIVATE, selectNodeBtn_Activated);
			this.AddComponent(selectNodeBtn);
			
			nextBtnPos += 35
			
			saveBtn = new Button(menuWidth - 20, 30);
			saveBtn.x = 10;
			saveBtn.y = nextBtnPos;
			saveBtn.text = "저장하기";
			saveBtn.addEventListener(UIEvent.ACTIVATE, saveBtn_Activated);
			this.AddComponent(saveBtn);
			
			nextBtnPos += 35
			
			loadSaveBtn = new Button(menuWidth - 20, 30);
			loadSaveBtn.x = 10;
			loadSaveBtn.y = nextBtnPos;
			loadSaveBtn.text = "불러오기";
			loadSaveBtn.addEventListener(UIEvent.ACTIVATE, loadSaveBtn_Activated);
			this.AddComponent(loadSaveBtn);
			
			nextBtnPos += 35
			
			toggleFreezeBtn = new Button(menuWidth - 20, 30);
			toggleFreezeBtn.x = 10;
			toggleFreezeBtn.y = nextBtnPos;
			toggleFreezeBtn.text = "액터 동결/해동";
			toggleFreezeBtn.addEventListener(UIEvent.ACTIVATE, toggleFreezeBtn_Activated);
			this.AddComponent(toggleFreezeBtn);
			
			nextBtnPos += 35
			
			resetActorBtn = new Button(menuWidth - 20, 30);
			resetActorBtn.x = 10;
			resetActorBtn.y = nextBtnPos;
			resetActorBtn.text = "액터 초기화";
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
			Shared.CloseMenu(0);
		}
		
		private function resetActorBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.ResetActor();
			Shared.CloseMenu(0);
		}
	}
}