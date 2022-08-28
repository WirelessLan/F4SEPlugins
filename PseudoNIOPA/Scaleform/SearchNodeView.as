package  {
	import UIComponent.View;
	import UIComponent.Button;
	import UIComponent.Label;
	import UIComponent.UIEvent;
	
	public class SearchNodeView extends View {
		private var menuWidth:uint = 350;
		private var menuHeight:uint = 220;
		
		private var nodeNameLbl:Label;
		private var selectNodeBtn:Button;
		private var selectParentNodeBtn:Button;
		private var selectChildNodesBtn:Button;
		
		private var nodeName:String;

		public function SearchNodeView(nodeName:String) {
			this.nodeName = nodeName;
        	super(menuWidth, menuHeight, "노드 탐색");
		}
		
		protected override function InitializeView() : void {
			super.InitializeView();
			
			nodeNameLbl = new Label(menuWidth, 20, this.nodeName, 16);
			nodeNameLbl.x = 0;
			nodeNameLbl.y = 60;
			this.AddComponent(nodeNameLbl);

			var nextBtnPos:Number = 110;
			
			selectNodeBtn = new Button(menuWidth - 20, 30);
			selectNodeBtn.x = 10;
			selectNodeBtn.y = nextBtnPos;
			selectNodeBtn.text = "현재 노드 선택";
			selectNodeBtn.addEventListener(UIEvent.ACTIVATE, selectNodeBtn_Activated);
			this.AddComponent(selectNodeBtn);
			
			nextBtnPos += 35;
			
			selectChildNodesBtn = new Button(menuWidth - 20, 30);
			selectChildNodesBtn.x = 10;
			selectChildNodesBtn.y = nextBtnPos;
			selectChildNodesBtn.text = "자식노드 조회";
			selectChildNodesBtn.addEventListener(UIEvent.ACTIVATE, selectChildNodesBtn_Activated);
			this.AddComponent(selectChildNodesBtn);
			
			nextBtnPos += 35;
			
			selectParentNodeBtn = new Button(menuWidth - 20, 30);
			selectParentNodeBtn.x = 10;
			selectParentNodeBtn.y = nextBtnPos;
			selectParentNodeBtn.text = "부모노드로 이동";
			selectParentNodeBtn.addEventListener(UIEvent.ACTIVATE, selectParentNodeBtn_Activated);
			this.AddComponent(selectParentNodeBtn);
		}
		
		private function selectNodeBtn_Activated(evn:UIEvent) : * {
			if (Shared.F4SEPlugin)
				Shared.F4SEPlugin.SelectNode(this.nodeName);
			Shared.CloseMenu(0);
		}
		
		private function selectParentNodeBtn_Activated(evn:UIEvent) : * {
			var parentNode:String = Shared.F4SEPlugin.GetParentNodeName(this.nodeName);
			if (!parentNode) {
				Shared.ShowMessageBox('오류', "부모 노드가 존재하지 않습니다");
				return;
			}
			
			Shared.CloseMenu(1);
			
			var view = new SearchNodeView(parentNode);
			Shared.ShowView(view);
		}
		
		private function selectChildNodesBtn_Activated(evn:UIEvent) : * {
			var childNodes:Array;
			if (Shared.F4SEPlugin)
				childNodes = Shared.F4SEPlugin.GetChildNodes(this.nodeName);
			else
				childNodes = new Array();
			
			var view = new SelectChildNodesView(childNodes);
			Shared.ShowView(view);
		}
	}
}