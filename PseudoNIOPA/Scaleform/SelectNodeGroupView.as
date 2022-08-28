package  {
	import UIComponent.View;
	import UIComponent.Button;
	import UIComponent.UIEvent;
	
	public class SelectNodeGroupView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 255;
		
		private var searchNodeBtn:Button;
		private var upperNodesBtn:Button;
		private var lowerNodesBtn:Button;
		private var fingerNodesBtn:Button;
		private var etcNodesBtn:Button;
		
		private var selectedNode:String;

		public function SelectNodeGroupView(selectedNode:String) {
			this.selectedNode = selectedNode;
        	super(menuWidth, menuHeight, "노드 선택");
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			var nextBtnPos:Number = 75;
			
			searchNodeBtn = new Button(menuWidth - 20, 30);
			searchNodeBtn.x = 10;
			searchNodeBtn.y = nextBtnPos;
			searchNodeBtn.text = "노드 탐색";
			searchNodeBtn.addEventListener(UIEvent.ACTIVATE, searchNodeBtn_Activated);
			this.AddComponent(searchNodeBtn);
			
			nextBtnPos += 35;
			
			upperNodesBtn = new Button(menuWidth - 20, 30);
			upperNodesBtn.x = 10;
			upperNodesBtn.y = nextBtnPos;
			upperNodesBtn.text = "Upper";
			upperNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(upperNodesBtn);
			
			nextBtnPos += 35;
			
			lowerNodesBtn = new Button(menuWidth - 20, 30);
			lowerNodesBtn.x = 10;
			lowerNodesBtn.y = nextBtnPos;
			lowerNodesBtn.text = "Lower";
			lowerNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(lowerNodesBtn);
			
			nextBtnPos += 35;
			
			fingerNodesBtn = new Button(menuWidth - 20, 30);
			fingerNodesBtn.x = 10;
			fingerNodesBtn.y = nextBtnPos;
			fingerNodesBtn.text = "Fingers";
			fingerNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(fingerNodesBtn);
			
			nextBtnPos += 35;
			
			etcNodesBtn = new Button(menuWidth - 20, 30);
			etcNodesBtn.x = 10;
			etcNodesBtn.y = nextBtnPos;
			etcNodesBtn.text = "ETC";
			etcNodesBtn.addEventListener(UIEvent.ACTIVATE, OnButtonActivated);
			this.AddComponent(etcNodesBtn);
		}
		
		private function searchNodeBtn_Activated(evn:UIEvent) : * {
			var nodeName:String;
			if (Shared.F4SEPlugin)
				nodeName = Shared.F4SEPlugin.GetNodeNameForSearch();
			else
				nodeName = "Root";
			
			if (!nodeName) {
				Shared.ShowMessageBox("오류", "선택된 액터의 노드를 가져올 수 없습니다");
				return;
			}
			var view = new SearchNodeView(nodeName);
			Shared.ShowView(view);
		}
		
		private function OnButtonActivated(evn:UIEvent) : * {
			var btn:Button = evn.currentTarget as Button;
			if (!btn)
				return;
			
			var node:String = btn.text;
			
			var view = new SelectNodeView(node, this.selectedNode);
			Shared.ShowView(view);
		}
	}
}